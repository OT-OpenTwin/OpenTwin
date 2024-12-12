// Service Header
#include "ConnectionManager.h"
#include "NGSpice.h"
#include "SimulationResults.h"

// Qt Header
#include "QtCore/qjsondocument.h"
#include "QtCore/qjsonobject.h"
#include "QtCore/qjsonarray.h"

ConnectionManager::ConnectionManager(QObject* parent) :QObject(parent) {

    m_socket = new QLocalSocket(this);
    m_ngSpice = new NGSpice();

    QObject::connect(m_socket, &QLocalSocket::connected, this, &ConnectionManager::sendHello);
	QObject::connect(m_socket, &QLocalSocket::readyRead, this, &ConnectionManager::receiveResponse);
    QObject::connect(m_socket, &QLocalSocket::errorOccurred, this, &ConnectionManager::handleError);
    QObject::connect(m_socket, &QLocalSocket::disconnected, this, &ConnectionManager::handleDisconnected);
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

void ConnectionManager::receiveResponse() {
    QByteArray jsonData = m_socket->readAll();
    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData);

    if (!jsonDoc.isObject()) {
        OT_LOG_E("No correct Json Format");
        return;
    }

    QJsonObject jsonObject = jsonDoc.object();

    //Getting action type
    QString typeString = jsonObject["type"].toString();
    
    //Getting data
    QJsonArray jsonArray = jsonObject["data"].toArray();
    std::list<std::string> data;
    for (const QJsonValue& value : jsonArray) {
        data.push_back(value.toString().toStdString());
    }

    handleActionType(typeString, jsonArray);

    //After Simulating I send back the results
    sendBackResults(SimulationResults::getInstance()->getResultMap());



}

void ConnectionManager::sendHello() {
    OT_LOG_D("Connected");
    QByteArray message = "Connected";
    m_socket->write(message);
    m_socket->flush();
}

void ConnectionManager::handleError(QLocalSocket::LocalSocketError error) {
    OT_LOG_E("Error in establishing connection to CircuitSimulatorService: " + m_socket->errorString().toStdString());
}

void ConnectionManager::handleDisconnected() {
    OT_LOG_D("Client disconnected from server");
    exit(0);
}

void ConnectionManager::handleActionType(QString _actionType, QJsonArray _data) {

    QJsonDocument doc(_data);
    QString jsonString = doc.toJson(QJsonDocument::Compact);

    if (_actionType.toStdString() == "ExecuteNetlist") {
        handleRunSimulation(jsonString);
    }
    else {
        OT_LOG_D("Normal Message:" + jsonString.toStdString());
    }
}

void ConnectionManager::handleRunSimulation(QString _netlistString) {
    
    //First getting the std::list
    std::list<std::string> _netlist;

    //Here i erase the []
    std::string temp = _netlistString.toStdString();
    temp.erase(std::remove(temp.begin(), temp.end(), '['), temp.end());
    temp.erase(std::remove(temp.begin(), temp.end(), ']'), temp.end());
    
    //Now i put the strings into the netlist and delete the ""
    std::stringstream ss(temp);
    std::string item;
    while (std::getline(ss, item, ',')) {
        // Schritt 3: Entferne zusätzliche Anführungszeichen
        item.erase(std::remove(item.begin(), item.end(), '\"'), item.end());
        _netlist.push_back(item);
    }

   
    //Executing run from NGSpice
    m_ngSpice->runSimulation(_netlist);

    //The results are being pushed in the callbacks to SimulationResults map

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

    
    QJsonDocument jsonDoc(jsonObject);
    QByteArray data = jsonDoc.toJson();
    
    OT_LOG_D("Sendling back results: " + data.toStdString());

    m_socket->write(data);
    m_socket->flush();
}
