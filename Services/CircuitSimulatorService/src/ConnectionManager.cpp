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
#include "OTGui/StyledTextBuilder.h"

QString ConnectionManager::toString(RequestType _type) {
    
    switch (_type) {
    case ConnectionManager::ExecuteNetlist: return "ExecuteNetlist";
    case ConnectionManager::Message: return "Message";
    case ConnectionManager::Error: return "Error";

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

bool ConnectionManager::isSingleJsonObject(const QByteArray& data) {
    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(data, &parseError);

    return parseError.error == QJsonParseError::NoError && doc.isObject();
}

bool ConnectionManager::isJsonArray(const QByteArray& data) {
    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(data, &parseError);

    return parseError.error == QJsonParseError::NoError && doc.isArray();
}

bool ConnectionManager::isMixed(const QByteArray& data) {
    return !isSingleJsonObject(data) && !isJsonArray(data);
}

int ConnectionManager::findEndOfJsonObject(const QByteArray& data) {

    int openBraces = 0;
    bool insideString = false;

    for (int i = 0; i < data.size(); ++i) {
        char c = data[i];

        if (c == '"' && (i == 0 || data[i - 1] != '\\')) {
            insideString = !insideString; 
        }

        if (!insideString) {
            if (c == '{') {
                openBraces++;
            }
            else if (c == '}') {
                openBraces--;
                if (openBraces == 0) {
                    return i; 
                }
            }
        }
    }

    return -1;
}

QList<QJsonObject> ConnectionManager::handleMultipleJsonObjects(const QByteArray& jsonStream) {
    QList<QJsonObject> parsedObjects;
    int startIndex = 0;
    int braceCount = 0;
    int objectStart = -1;

    for (int i = 0; i < jsonStream.size(); ++i) {
        char currentChar = jsonStream[i];

        
        if (currentChar == '{') {
            if (braceCount == 0) {
                objectStart = i; 
            }
            braceCount++;
        }
        
        else if (currentChar == '}') {
            braceCount--;
           
            if (braceCount == 0 && objectStart != -1) {
              
                QByteArray singleObject = jsonStream.mid(objectStart, i - objectStart + 1);

              
                QJsonParseError parseError;
                QJsonDocument jsonDoc = QJsonDocument::fromJson(singleObject, &parseError);

                if (parseError.error == QJsonParseError::NoError && jsonDoc.isObject()) {
                    parsedObjects.append(jsonDoc.object());
                }
                else {
                    OT_LOG_E("Error by parsing of JSON_Objects");
                }

                
                objectStart = -1;
            }
        }
    }

    return parsedObjects;
}


void ConnectionManager::handleReadyRead() {
    QByteArray rawData = m_socket->readAll();

    if (isMixed(rawData))         {
        QList<QJsonObject> jsonObjects = handleMultipleJsonObjects(rawData);
        for (const QJsonObject& object : jsonObjects) {
            handleWithJson(object);
        }
    }
    else if (isSingleJsonObject(rawData)) {

        QJsonDocument jsonDoc = QJsonDocument::fromJson(rawData);

        if (!jsonDoc.isObject() && !jsonDoc.isArray()) {
            OT_LOG_E("Incorrect json format");
            return;
        }
        QJsonObject jsonObject = jsonDoc.object();

        handleWithJson(jsonObject);
    }
    else if(isJsonArray(rawData)) {
        QJsonDocument jsonDoc = QJsonDocument::fromJson(rawData);
        QJsonArray jsonArray = jsonDoc.array();

        for (const QJsonValue& value : jsonArray) {
            if (value.isObject())                 {
                QJsonObject obj = value.toObject();
                handleWithJson(obj);
            }
            else {
                OT_LOG_E("Incorrect json format");
            }
        }
    }
    
   
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

void ConnectionManager::handleWithJson(const QJsonObject& jsonObject) {
    // Getting message Type
    if (!jsonObject.contains("type")) {
        OT_LOG_E("Missing json object member \"type\"");
        return;
    }
    if (!jsonObject["type"].isString()) {
        OT_LOG_E("JSON object member \"type\" is not a string");
        return;
    }

    QString typeString = jsonObject["type"].toString();

    // Getting message data
    if (!jsonObject.contains("results")) {
        OT_LOG_E("Missing json object member \"results\"");
        return;
    }
    if (!jsonObject["results"].isString() && !jsonObject["results"].isArray()) {
        OT_LOG_E("JSON object member \"results\" is not a string and not an array");
        return;
    }

    std::list<std::string> data;
    const QJsonValue resultsValue = jsonObject["results"];


    handleMessageType(typeString, resultsValue);
}

void ConnectionManager::handleMessageType(QString& _actionType, const QJsonValue& data) {

    
            
        if (_actionType.toStdString() == "Error") {
            // ot::StyledTextBuilder textBuilder;

            if (data.isString()){
                SimulationResults::getInstance()->displayError(data.toString().toStdString());
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

    //I only send the netlist when the connection is established to prevent sending before connected
    OT_LOG_D("Send netlist");
    m_socket->write(m_netlist);
    m_socket->flush(); 
}













