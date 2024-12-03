#include "ConnectionManager.h"
#include "OTCommunication/ActionTypes.h"
#include "OTSystem/OperatingSystem.h"

ConnectionManager::ConnectionManager()
{

}

ConnectionManager::~ConnectionManager()
{

}

void ConnectionManager::startListen(std::string serverName)
{
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


