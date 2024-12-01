#include "ConnectionManager.h"
#include "OTCommunication/ActionTypes.h"
#include "OTSystem/OperatingSystem.h"

ConnectionManager::ConnectionManager()
{

}

ConnectionManager::~ConnectionManager()
{

}

//bool ConnectionManager::connectToSubprocess() {
//	OT_LOG_D("Waiting for connection servername: " + m_serverName);
//	bool connected = m_server.waitForNewConnection(_timeoutServerConnect);
//	if (connected)
//	{
//		m_socket = m_server.nextPendingConnection();
//		m_socket->waitForConnected(_timeoutServerConnect);
//		connected = _socket->state() == QLocalSocket::ConnectedState;
//		if (connected)
//		{
//			OT_LOG_D("Connection with subservice established");
//		}
//		else
//		{
//			std::string errorMessage;
//			SocketErrorOccured(errorMessage);
//			errorMessage = "Error occured while connecting with Python Subservice: " + errorMessage;
//			throw std::exception(errorMessage.c_str());
//		}
//	}
//	else
//	{
//		throw std::exception("Timout while waiting for subprocess to connect with server.");
//	}
//	return true;
//}

std::string ConnectionManager::sendRequest(std::string& message)
{
	return std::string();
}

std::string ConnectionManager::receiveResponse()
{
	return std::string();
}
