#ifndef CLIENTCOMMTHREAD_H
#define CLIENTCOMMTHREAD_H

#include <QtNetwork>
#include "shared.h"
#include "server.h"

class Server;

class ClientCommThread : public QObject{
	Q_OBJECT
public:
	ClientCommThread(QTcpSocket *socket, Server *parent, int id);
	~ClientCommThread();
	void writeMessage(QString *message);
	void writeMessage(string *message);

	CharacterType getMyCharacter();
	SearchType getSearchTypeResult();

public slots:
	void readIncomingData();
signals:
	void characterSelected(CharacterType type, int id);
	void spawnSelected(DwellingType type, int id);
	void incomingTurn(QString &turn, int id);
	void searchTypeReturned();
	void blockCheckReturn(bool answer, CharacterType responder);
	void monsterCombatReturned(int result, int monsterID, CharacterType responder);
	void subEncounterC(CharacterType, bool, int);
	void subMeleeC(CharacterType,int, CombatFightType, int, CombatMoveType, CombatShieldBlock);
	void playerWoundedC(CharacterType, vector<int>);
	void restResponseC(CharacterType, int);
private:
	QTcpSocket *clientConnection;
	Server *parent;
	quint16 blocksize;
	int clientID;
	CharacterType myCharacter;
	SearchType searchTypeRes;
};

#endif // CLIENTCOMMTHREAD_H