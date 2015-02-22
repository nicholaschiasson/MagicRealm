#ifndef SERVER_H
#define SERVER_H

#include <QtNetwork>
#include <QtCore>
#include <iostream>
#include <sstream>
#include <vector>
#include "clientCommThread.h"
#include "shared.h"
#include "game.h"
#include "recordedTurn.h"

class ClientCommThread;

class Server : public QObject{
	Q_OBJECT
public:
	Server(int port, QObject *parent);
	~Server();
public slots:
	void handleIncomingUsers();
	void run();
	void setSpawn(DwellingType type, int clientID);
	void characterUnavail(CharacterTypes type, int clientID);
	void recordedTurn(RecordedTurn *turn, int clientID);
signals:
	void finished();
private:
	quint16 myPort;
	QTcpServer *incoming;
	std::vector<ClientCommThread *> *clientThreadList;
	bool selectedCharacters[MAXPLAYERS];
	bool receivedRecTurn[MAXPLAYERS];
    Game game;

	void moveCharacter(Character*, Clearing*);
	void writeMessageAllClients(string*);
	void writeMessageAllClients(QString*);
	void calculatePlayerTurnPhases();
};

#endif // SERVER_H