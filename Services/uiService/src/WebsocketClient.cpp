#include "WebsocketClient.h"

#include <QEventLoop>
#include <QFile>
#include <QCoreApplication>
#include <iostream>
#include <thread>

// OpenTwin header
#include "OTCore/JSON.h"
#include "OTCore/Logger.h"
#include "OTCore/StringHelper.h"
#include "OTCommunication/ActionTypes.h"

// SSL
#include <QtCore/QFile>
#include <QProcessEnvironment>

extern "C"
{
	_declspec(dllexport) const char *performAction(const char *json, const char *senderIP);
	_declspec(dllexport) const char *queueAction(const char *json, const char *senderIP);
	_declspec(dllexport) void deallocateData(const char *data);
}

WebsocketClient::WebsocketClient(const std::string& _socketUrl) :
	QObject(nullptr), m_isConnected(false), m_currentlyProcessingQueuedMessage(false), m_sessionIsClosing(false)
{
	std::string wsUrl = "wss://" + _socketUrl;
	wsUrl = ot::stringReplace(wsUrl, "127.0.0.1", "localhost");
	m_url = QUrl(wsUrl.c_str());

	QString caStr = QCoreApplication::applicationDirPath() + "\\Certificates\\ca.pem";

	// Check whether local cert file ca.pem exists
	if (!QFile::exists("caStr"))
	{
		// Get the development root environment variable
		std::cout << "Using development root certificate file" << std::endl;

		caStr = QProcessEnvironment::systemEnvironment().value("OPENTWIN_DEV_ROOT", "") + "\\Deployment\\Certificates\\ca.pem";
	}
	else
	{
		std::cout << "Using local certificate file" << std::endl;
	}

	// SSL/ TLS Configuration
	QSslConfiguration sslConfiguration;
		
	QFile caCertFile(caStr);
	caCertFile.open(QIODevice::ReadOnly);
	QSslCertificate caCert(&caCertFile, QSsl::Pem);
	caCertFile.close();

	
	QList<QSslCertificate> caCerts = sslConfiguration.caCertificates();
	caCerts.append(caCert);
	sslConfiguration.setCaCertificates(caCerts);
	
	m_webSocket.setSslConfiguration(sslConfiguration);

	connect(&m_webSocket, &QWebSocket::connected, this, &WebsocketClient::slotConnected);
	connect(&m_webSocket, &QWebSocket::disconnected, this, &WebsocketClient::slotSocketDisconnected);

	connect(&m_webSocket, QOverload<const QList<QSslError>&>::of(&QWebSocket::sslErrors),
		this, &WebsocketClient::slotSslErrors);

	m_webSocket.open(QUrl(m_url));

}

WebsocketClient::~WebsocketClient()
{
	m_webSocket.close();

	processMessages();
}

void WebsocketClient::sendMessage(const std::string& _message, std::string& _response) {
	size_t index1 = _message.find('\n');
	size_t index2 = _message.find('\n', index1 + 1);

	std::string senderIP = _message.substr(index1 + 1, index2 - index1 - 1);

	// Make sure we are connected
	if (!ensureConnection()) return;

	// Now send our message	
	m_waitingForResponse[senderIP] = true;
	m_webSocket.sendTextMessage(_message.c_str());

	// Wait for the reponse
	while (m_waitingForResponse[senderIP]) {
		processMessages();
	}

	// We have received a response and return the text
	_response = m_responseText;
}

void WebsocketClient::sendResponse(const std::string& _message) {
	// Make sure we are connected
	if (!ensureConnection()) return;

	// Send the reponse
	m_webSocket.sendTextMessage(_message.c_str());
}

void WebsocketClient::finishedProcessingQueuedMessage(void) {
	assert(m_currentlyProcessingQueuedMessage);
	m_currentlyProcessingQueuedMessage = false;

	if (!m_commandQueue.empty()) {
		QString message = m_commandQueue.front();
		m_commandQueue.pop_front();

		this->slotMessageReceived(message);
	}
}

// ###############################################################################################################################################

// Private Slots

void WebsocketClient::slotConnected() {
	OT_LOG_D("Client connected");

	connect(&m_webSocket, &QWebSocket::textMessageReceived, this, &WebsocketClient::slotMessageReceived);
	m_isConnected = true;
}

