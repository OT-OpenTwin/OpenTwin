#include "ConnectionManager.h"
#include "OTCommunication/ActionTypes.h"
#include "OTSystem/OperatingSystem.h"


ConnectionManager::ConnectionManager(QObject* parent) : QObject(parent) {
    
}

ConnectionManager::~ConnectionManager()
{
    if (m_socket)
    {
        m_socket->disconnectFromServer();
        m_socket->deleteLater();
    }
}

void ConnectionManager::startListen(std::string serverName) {
    m_server.listen(m_serverName.c_str());
}

QLocalSocket* ConnectionManager::getSocket()
{
    return m_socket;
}

QLocalServer& ConnectionManager::getServer()
{
    return m_server;
}

void ConnectionManager::connectWithSubprocess()
{
	OT_LOG_D("Waiting for connection servername: " + m_serverName);
	bool connected = m_server.waitForNewConnection(m_timeoutServerConnect);
	if (connected)
	{
		m_socket = m_server.nextPendingConnection();
		m_socket->waitForConnected(m_timeoutServerConnect);
		connected = m_socket->state() == QLocalSocket::ConnectedState;
		if (connected)
		{
			OT_LOG_D("Connection with subservice established");
		}
		else
		{
			std::string errorMessage;
			SocketErrorOccured(errorMessage);
			errorMessage = "Error occured while connecting with Python Subservice: " + errorMessage;
			throw std::exception(errorMessage.c_str());
		}
	}
	else
	{
		throw std::exception("Timout while waiting for subprocess to connect with server.");
	}
}

void ConnectionManager::setServerName(std::string serverName)
{
	this->m_serverName = serverName;
}




void ConnectionManager::SocketErrorOccured(std::string& message)
{
	message = m_socket->errorString().toStdString();
}





