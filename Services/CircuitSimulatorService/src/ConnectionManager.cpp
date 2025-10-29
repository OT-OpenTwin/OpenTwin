// @otlicense

// Service Header
#include "ConnectionManager.h"
#include "SimulationResults.h"

// Qt Header
#include "QtNetwork/qlocalserver.h"
#include "QtNetwork/qlocalsocket.h"
#include "QtCore/qjsonarray.h"
#include "QtCore/qjsondocument.h"
#include "QtCore/qjsonobject.h"
#include "QtCore/qcoreapplication.h"
#include <QtCore/qelapsedtimer>
// OpenTwin Header
#include "OTCore/LogDispatcher.h"
#include "OTCommunication/ActionTypes.h"
#include "OTSystem/OperatingSystem.h"
#include "OTGui/StyledTextBuilder.h"
#include "Application.h"

QString ConnectionManager::toString(RequestType _type) {
    
    switch (_type) {
    case ConnectionManager::ExecuteNetlist: return "ExecuteNetlist";
    case ConnectionManager::Message: return "Message";
    case ConnectionManager::Error: return "Error";
    case ConnectionManager::Disconnect: return "Disconnect";
    default:
        OT_LOG_EAS("Unknown request type (" + std::to_string((int)_type) + ")");
        return "";
    }
}



ConnectionManager::ConnectionManager(QObject* parent) : QObject(parent) {
    m_server = new QLocalServer(this);
    m_socket = nullptr;
    waitForHealthcheck = false;
    healthCheckTimer = nullptr;
    m_netlist = nullptr;
}

ConnectionManager::~ConnectionManager()
{
    delete m_server;
    m_server = nullptr;

    delete healthCheckTimer;
    healthCheckTimer = nullptr;


}

void ConnectionManager::startListen(const std::string& _serverName) {
    
   
    connect(m_server, &QLocalServer::newConnection, this, &ConnectionManager::handleConnection);
    m_server->listen(QString::fromStdString(_serverName));

    OT_LOG_D("CircuitSimulatorService starting to listen: " + _serverName);
}

bool ConnectionManager::sendNetlist() {
    if (!this->waitForClient()) {
        return false;
    }

    m_socket->write(m_netlist + "\n");
    m_socket->flush();
    return true;
}

bool ConnectionManager::waitForClient(void) {

    const int tickTime = 10;
    int timeout = 30000 / tickTime;
    while ( m_socket == nullptr) {
        if (timeout--) {
            std::this_thread::sleep_for(std::chrono::milliseconds(tickTime));
            QCoreApplication::processEvents();
        }
        else {
            OT_LOG_E("Client connection timeout");
            return false;
        }
    }

    return true;
}

void ConnectionManager::send(std::string messageType, std::string message) {
    QJsonObject jsonObject;

    jsonObject["type"] = messageType.c_str();
    jsonObject["results"] = message.c_str();

    QJsonDocument jsonDoc(jsonObject);
    QByteArray data = jsonDoc.toJson(QJsonDocument::Compact);
    data.append('\n');

    OT_LOG(data.toStdString(), ot::OUTGOING_MESSAGE_LOG);
    if (!m_socket)         {
        OT_LOG_E("CircuitExecution died before");
        return;
    }
    m_socket->write(data + "\n");
    m_socket->flush();
}

void ConnectionManager::setNetlist(RequestType _type,std::list<std::string>& _netlist) {
    //In this function I set the netlist which I got from Application
    QJsonObject jsonObject;

    jsonObject["type"] = toString(_type);

    QJsonArray jsonArray;

    for (const auto& str : _netlist) {
        jsonArray.append(QString::fromStdString(str));
    }
    jsonObject["data"] = jsonArray;

    QJsonDocument jsonDoc(jsonObject);
    QByteArray jsonData = jsonDoc.toJson(QJsonDocument::Compact);
    jsonData.append('\n');

    m_netlist = jsonData;
}


void ConnectionManager::handleReadyRead() {
    QByteArray rawData = m_socket->readAll();
    OT_LOG(rawData.toStdString(), ot::INBOUND_MESSAGE_LOG);
    
    QByteArray buffer = rawData;

    while (true) {
        int newlineIndex = buffer.indexOf('\n');
        if (newlineIndex == -1 || newlineIndex == 0) {
            break;
        }

        QByteArray line = buffer.left(newlineIndex);
        buffer.remove(0, newlineIndex + 1);

        QJsonParseError parseError;
        QJsonDocument doc = QJsonDocument::fromJson(line, &parseError);
        if (parseError.error != QJsonParseError::NoError) {
            qWarning() << "JSON parse error:" << parseError.errorString();
            continue;
        }

        QJsonObject jsonObject = doc.object();
        QString typeString = jsonObject["type"].toString();
        QJsonValue dataValue = jsonObject["results"];

        handleMessageType(typeString, dataValue);
    }
    
    
   
}


