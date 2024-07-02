

#include "SocketServer.h"

// OpenTwin header
#include "OTCore/JSON.h"						// rapidjson wrapper
#include "OTCore/Logger.h"						// Logger
#include "OTCommunication/ActionTypes.h"		// action member and types definition
#include "OTCommunication/Msg.h"				// message sending

#include <QtWebSockets/qwebsocketserver.h>
#include <QtWebSockets/qwebsocket.h>
#include <QEventLoop>

#include <iostream>

// Curl
#include "curl/curl.h"

extern std::string globalServiceIP;

// SSL
#include <QtCore/QDebug>
#include <QtCore/QFile>
#include <QtNetwork/QSslCertificate>
#include <QtNetwork/QSslKey>
#include <QProcessEnvironment>




SocketServer::SocketServer(std::string socketIP, unsigned int socketPort) :
	QObject(nullptr),
	m_pWebSocketServer(new QWebSocketServer(QStringLiteral("SSL Websocket Server"), QWebSocketServer::SecureMode, this)),
	responseReceived(false)
{
	QHostAddress serverAddress;
	serverAddress.setAddress(socketIP.c_str());

	// TLS/SSL Config
	QSslConfiguration sslConfiguration;

	QString server_cert_file_path = QProcessEnvironment::systemEnvironment().value("OPEN_TWIN_SERVER_CERT", "");
	QString server_key_file_path = QProcessEnvironment::systemEnvironment().value("OPEN_TWIN_SERVER_CERT_KEY", "");

	QFile certFile(server_cert_file_path);
	QFile keyFile(server_key_file_path);
	certFile.open(QIODevice::ReadOnly);
	keyFile.open(QIODevice::ReadOnly);

	QSslCertificate certificate(&certFile, QSsl::Pem);
	QSslKey sslKey(&keyFile, QSsl::Rsa, QSsl::Pem);
	certFile.close();
	keyFile.close();
	sslConfiguration.setPeerVerifyMode(QSslSocket::VerifyNone);
	sslConfiguration.setLocalCertificate(certificate);
	sslConfiguration.setPrivateKey(sslKey);
	m_pWebSocketServer->setSslConfiguration(sslConfiguration);
	
	// Initialize the performance counters
	m_systemLoad.initialize();

	if (m_pWebSocketServer->listen(serverAddress, socketPort))
	{
		OT_LOG_I("Websocket server listening on: " + socketIP + ":" + std::to_string(socketPort));
        connect(m_pWebSocketServer, &QWebSocketServer::newConnection, this, &SocketServer::onNewConnection);
        connect(m_pWebSocketServer, &QWebSocketServer::closed, this, &SocketServer::closed);
		connect(m_pWebSocketServer, &QWebSocketServer::sslErrors,
			this, &SocketServer::onSslErrors);
    }
}

void SocketServer::onSslErrors(const QList<QSslError> & _errors)
{
	// TODO: HANDLE SSL ERRORS?
	for (auto e : _errors) {
		OT_LOG_E("SSL error occurred: " + e.errorString().toStdString());
	}
}

SocketServer::~SocketServer()
{
    m_pWebSocketServer->close();
    qDeleteAll(m_clients.begin(), m_clients.end());
}

void SocketServer::onNewConnection()
{
	OT_LOG_I("New client connected on websocket");

    QWebSocket *pSocket = m_pWebSocketServer->nextPendingConnection();

    connect(pSocket, &QWebSocket::textMessageReceived, this, &SocketServer::processMessage);
    connect(pSocket, &QWebSocket::disconnected, this, &SocketServer::socketDisconnected);

    m_clients << pSocket;
}

void SocketServer::processMessage(QString message)
{
	try {
		std::string stdMessage = message.toStdString();

		//std::cout << "Processing message: " << message.toStdString() << std::endl;

		int index1 = stdMessage.find('\n');
		int index2 = stdMessage.find('\n', index1 + 1);

		std::string operation = stdMessage.substr(0, index1);

		if (operation == "response")
		{
			OT_LOG_I("Response to previous message received on websocket");

			responseText = stdMessage.substr(index1 + 1);
			responseReceived = true;
		}
		else if (operation == "execute" || operation == "queue")
		{
			OT_LOG_I("New execute or queue message received on websocket");

			std::string url = stdMessage.substr(index1 + 1, index2 - index1 - 1);
			std::string jsonData = stdMessage.substr(index2 + 1);

			std::string response;

			OT_LOG_I("Relaying received message to HTTP");
			sendHttpRequest(operation, url, jsonData, response);

			response = "response\n" + url + "\n" + response;

			OT_LOG_I("Sending reponse of HTTP message through websocket");

			for (auto pClient : m_clients)
			{
				pClient->sendTextMessage(response.c_str());
				pClient->flush();

			}
		}
		else
		{
			throw std::exception("Unknown operation received on processMessage");
		}
	}
	catch (const std::exception & e) {
		OT_LOG_E(std::string(e.what()));
	}
	catch (...) {
		OT_LOG_E("Unknown error");
	}
}

std::string SocketServer::sendProcessWSMessage(const std::string operation, const std::string senderIP, const std::string jsonData)
{
	responseReceived = false;

	// Build and send the message through the websocket connection

	std::string message = operation + "\n" + senderIP + "\n" + jsonData;

	OT_LOG_I("Relaying received message to websocket");

	for (auto pClient : m_clients)
	{
		pClient->sendTextMessage(message.c_str());
		pClient->flush();
	}
	
	// Now wait for the response
	// NOTE: This function is problematic, since it will also process further queued actions. A message filter needs to be 
	// set such that this type of messages is not processed here.
	while (!responseReceived)
	{
		processMessages();
	}

	return responseText;
}

