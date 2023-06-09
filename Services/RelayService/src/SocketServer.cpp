

#include "SocketServer.h"

#include <Logger.h>

// OpenTwin header
#include "OpenTwinCore/rJSON.h"				// rapidjson wrapper
#include "OpenTwinCore/Logger.h"		// Logger
#include "OpenTwinCommunication/ActionTypes.h"		// action member and types definition
#include "OpenTwinCommunication/Msg.h"				// message sending

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

std::string SocketServer::sendWSMessage(const std::string operation, const std::string senderIP, const std::string jsonData)
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
	while (!responseReceived)
	{
		processMessages();
	}

	return responseText;
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
		OT_rJSON_parseDOC(doc, json);
		OT_rJSON_docCheck(doc);

		std::string action = ot::rJSON::getString(doc, "action");

		OT_LOG("Received HTTP execute message: " + action, ot::INBOUND_MESSAGE_LOG);

		if (action == OT_ACTION_CMD_ShutdownRequestedByService) {
			shutdown();
		}

		std::string response = sendWSMessage("execute", senderIP, json);

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
		OT_rJSON_parseDOC(doc, json);
		OT_rJSON_docCheck(doc);	

		std::string action = ot::rJSON::getString(doc, "action");

		OT_LOG_I("Received HTTP queue message: " + action);

		if (action == OT_ACTION_CMD_ShutdownRequestedByService) {
			shutdown();
		}
		
		std::string response = sendWSMessage("queue", senderIP, json);

		OT_LOG_I("Returning received websocket answer to HTTP sender");

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