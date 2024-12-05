
#include "ConnectionManager.h"


#include "QtNetwork/qlocalserver.h"
#include "QtNetwork/qlocalsocket.h"

// OpenTwin Header
#include "OTCore/Logger.h"
#include "OTCommunication/ActionTypes.h"
#include "OTSystem/OperatingSystem.h"

std::string ConnectionManager::toString(RequestType _type) {
    
    switch (_type) {
    case ConnectionManager::ExecuteNetlist: return "ExecuteNetlist";
        
    default:
        OT_LOG_EAS("Unknown request type (" + std::to_string((int)_type) + ")");
        return "";
    }
}

ConnectionManager::ConnectionManager(QObject* parent) : QObject(parent) {
    m_server = new QLocalServer(this);
    m_socket = nullptr;
    
}

ConnectionManager::~ConnectionManager()
{
    delete m_server;
    m_server = nullptr;
}

void ConnectionManager::startListen(const std::string& _serverName) {
    
   
    connect(m_server, &QLocalServer::newConnection, this, &ConnectionManager::handleConnection);
    m_server->listen(QString::fromStdString(_serverName));
    OT_LOG_D("CircuitSimulatorService starting to listen: " + _serverName);
}

void ConnectionManager::queueRequest(RequestType _type, const std::string& _data) {
    QMetaObject::invokeMethod(this, "handleQueueRequest", Qt::QueuedConnection, Q_ARG(RequestType, _type), Q_ARG(std::string, _data));
}

void ConnectionManager::handleReadyRead() {
    QByteArray data = m_socket->readAll();
    OT_LOG_D("Received Data: " + data.toStdString());
}

void ConnectionManager::handleDisconnected() {

    OT_LOG_D("Client disconnected");
    delete m_socket;
    m_socket = nullptr;
}

void ConnectionManager::handleQueueRequest(RequestType _type, std::string _data) {

}



void ConnectionManager::handleConnection() {
    QLocalSocket* clientSocket = m_server->nextPendingConnection();
    
    // Check if connection already exists
    if (m_socket != nullptr) {
        OT_LOG_E("Connection already established");
        clientSocket->disconnect();
        delete clientSocket;
        return;
    }
    
    m_socket = clientSocket;
    OT_LOG_D("Subprocess connected");
    connect(m_socket, &QLocalSocket::readyRead, this, &ConnectionManager::handleReadyRead);
    connect(m_socket, &QLocalSocket::disconnected, this, &ConnectionManager::handleDisconnected);

    m_socket->write("Hello I am CircuitSimulatorService");
    m_socket->flush();

}