void ConnectionManager::handleDisconnected() {


    QDateTime endTime = QDateTime::currentDateTime();
    SimulationResults::getInstance()->handleCircuitExecutionTiming(endTime, "finishedCircuitExecutionTime");
   
    OT_LOG_D("Client disconnected");

    if (healthCheckTimer != nullptr) {
        healthCheckTimer->stop();
        delete healthCheckTimer;
        healthCheckTimer = nullptr;
    }

    waitForHealthcheck = false;
    delete m_socket;
    m_socket = nullptr;

    Application::instance()->finishSimulation();
}


void ConnectionManager::sendHealthcheck() {
    if (m_socket == nullptr)         {
        return;
    }
    if (m_socket->state() == QLocalSocket::ConnectedState) {
        if (waitForHealthcheck == false) {
            waitForHealthcheck = true;
            OT_LOG_D("Sending Ping. waitForHealthcheck: " + std::to_string(waitForHealthcheck));
            send("Ping", "Healthcheck");
            return;
        }
        else {
            OT_LOG_W("Ping timeout. waitForHealthcheck: " + std::to_string(waitForHealthcheck));
            Application::instance()->getSubProcessHandler()->stopSubprocess();
            handleDisconnected();
            return;
        }
    }
    else {
        OT_LOG_E("No connection established, healthcheck failed");
        return;
    }
    
}

void ConnectionManager::handleMessageType(QString& _actionType, const QJsonValue& data) {

    
            
        if (_actionType.toStdString() == "Error") {
            // ot::StyledTextBuilder textBuilder;

            if (data.isString()){
                std::string temp = data.toString().toStdString();
                if (temp == " Note: can't find the initialization file spinit.") {

                    //Here we supress the missing initialization file message
                    return;
                }
                SimulationResults::getInstance()->displayError(data.toString().toStdString());
            }
            else {
                OT_LOG_E("JSON array entry is not a string");
                return;
            }  
        }
        else if (_actionType.toStdString() == "UnrecoverableError"){
            if (data.isString()) {

                SimulationResults::getInstance()->displayError(data.toString().toStdString());

                // If we get an error we need to shut down the subservice
                send(toString(ConnectionManager::RequestType::Disconnect).toStdString(), "Disconnect");

            }
            else {
                OT_LOG_E("JSON array entry is not a string");
                return;
            }

        }
        else if (_actionType.toStdString() == "Message") {
            if (data.isString()) {
                SimulationResults::getInstance()->displayMessage(data.toString().toStdString());
               
            }
            else {
                OT_LOG_E("JSON array entry is not a string");
                return;
            }
        }
        else if (_actionType.toStdString() == "SendResults") {
            SimulationResults::getInstance()->handleResults(data);
            OT_LOG_D("Got Results");
            send(toString(ConnectionManager::RequestType::Disconnect).toStdString(), "Disconnect");
            
               
        }
        else if (_actionType.toStdString() == "Ping") {
                send("ResultPing", "Healthcheck");
                OT_LOG_D("Sending Ping. waitForHealthcheck: " + std::to_string(waitForHealthcheck));
        }
        else if (_actionType.toStdString() == "ResultPing") {
            waitForHealthcheck = false;
            OT_LOG_D("Received ResultPing. waitForHealthcheck before reset: " + std::to_string(waitForHealthcheck));
        }
        else {
            if (data.isString()) {
                SimulationResults::getInstance()->handleUnknownMessageType(data.toString().toStdString());

            }
            else {
                OT_LOG_E("JSON array entry is not a string");
                return;
            }
            
            
        }
    
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
#ifndef _DEBUG
    healthCheckTimer = new QTimer(this);
    connect(healthCheckTimer, &QTimer::timeout, this, &ConnectionManager::sendHealthcheck);

    healthCheckTimer->setInterval(5000);
    healthCheckTimer->start();
    waitForHealthcheck = false;
#endif // !_DEBUG

    OT_LOG_D("Hello CircuitExecution!");

}













