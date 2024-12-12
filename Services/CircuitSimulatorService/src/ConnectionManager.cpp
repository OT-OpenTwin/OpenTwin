// Service Header
#include "ConnectionManager.h"
#include "SimulationResults.h"

// Qt Header
#include "QtNetwork/qlocalserver.h"
#include "QtNetwork/qlocalsocket.h"
#include "QtCore/qjsonarray.h"
#include "QtCore/qjsondocument.h"
#include "QtCore/qjsonobject.h"

// OpenTwin Header
#include "OTCore/Logger.h"
#include "OTCommunication/ActionTypes.h"
#include "OTSystem/OperatingSystem.h"

QString ConnectionManager::toString(RequestType _type) {
    
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

void ConnectionManager::queueRequest(RequestType _type, const std::list<std::string>& _data) {
    
    QMetaObject::invokeMethod(this, "handleQueueRequest", Qt::QueuedConnection, Q_ARG(RequestType, _type), Q_ARG(std::list<std::string>, _data));
   
}

void ConnectionManager::handleReadyRead() {
    QByteArray data = m_socket->readAll();
    QString result = QString::fromUtf8(data);

    OT_LOG_D("Got the Results: " + result.toStdString());
    SimulationResults::getInstance()->addResults(result);   
}

void ConnectionManager::handleDisconnected() {

    OT_LOG_D("Client disconnected");
    delete m_socket;
    m_socket = nullptr;
}

void ConnectionManager::handleQueueRequest(RequestType _type, std::list<std::string> _data) {

    //In this function I set the netlist which I got from Application
    QJsonObject jsonObject;

    jsonObject["type"] = toString(_type);

    QJsonArray jsonArray;

    for (const auto& str : _data) {
        jsonArray.append(QString::fromStdString(str));
    }
    jsonObject["data"] = jsonArray;

    QJsonDocument jsonDoc(jsonObject);
    QByteArray jsonData = jsonDoc.toJson();

    m_netlist = jsonData;
    
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

    //I only send the netlist when the connection is established to prevent sending before connected
    OT_LOG_D("Send netlist");
    m_socket->write(m_netlist);
    m_socket->flush();

   

}













