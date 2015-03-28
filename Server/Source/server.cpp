#include "server.h"

Server::Server(int port, QObject *parent = 0) : QObject(parent) {
	myPort = port;
	std::cout << "port is: " << myPort << std::endl;
	clientThreadList = new std::vector<ClientCommThread *>;
	incoming = new QTcpServer(parent);

	for (int i = 0; i < MAXPLAYERS; ++i) {
		selectedCharacters[i] = false;
		recTurns[i] = NULL;
	}

	QObject::connect(incoming,
		SIGNAL(newConnection()),
		this,
		SLOT(handleIncomingUsers()));
}

Server::~Server() {

}

void Server::run() {
	std::cout << "Starting up the server" << std::endl;
	bool cheatMode = false;
	cout << "Cheat Mode? (0/1)" <<endl;
	cin >> cheatMode;
	incoming->listen(QHostAddress::Any, (quint16) myPort);
	std::cout << "listening for connections" << std::endl;
    game.setupGame(cheatMode);
    game.runGame();
}

void Server::handleIncomingUsers()  {
	std::cout << "new user incoming" << std::endl;
		
	if(incoming->hasPendingConnections()) {
		QTcpSocket *newClient = incoming->nextPendingConnection();
		if (clientThreadList->size() >= MAXPLAYERS) {
			std::cout << "new user has been rejected" << std::endl;
			newClient->write(DECLINECONN);
			newClient->close();
		} else {
			ClientCommThread *newThread = new ClientCommThread(newClient, this,
				clientThreadList->size());
			connect(newThread, SIGNAL(characterSelected(CharacterType, int)),
				this, SLOT(characterUnavail(CharacterType, int)));
			connect(newThread, SIGNAL(spawnSelected(DwellingType, int)),
				this, SLOT(setSpawn(DwellingType, int)));
			connect(newThread, SIGNAL(incomingTurn(QString&, int)), 
				this, SLOT(recordedTurn(QString&, int)));
			connect(newThread, SIGNAL(searchTypeReturned()),
				this, SLOT(endAction()));
			clientThreadList->push_back(newThread);
			std::cout << "new user has been accepted" << std::endl;
			stringstream s;
			s << ACCEPTCONN;
			s << CLASSDELIM;
			s << game.getTime();
			newThread->writeMessage(new string(s.str()));	//send the connection OK message
			sendBoard(newThread);

			//Inform new player of currently selected characters
			for (int i = 0; i < MAXPLAYERS; ++i) {
				if (selectedCharacters[i]) {
					stringstream s;
					s << "CharacterType";
					s << CLASSDELIM;
					s << i;
					newThread->writeMessage(new string(s.str()));
				}
			}
			for (int i = 0; i < MAXPLAYERS; ++i) {
				Character *character = game.getPlayer((CharacterType) i);
				if (character != NULL) {
					newThread->writeMessage(character->serialize());
					stringstream s;
					s << "MoveCharacter";
					s << CLASSDELIM;
					s << i;
					s << VARDELIM;
					s << *(character->getCurrentLocation()->toString());
					newThread->writeMessage(new string(s.str()));
				}
			}
			//todo send player locations to new players
		}
	}
}

void Server::characterUnavail(CharacterType type, int clientID) {
	stringstream s;
	s << "CharacterType";
	s << CLASSDELIM;
	s << type;
	
	stringstream s2;
	s2 << "Selection";
	s2 << CLASSDELIM;

	//check if character is in use
	if (selectedCharacters[(int) type]) {
		s2 << false;
	} else {
		s2 << true;
		selectedCharacters[(int) type] = true;
		game.addPlayer(type);
		writeMessageAllClients(new string(s.str()));
		writeMessageAllClients(game.getPlayer(type)->serialize());
	}
	string *message = new string(s2.str());;
	clientThreadList->at(clientID)->writeMessage(message);
}

void Server::setSpawn(DwellingType type, int clientID) {
	moveCharacter(game.getPlayer(clientThreadList->at(clientID)->getMyCharacter()),
		game.getDwelling(type)->getLocation());

	calculatePlayerTurnPhases(clientThreadList->at(clientID));
}

void Server::moveCharacter(Character *character, Clearing *dest) {
	game.moveRequest(character, dest);

	stringstream s;

	s << "MoveCharacter";
	s << CLASSDELIM;
	s << character->getType();
	s << VARDELIM;
	s << *(dest->toString());
	writeMessageAllClients(new string(s.str()));
}

void Server::writeMessageAllClients(string *message) {
	writeMessageAllClients(new QString(message->c_str()));
	delete message;
}

