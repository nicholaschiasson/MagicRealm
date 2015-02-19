#include "serverCommThread.h"

ServerCommThread::ServerCommThread(QObject *parent) : QObject(parent) 
{
	serverConnection = 0;
}

ServerCommThread::~ServerCommThread()
{
	threadDisconnect();
}

errno_t ServerCommThread::threadConnect(QString &hostIP, quint16 hostPort)
{
	errno_t err = 0;

	serverConnection = new QTcpSocket(this);
	serverConnection->connectToHost(hostIP, hostPort);
	connect(serverConnection, SIGNAL(readyRead()), this, SLOT(updateFromServer()));

	return err;
}

errno_t ServerCommThread::threadDisconnect()
{
	errno_t err = 0;

	if (serverConnection != 0)
	{
		serverConnection->close();
		delete serverConnection;
		serverConnection = 0;
	}

	return err;
}

bool ServerCommThread::isConnected()
{
	// TODO: FIX ME!!
	// How do we tell if a connection is made? This doesn't seem to work at all.
	return (serverConnection->state() == QAbstractSocket::ConnectedState || serverConnection->isValid());
}

void ServerCommThread::updateFromServer()
{

}