void SocketServer::sendQueueWSMessage(const std::string operation, const std::string senderIP, const std::string jsonData)
{
	// Build and send the message through the websocket connection

	std::string message = operation + "\n" + senderIP + "\n" + jsonData;

	OT_LOG_I("Relaying received message to websocket");

	for (auto pClient : m_clients)
	{
		pClient->sendTextMessage(message.c_str());
		pClient->flush();
	}
}

void SocketServer::processMessages(void)
{
	QEventLoop eventLoop;
	// We have to process all messages to make sure we are connected
	eventLoop.processEvents(QEventLoop::ExcludeUserInputEvents | QEventLoop::WaitForMoreEvents);
}

void SocketServer::socketDisconnected()
{
	OT_LOG_I("Client disconnected on websocket");

    QWebSocket *pClient = qobject_cast<QWebSocket *>(sender());

    if (pClient) {
        m_clients.removeAll(pClient);
        pClient->deleteLater();
    }

	if (m_clients.empty())
	{
		OT_LOG_I("No more clients, shutting down relay service");

		exit(0);
	}
}

size_t writeFunction(void *ptr, size_t size, size_t nmemb, std::string* data) {
	try {
		data->append((char*)ptr, size * nmemb);
		return size * nmemb;
	}
	catch (...) {
		assert(0); // Error handling
	}
	return 0;
}

bool SocketServer::sendHttpRequest(const std::string &operation, const std::string &url, const std::string &jsonData, std::string &response)
{
	try {

		OT_LOG_I("Sending http request (" + operation + ") to " + url + ": " + jsonData);

		bool success = false;

		if (operation == "execute")
		{
			success = ot::msg::send(globalServiceIP, url, ot::EXECUTE, jsonData, response);
		}
		else if (operation == "queue")
		{
			success = ot::msg::send(globalServiceIP, url, ot::QUEUE, jsonData, response);
		}
		else
		{
			assert(0);
		}

		return success;
	}
	catch (...) {
		assert(0); // Error handling
		return false;
	}

	return false;
}

QString SocketServer::performAction(const char *json, const char *senderIP)
{
	try {
		ot::JsonDocument doc;
		doc.fromJson(json);

		std::string action = ot::json::getString(doc, "action");

		if (action == OT_ACTION_CMD_GetSystemInformation)
		{
			return getSystemInformation().c_str();
		}

		OT_LOG_E("Received HTTP execute message (not yet suported by relay service): " + action);

		OT_LOG("Received HTTP execute message: " + action, ot::INBOUND_MESSAGE_LOG);

		if (action == OT_ACTION_CMD_ShutdownRequestedByService) {
			shutdown();
		}

		std::string response = sendProcessWSMessage("execute", senderIP, json);

		QString retVal = response.c_str();

		OT_LOG_I("Returning received websocket answer to HTTP sender");

		return retVal;
	}
	catch (const std::exception & e) {
		OT_LOG_E(std::string(e.what()));
		return "";
	}
	catch (...) {
		OT_LOG_E("Unknown error");
		return "";
	}
}

void SocketServer::queueAction(const char *json, const char *senderIP)
{
	try {
		ot::JsonDocument doc;
		doc.fromJson(json);

		std::string action = ot::json::getString(doc, "action");

		OT_LOG_I("Received HTTP queue message: " + action);

		if (action == OT_ACTION_CMD_ShutdownRequestedByService) {
			shutdown();
		}
		
		sendQueueWSMessage("queue", senderIP, json);

		delete[] json;
		json = nullptr;

		delete[] senderIP;
		senderIP = nullptr;
	}
	catch (const std::exception & e) {
		OT_LOG_E(std::string(e.what()));
	}
	catch (...) {
		OT_LOG_E("Unknown error");
	}
}

void SocketServer::shutdown() 
{
	// Exit the application
	exit(0);
}

void SocketServer::deallocateData(const char *data)
{
	if (data != nullptr)
	{
		delete[] data;
	}
	data = nullptr;
}

std::string SocketServer::getSystemInformation()
{
	double globalCpuLoad = 0, globalMemoryLoad = 0, processCpuLoad = 0, processMemoryLoad = 0;

	m_systemLoad.getGlobalCPUAndMemoryLoad(globalCpuLoad, globalMemoryLoad);
	m_systemLoad.getCurrentProcessCPUAndMemoryLoad(processCpuLoad, processMemoryLoad);

	ot::JsonDocument reply;
	reply.AddMember(OT_ACTION_PARAM_GLOBAL_CPU_LOAD, globalCpuLoad, reply.GetAllocator());
	reply.AddMember(OT_ACTION_PARAM_GLOBAL_MEMORY_LOAD, globalMemoryLoad, reply.GetAllocator());
	reply.AddMember(OT_ACTION_PARAM_PROCESS_CPU_LOAD, processCpuLoad, reply.GetAllocator());
	reply.AddMember(OT_ACTION_PARAM_PROCESS_MEMORY_LOAD, processMemoryLoad, reply.GetAllocator());

	reply.AddMember(OT_ACTION_PARAM_SERVICE_TYPE, ot::JsonString("RelayService", reply.GetAllocator()), reply.GetAllocator());
	//reply.AddMember(OT_ACTION_PARAM_SESSION_ID, ot::JsonString(m_application->sessionID(), reply.GetAllocator()), reply.GetAllocator());
	//reply.AddMember(OT_ACTION_PARAM_SESSION_TYPE, ot::JsonString(m_application->projectType(), reply.GetAllocator()), reply.GetAllocator());

	return reply.toJson();
}