void Server::writeMessageAllClients(QString *message) {
	for (vector<ClientCommThread*>::iterator it = clientThreadList->begin();
		it != clientThreadList->end(); ++it) {
			(*it)->writeMessage(message);
	}
}

/*
We have received a recorded turn for the given client
*/
void Server::recordedTurn(QString &turn, int clientID) {
	string *s = new string(turn.toUtf8().constData());
	RecordedTurn *recTurn = new RecordedTurn(s, game.getBoard());
	recTurns[clientID] = recTurn;
	int count = 0;
	for (int i = 0; i < MAXPLAYERS; ++i) {
		if (recTurns[i] != NULL)
			++count;
	}
	if (count == clientThreadList->size()) {
		//stop accepting connections
		incoming->pauseAccepting();
		qDebug() << "no longer accepting connections";
		//start game at sunrise
		sunrise();
	}
}

/*
Creates a RecordedTurn object with the number of each type of phase
sends the serialized version of the object to the client

Birdsong stage of game
*/
void Server::calculatePlayerTurnPhases(ClientCommThread *client) {
	Character *player = game.getPlayer(client->getMyCharacter());
	RecordedTurn turn;
	turn.addPhase(BasicPhase, 2); //all players get two basic phases.
	if (player->hasAdvantage(STAMINA)) {
		turn.addPhase(MovePhase);
	}
	if (!player->hasAdvantage(SHORTLEGS)) {
		//if not in caves
		if (player->getCurrentLocation()->getClearingType() != CAVES) {
			turn.addPhase(SunlightPhase, 2);
		}
	}
	if (player->hasAdvantage(ELUSIVNESS)) {
		turn.addPhase(HidePhase);
	}
	if (player->hasAdvantage(REPUTATION)) {
		if (player->getCurrentLocation()->getDwelling() != NULL) {
			turn.addPhase(BasicPhase);
		}
	}

	client->writeMessage(turn.serialize());
}

void Server::birdsong() {
	qDebug() << "birdsong begins";
	for (vector<ClientCommThread*>::iterator it = clientThreadList->begin();
		it != clientThreadList->end(); ++it)
	{
		calculatePlayerTurnPhases((*it));
	}
}

//Determine what monsters are prwoling
void Server::sunrise() {
	qDebug() << "sunrise begins";
	int roll = game.rollDice();
    daylight();
}

//play player turns in random order
void Server::daylight() {
	
	qDebug() << "unhiding all players";
	for (vector<ClientCommThread*>::iterator it = clientThreadList->begin();
		it != clientThreadList->end(); ++it) {
			if(game.getPlayer((*it)->getMyCharacter())->isHidden()) {
				game.getPlayer((*it)->getMyCharacter())->toggleHide();
				stringstream s;
				s << "Hidden";
				s << CLASSDELIM;
				s << (*it)->getMyCharacter();
				s << VARDELIM;
				s << false;
				writeMessageAllClients(new string(s.str()));
			}
	}
	startPlayerTurn();
}

void Server::startPlayerTurn() {
	currentPlayer = 0;
	do {
		currentPlayer = game.rollDice() -1;
		qDebug() << "rolled: " << currentPlayer;
		qDebug() << "array value: " << recTurns[currentPlayer];
	} while(recTurns[currentPlayer] == NULL);
	qDebug() << "selected player: " << currentPlayer;
	currentAction = recTurns[currentPlayer]->getActions()->begin();
	if (currentAction != recTurns[currentPlayer]->getActions()->end()) {
		startAction();
	} else {
		endPlayerTurn();
	}
}

void Server::startAction() {
	qDebug() << "performing action";
	stringstream s;
	Character *character = game.getPlayer(clientThreadList->at(currentPlayer)->getMyCharacter());
	switch ((*currentAction)->getAction()) {
	case MoveAction: 
		moveCharacter(character, (*currentAction)->getTarget());
		endAction();
		break;
	case SearchAction: 
		s << "SearchTypeReq";
		s << CLASSDELIM;
		s << PEER;
		s << VARDELIM;
		s << LOCATE;
		if (game.canLoot(character)) {
			s << VARDELIM;
			s << LOOT;
		}
		clientThreadList->at(currentPlayer)->writeMessage(new string(s.str()));
		qDebug() << "wating for response from client";
		break;
	case TradeAction: break; //Not implemented yet
	case HideAction: 
		bool result = game.hideRequest(character);
		s << "Hidden";
		s << CLASSDELIM;
		s << clientThreadList->at(currentPlayer)->getMyCharacter();
		s << VARDELIM;
		s << result;
		writeMessageAllClients(new string(s.str()));
		endAction();
		break;
	}
}

