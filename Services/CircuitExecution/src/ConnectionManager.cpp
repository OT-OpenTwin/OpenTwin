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
    m_ngSpice = new NGSpice();
    SimulationResults* simulationResults = SimulationResults::getInstance();

    QObject::connect(m_socket, &QLocalSocket::connected, this, &ConnectionManager::sendHealthCheck);
	QObject::connect(m_socket, &QLocalSocket::readyRead, this, &ConnectionManager::receiveResponse);
    QObject::connect(m_socket, &QLocalSocket::errorOccurred, this, &ConnectionManager::handleError);
    QObject::connect(m_socket, &QLocalSocket::disconnected, this, &ConnectionManager::handleDisconnected);
    QObject::connect(simulationResults, &SimulationResults::callback, this, &ConnectionManager::send);

    waitForHealthcheck = false;

    // Adding healthcheck
    healthCheckTimer = new QTimer(this);
    QObject::connect(healthCheckTimer, &QTimer::timeout, this, &ConnectionManager::sendHealthCheck);
    healthCheckTimer->setInterval(5000);
    healthCheckTimer->start();
}

ConnectionManager::~ConnectionManager() {
    healthCheckTimer->stop();
    delete healthCheckTimer;
    healthCheckTimer = nullptr;

    delete m_ngSpice;
    m_ngSpice = nullptr;

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

void ConnectionManager::send(std::string messageType, std::string message) {
    QJsonObject jsonObject;

    jsonObject["type"] = messageType.c_str();
    jsonObject["results"] = message.c_str();

    QJsonDocument jsonDoc(jsonObject);
    QByteArray data = jsonDoc.toJson();

    OT_LOG(data.toStdString(),ot::OUTGOING_MESSAGE_LOG);
    m_socket->write(data);
    m_socket->flush();
}

void ConnectionManager::receiveResponse() {
    QByteArray jsonData = m_socket->readAll();
    OT_LOG(jsonData.toStdString(), ot::INBOUND_MESSAGE_LOG);

  
    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData);

    if (!jsonDoc.isObject()) {
        OT_LOG_E("No correct Json Format");
        return;
    }

    QJsonObject jsonObject = jsonDoc.object();

    //Getting message type
    QString typeString = jsonObject["type"].toString();
    
    //Getting data
    QJsonArray jsonArray = jsonObject["data"].toArray();
    std::list<std::string> data;
    for (const QJsonValue& value : jsonArray) {
        data.push_back(value.toString().toStdString());
    }

    handleActionType(typeString, jsonArray);

    //After Simulating I send back the results
    //sendBackResults(SimulationResults::getInstance()->getResultMap());



}



void ConnectionManager::handleError(QLocalSocket::LocalSocketError error) {
    OT_LOG_E("Error in establishing connection to CircuitSimulatorService: " + m_socket->errorString().toStdString());
}

void ConnectionManager::handleDisconnected() {
    OT_LOG_D("Client disconnected from server");
    SimulationResults::getInstance()->getResultMap().clear();
    healthCheckTimer->stop();
    exit(0);
}

void ConnectionManager::sendHealthCheck() {
    if (m_socket->state() == QLocalSocket::ConnectedState) {
        if (waitForHealthcheck == false)             {
            waitForHealthcheck = true;
            send("Ping", "Healthcheck");
        }
        else{
            handleDisconnected();
            OT_LOG_E("No connection established, healthcheck failed");
        }
    } 

    
    
}

void ConnectionManager::handleActionType(QString _actionType, QJsonArray _data) {

 

    if (_actionType.toStdString() == "ExecuteNetlist") {
        std::list<std::string> netlist;

        for (const QJsonValue& value : _data) {
            // Umwandlung von QJsonValue (String) zu std::string
            QString item = value.toString();
            netlist.push_back(item.toStdString());  // QString zu std::string konvertieren
        }
        handleRunSimulation(netlist);
    }
    else if (_actionType.toStdString() == "Disconnect") {
        handleDisconnected();
    }
    else if (_actionType.toStdString() == "Ping") {
        send("ResultPing", "Healthcheck");
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
  
    //Initialize Callbacks of NGSpice
    m_ngSpice->initializeCallbacks();

    //Intialize NGSpice
    m_ngSpice->intializeNGSpice();

    //Executing run from NGSpice
    m_ngSpice->runSimulation(_netlist);

    //The results are being pushed in the callbacks to SimulationResults map
    sendBackResults(SimulationResults::getInstance()->getResultMap());

}

void ConnectionManager::sendBackResults(std::map<std::string, std::vector<double>> _results) {

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
    QByteArray data = jsonDoc.toJson();
    
    
    OT_LOG_D("Sending Back Results");
    m_socket->write(data);
    m_socket->flush();

    //handleDisconnected();
}
