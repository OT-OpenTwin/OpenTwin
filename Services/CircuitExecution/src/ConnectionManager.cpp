// Service Header
#include "ConnectionManager.h"
#include "NGSpice.h"
#include "SimulationResults.h"

// Qt Header
#include "QtCore/qjsondocument.h"
#include "QtCore/qjsonobject.h"
#include "QtCore/qjsonarray.h"
#include "QtCore/qtimer.h"

QString ConnectionManager::toString(RequestType _type) {

    switch (_type) {
    case ConnectionManager::SendResults: return "SendResults";
    case ConnectionManager::Message: return "Message";
    case ConnectionManager::Error: return "Error";

    default:
        OT_LOG_EAS("Unknown request type (" + std::to_string((int)_type) + ")");
        return "";
    }
}

ConnectionManager::ConnectionManager(QObject* parent) :QObject(parent) {

    m_socket = new QLocalSocket(this);
    healthCheckTimer = nullptr;
    SimulationResults* simulationResults = SimulationResults::getInstance();

    QObject::connect(m_socket, &QLocalSocket::connected, this, &ConnectionManager::handleConnected);
	QObject::connect(m_socket, &QLocalSocket::readyRead, this, &ConnectionManager::receiveResponse);
    QObject::connect(m_socket, &QLocalSocket::errorOccurred, this, &ConnectionManager::handleError);
    QObject::connect(m_socket, &QLocalSocket::disconnected, this, &ConnectionManager::handleDisconnected);
    QObject::connect(simulationResults, &SimulationResults::callback, this, &ConnectionManager::sendMessage);

    waitForHealthcheck = false;
#ifndef _DEBUG
    // Adding healthcheck
    healthCheckTimer = new QTimer(this);
    QObject::connect(healthCheckTimer, &QTimer::timeout, this, &ConnectionManager::sendHealthCheck);
    healthCheckTimer->setInterval(100);
    healthCheckTimer->start();
#endif // !_DEBUG


}

ConnectionManager::~ConnectionManager() {
    if (healthCheckTimer != nullptr) {
        healthCheckTimer->stop();
        delete healthCheckTimer;
        healthCheckTimer = nullptr;
    }
    

    delete m_socket;
    m_socket = nullptr;
}


void ConnectionManager::connectToCircuitSimulatorService(const QString& serverName) {

    /*OT_LOG_D("Connecting to Server:" + serverName.toStdString());*/
    m_socket->connectToServer(serverName);
    if (!m_socket->waitForConnected()) {
        OT_LOG_E("Failed to connect to server:" + m_socket->errorString().toStdString());
    }
    else {
        OT_LOG_D("Connected to Server");
    }

}

void ConnectionManager::sendMessage(std::string messageType, std::string message) {
    QJsonObject jsonObject;

    jsonObject["type"] = messageType.c_str();
    jsonObject["results"] = message.c_str();

    QJsonDocument jsonDoc(jsonObject);
    QByteArray data = jsonDoc.toJson(QJsonDocument::Compact);

    data.append('\n');
#ifndef _DEBUG
    OT_LOG(data.toStdString(), ot::OUTGOING_MESSAGE_LOG);
#endif // !_DEBUG
    sendJson(data);
}

void ConnectionManager::receiveResponse() {
    QByteArray jsonReceive = m_socket->readAll();
    QByteArray buffer = jsonReceive;

#ifndef _DEBUG
    OT_LOG(buffer.toStdString(), ot::INBOUND_MESSAGE_LOG);
#endif // !_DEBUG

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
        QJsonArray jsonArray = jsonObject["data"].toArray();

        handleActionType(typeString, jsonArray);
    }

    


 

}

void ConnectionManager::handleError(QLocalSocket::LocalSocketError error) {
    OT_LOG_E("Error in establishing connection to CircuitSimulatorService: " + m_socket->errorString().toStdString());
}

void ConnectionManager::handleDisconnected() {
    OT_LOG_D("Client disconnected from server");
    SimulationResults::getInstance()->getResultMap().clear();
    if (healthCheckTimer != nullptr) {
        healthCheckTimer->stop();
    }
    exit(0);
}

void ConnectionManager::sendHealthCheck() {
    if (m_socket->state() == QLocalSocket::ConnectedState) {
        if (waitForHealthcheck == false)             {
            waitForHealthcheck = true;
            sendMessage("Ping", "Healthcheck");
            OT_LOG_D("CircuitExecution Healthcheck");
        }
        else{
            handleDisconnected();
            OT_LOG_E("No connection established, healthcheck failed");
        }
    } 

    
    
}

void ConnectionManager::handleConnected() {
    OT_LOG_D("Hello CircuitSimulatorService!");
}

void ConnectionManager::handleActionType(QString _actionType, QJsonArray _data) {

 

    if (_actionType.toStdString() == "ExecuteNetlist") {
        std::list<std::string> netlist;

        for (const QJsonValue& value : _data) {
            
            QString item = value.toString();
            netlist.push_back(item.toStdString());  
        }
        handleRunSimulation(netlist);
    }
    else if (_actionType.toStdString() == "Disconnect") {
        handleDisconnected();
    }
    else if (_actionType.toStdString() == "Ping") {
        sendMessage("ResultPing", "Healthcheck");
    }
    else if (_actionType.toStdString() == "ResultPing") {
        waitForHealthcheck = false;
    }
    else {
        
        QJsonDocument doc(_data);
        QString jsonString = doc.toJson(QJsonDocument::Compact);
        OT_LOG_D("Normal Message:" + jsonString.toStdString());
    }
}

void ConnectionManager::handleRunSimulation(std::list<std::string> _netlist) {
    
    NGSpice::runNGSpice(_netlist);


}

void ConnectionManager::sendJson(QByteArray _data) {
    if (m_socket && m_socket->state() == QLocalSocket::ConnectedState) {
        qint64 bytesWritten = m_socket->write(_data);

        if (bytesWritten == -1) {
            OT_LOG_E("Error writing data");
            return;
        }

        // wait until all bytes are written
        if (!m_socket->waitForBytesWritten()) {
            OT_LOG_E("Error: waitForBytesWritten failed " + m_socket->error());

        }
        else {
            OT_LOG_D("Data successfully sent.");
        }

    }
    else {
        OT_LOG_E("Socket not connected!");
        return;
    }

}

void ConnectionManager::sendBackResults(std::map<std::string, std::vector<double>>& _results) {

    if (_results.empty()) {
        OT_LOG_E("No Results");
        handleDisconnected();
    }

    QJsonObject jsonObject;
    QJsonArray jsonArray;

    for (const auto& pair : _results) {
        QJsonObject entry;
        entry["key"] = QString::fromStdString(pair.first);
        QJsonArray valuesArray;
        for (double value : pair.second) {
            valuesArray.append(value);
        }
        entry["values"] = valuesArray;
        jsonArray.append(entry);
    }
    
    jsonObject["results"] = jsonArray;

    //Adding Action
    jsonObject["type"] = toString(ConnectionManager::SendResults);
    
    QJsonDocument jsonDoc(jsonObject);
    QByteArray data = jsonDoc.toJson(QJsonDocument::Compact);
    data.append('\n');
    
    OT_LOG_D("Sending Back Results");
    sendJson(data);
}