void WebsocketClient::slotMessageReceived(const QString& _message)
{
	// Get the action from the message
	int index1 = _message.indexOf('\n');
	int index2 = _message.indexOf('\n', index1 + 1);

	QString action = _message.left(index1);
	QString senderIP = _message.mid(index1 + 1, index2 - index1 - 1);

	// Process the action
	if (action == "response")
	{
		m_responseText = _message.mid(index2 + 1).toStdString();
		m_waitingForResponse[senderIP.toStdString()] = false;
	}
	else if (action == "execute")
	{
		if (m_currentlyProcessingQueuedMessage || m_waitingForResponse[senderIP.toStdString()])
		{
			m_commandQueue.push_back(_message);
		}
		else
		{
			sendExecuteOrQueueMessage(_message);
		}
	}
	else if (action == "queue")
	{
		if (m_currentlyProcessingQueuedMessage || m_waitingForResponse[senderIP.toStdString()])
		{
			m_commandQueue.push_back(_message);
		}
		else
		{
			sendExecuteOrQueueMessage(_message);
		}
	}
}

void WebsocketClient::slotSocketDisconnected() {
	if (!m_isConnected) return; // This message might be sent on an unsuccessful connection attempt (when the relay server is not yet ready). In this case, we can 
	// safely ignore this message.

	OT_LOG_D("Relay server disconnected on websocket");
	m_isConnected = false;

	if (!m_sessionIsClosing) {
		// This is an unexpected disconnect of the relay service -> we need to close the session
		ot::JsonDocument doc;
		doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_ServiceEmergencyShutdown, doc.GetAllocator()), doc.GetAllocator());
		m_currentlyProcessingQueuedMessage = true;
		queueAction(doc.toJson().c_str(), "");
	}
}

void WebsocketClient::slotSslErrors(const QList<QSslError>& errors) {
	//Q_UNUSED(errors);

	foreach(QSslError err, errors) {
		OT_LOG_E(err.errorString().toStdString());
	}
	// WARNING: Never ignore SSL errors in production code.
	// The proper way to handle self-signed certificates is to add a custom root
	// to the CA store.

	// TODO: REMOVE THIS AFTER Qt UPDATE
	m_webSocket.ignoreSslErrors();
}

// ###############################################################################################################################################

// Private helper

void WebsocketClient::processMessages(void) 
{
	QEventLoop eventLoop;		
	eventLoop.processEvents(QEventLoop::ExcludeUserInputEvents | QEventLoop::WaitForMoreEvents);
}

void WebsocketClient::sendExecuteOrQueueMessage(QString message)
{
	// We have received a message through the websocket and need to forward it to the standard
	// messaging system for dispatching received messages.

	int index1 = message.indexOf('\n');
	int index2 = message.indexOf('\n', index1 + 1);

	QString action = message.left(index1);
	QString senderIP = message.mid(index1 + 1, index2 - index1 - 1);
	QString jsonData = message.mid(index2 + 1);

	const char *response = nullptr;

	if (action == "execute")
	{
		response = performAction(jsonData.toStdString().c_str(), senderIP.toStdString().c_str());
	}
	else if (action == "queue")
	{
		m_currentlyProcessingQueuedMessage = true;
		response = queueAction(jsonData.toStdString().c_str(), senderIP.toStdString().c_str());
	}
	else
	{
		assert(0); // Unknown action.
		return;
	}

	// The message was processed and we need to relay the reponse
	std::string returnMessage = "response\n";

	if (response != nullptr)
	{
		returnMessage += response;
		deallocateData(response);
	}

	sendResponse(returnMessage);
}

bool WebsocketClient::ensureConnection(void) {
	if (!m_isConnected && m_sessionIsClosing) return false;

	while (!m_isConnected) {
		if (m_webSocket.state() == QAbstractSocket::UnconnectedState) {
			// The relay service was probably not running when the connection was tried to establish
			// Try to connect again
			m_webSocket.open(QUrl(m_url));
		}
		processMessages();
	}
	assert(m_isConnected);

	return m_isConnected;
}