void Server::endAction() {
	SearchType sType;
	Character *character = game.getPlayer(clientThreadList->at(currentPlayer)->getMyCharacter());
	switch ((*currentAction)->getAction())
	{
	case SearchAction:
		sType = clientThreadList->at(currentPlayer)->getSearchTypeResult();
		searchClearing(character, sType, (*currentAction)->getTarget());
		break;
	}

	delete (*currentAction);
	++currentAction;
	if (currentAction != recTurns[currentPlayer]->getActions()->end()) {
		startAction();
	} else {
		endPlayerTurn();
	}
}

void Server::endPlayerTurn() {
	delete recTurns[currentPlayer];
	recTurns[currentPlayer] = NULL;
	if (turnExists()) {
		startPlayerTurn();
	} else {
		sunset();
	}
}

bool Server::turnExists() {
	for (int i = 0; i < MAXPLAYERS; ++i) {
		if (recTurns[i] != NULL)
			return true;
	}
	return false;
}

void Server::sunset() {
	qDebug() << "sunset begins";
    evening();
}

//randomly select clearings with players in them
//player can rearrange belongings (not needed for iter 1)
//combat?
void Server::evening() {
	qDebug() << "evening begins";
    midnight();
}

//reset all face up mapchits
//start new day
void Server::midnight() {
	qDebug() << "midnight begins";
	birdsong();
}
/*
Searches the players current location
*/
void Server::searchClearing(Character *character, SearchType type, Clearing *target) {
	DiscoveryType discover = DISCOVER_NOTHING;
	stringstream s;
	Treasure* found = NULL;
	switch(type) {
	case PEER: 
		discover = game.searchPeerRequest();
		break;
	case LOCATE: 
		discover = game.searchLocateRequest();
		break;
	case LOOT: 
		found = game.searchLootRequest(character);
		int worth;
		if (found != NULL) {
			worth = found->getWorth();
			character->addGold(worth);
		}
		s << "TreasureFound";
		s << CLASSDELIM;
		s << worth;
		s << VARDELIM;
		s << character->getType();
		break;
	}
	Chit* site = NULL;
	switch(discover) {
	case DISCOVER_HENEMIES: break;
	case DISCOVER_ENEMAPATH: 
	case DISCOVER_HPATH: break;
	case DISCOVER_PATHACLUES: 
	case DISCOVER_CLUES: break;
	case DISCOVER_PASSACLUES: 
	case DISCOVER_SPASS: break;
	case DISCOVER_CHIT: 
		site = target->getTile()->getSiteOrSoundChit();
		//bug we do not handle LOST CITY / LOST CASTLE here...
		if (site->getType() == CHIT_LOST) {
			int i = 0;
			vector<Chit*> *contents = site->getContents();
			do {
				site = contents->at(i);
				++i;
			} while((i < contents->size()) 
				&& ((site->getType() != CHIT_SITE)
				&&(site->getClearingNum() != character->getCurrentLocation()->getClearingNum())));
		}
		if (site != NULL && site->getType() == CHIT_SITE) {
			character->discover(site);
			s << "SiteFound";
			s << CLASSDELIM;
			s << site->getName();
			s << VARDELIM;
			s << character->getType();
		}
		break;
	case DISCOVER_CHOICE: break;
	}
	writeMessageAllClients(new string(s.str()));
	//TODO handle result of search and send message
}

/*
Attempt to the player at the current location
*/
void Server::hidePlayer(Character *character) {
	bool result = game.hideRequest(character);
	stringstream s;
	s << "Hidden";
	s << CLASSDELIM;
	s << character->getType();
	s << VARDELIM;
	s << result;

	writeMessageAllClients(new string(s.str()));
}

void Server::sendBoard(ClientCommThread* client) {

	//TODO get all tiles, that might be all todo
	vector<Tile*> tiles = *(game.getBoard()->getTiles());
	for (vector<Tile*>::iterator it = tiles.begin(); it != tiles.end(); ++it) 
	{
		stringstream s;
		Chit* chit = (*it)->getSiteOrSoundChit();
		if (chit == NULL)
			continue;
		s << *(chit->serialize());
		s << "Tile";
		s << CLASSDELIM;
		s << (*it)->getName(); //name of tile
		if (chit->getType() == CHIT_LOST) 
		{ //LOST CITY, has multiple inner chits
			//TODO need to mark all of the contained chits as such, so they get 
			//added to the correct LOSTCHIT
			vector <Chit*> contents = (*chit->getContents());
			for (vector<Chit*>::iterator chiter = contents.begin(); 
				chiter != contents.end(); ++chiter) 
			{
				s << LISTDELIM;
				s << *((*chiter)->serialize());
			}
		}
		client->writeMessage(new string(s.str()));
	}
}