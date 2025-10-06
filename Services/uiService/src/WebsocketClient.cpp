#include "AppBase.h"
#include "WebsocketClient.h"
#include "ExternalServicesComponent.h"

#include <QEventLoop>
#include <QFile>
#include <QCoreApplication>
#include <iostream>
#include <thread>

// OpenTwin header
#include "OTSystem/DateTime.h"
#include "OTCore/JSON.h"
#include "OTCore/LogDispatcher.h"
#include "OTCore/String.h"
#include "OTCore/ReturnMessage.h"
#include "OTCore/BasicScopedBoolWrapper.h"
#include "OTCommunication/Msg.h"
#include "OTCommunication/ActionTypes.h"

// SSL
#include <QtCore/qfile.h>
#include <QtCore/qthread.h>
#include <QtCore/qprocess.h>

WebsocketClient::WebsocketClient(const std::string& _socketUrl) :
	QObject(nullptr), m_isConnected(false), m_currentlyProcessingQueuedMessage(false), 
	m_sessionIsClosing(false), m_unexpectedDisconnect(false), m_bufferHandlingRequested(false)
{
	std::string wsUrl = "wss://" + _socketUrl;
	wsUrl = ot::String::replace(wsUrl, "127.0.0.1", "localhost");
	m_url = QUrl(wsUrl.c_str());

	QString caStr = QProcessEnvironment::systemEnvironment().value("OPEN_TWIN_CA_CERT", "");

	// Check whether cert file ca.pem exists by using environment variable
	if (!QFile::exists(caStr))
	{
		caStr = QCoreApplication::applicationDirPath() + "\\Certificates\\ca.pem";

		// Check whether local cert file ca.pem exists
		if (!QFile::exists(caStr))
		{
			// Get the development root environment variable
			OT_LOG_D("Using development root certificate file");

			caStr = QProcessEnvironment::systemEnvironment().value("OPENTWIN_DEV_ROOT", "") + "\\Deployment\\Certificates\\ca.pem";
		}
		else
		{
			OT_LOG_D("Using local certificate file");
		}
	}
	else
	{
		OT_LOG_D("Using global certificate file (OPEN_TWIN_CA_CERT)");
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

	// Connect signals
	connect(&m_webSocket, &QWebSocket::connected, this, &WebsocketClient::slotConnected);
	connect(&m_webSocket, &QWebSocket::disconnected, this, &WebsocketClient::slotSocketDisconnected);
	connect(&m_webSocket, QOverload<const QList<QSslError>&>::of(&QWebSocket::sslErrors), this, &WebsocketClient::slotSslErrors);

	// Try to connect
	m_webSocket.open(QUrl(m_url));
}

WebsocketClient::~WebsocketClient()
{
	m_webSocket.close();

	// Process any remaining events
	QEventLoop eventLoop;
	eventLoop.processEvents(QEventLoop::ExcludeUserInputEvents | QEventLoop::WaitForMoreEvents);
}

bool WebsocketClient::sendMessage(ot::RelayedMessageHandler::MessageType _type, const std::string& _receiverUrl, const std::string& _message, std::string& _response) {
	// Make sure we are connected
	if (!this->ensureConnection()) {
		return false;
	}

	OT_LOG("Sending message to (Receiver = \"" + _receiverUrl +
		"\"; Endpoint = " + (_type == ot::RelayedMessageHandler::Queue ? "Queue" : (_type == ot::RelayedMessageHandler::Execute ? "Execute" : "Control")) +
		"). Message = \"" + _message + "\"",
		ot::OUTGOING_MESSAGE_LOG
	);

	// Create the request message
	std::string request;
	uint64_t messageId = 0;
	switch (_type) {
	case ot::RelayedMessageHandler::Response:
		OT_LOG_EA("Manual trigger of response message is forbidden");
		break;

	case ot::RelayedMessageHandler::Execute:
		if (!m_messageHandler.createExecuteRequest(_receiverUrl, _message, request, messageId)) {
			OT_LOG_EAS("Could not create relayed message");
			return false;
		}
		break;

	case ot::RelayedMessageHandler::Queue:
		request = m_messageHandler.createQueueRequest(_receiverUrl, _message);
		break;

	case ot::RelayedMessageHandler::Control:
		request = m_messageHandler.createControlRequest(_message);
		break;

	default:
		OT_LOG_E("Unknown request type (" + std::to_string(static_cast<int>(_type)) + ")");
		return false;
	}

	// Send the message
	m_webSocket.sendTextMessage(QString::fromStdString(request));

	// Wait for response if needed
	if (_type == ot::RelayedMessageHandler::Execute) {
		// Wait for the reponse
		while (m_messageHandler.isWaitingForResponse(messageId) && m_isConnected) {
			QEventLoop eventLoop;
			eventLoop.connect(this, &WebsocketClient::responseReceived, &eventLoop, &QEventLoop::quit);
			eventLoop.connect(this, &WebsocketClient::connectionClosed, &eventLoop, &QEventLoop::quit);
			eventLoop.exec(QEventLoop::ExcludeUserInputEvents | QEventLoop::WaitForMoreEvents);
		}

		// We have received a response and return the text
		_response = m_messageHandler.grabResponse(messageId);
	}

	OT_LOG("...Sending message to (Receiver = \"" + _receiverUrl + 
		"\"; Endpoint = " + (_type == ot::RelayedMessageHandler::Queue ? "Queue" : (_type == ot::RelayedMessageHandler::Execute ? "Execute" : "Control")) +
		") completed. Response = \"" + _response + "\"", 
		ot::OUTGOING_MESSAGE_LOG
	);

	// Queue buffer processing if no message is currently processed and buffer is not empty
	this->queueBufferProcessingIfNeeded();

	return true;
}

void WebsocketClient::prepareSessionClosing() {
	// Mark the session as closing, this will avoid processing any new queued messages
	m_sessionIsClosing = true;
	m_newRequests.clear();
	m_currentRequests.clear();
}

void WebsocketClient::updateLogFlags(const ot::LogFlags& _flags) {
	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_SetGlobalLogFlags, doc.GetAllocator()), doc.GetAllocator());
	ot::JsonArray arr;
	ot::addLogFlagsToJsonArray(_flags, arr, doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_Flags, arr, doc.GetAllocator());

	std::string tmp;
	this->sendMessage(ot::RelayedMessageHandler::Control, "", doc.toJson(), tmp);
}

// ###############################################################################################################################################

// Private Slots

void WebsocketClient::slotConnected() {
	OT_LOG_D("Connected to server");

	connect(&m_webSocket, &QWebSocket::textMessageReceived, this, &WebsocketClient::slotMessageReceived);
	m_isConnected = true;

#ifdef _DEBUG
	const std::string request = m_messageHandler.createControlRequest("noinactivitycheck");
	m_webSocket.sendTextMessage(QString::fromStdString(request));
#endif // _DEBUG
}

void WebsocketClient::slotMessageReceived(const QString& _message) {
	ot::RelayedMessageHandler::Request request = m_messageHandler.requestReceived(_message.toStdString());

	switch (request.messageType) {
	case ot::RelayedMessageHandler::Response:
		Q_EMIT responseReceived();
		break;

	case ot::RelayedMessageHandler::Execute:
		this->dispatchExecuteRequest(request);
		break;

	case ot::RelayedMessageHandler::Queue:
		if (m_sessionIsClosing) {
			OT_LOG_D("Ignoring queued message due to session close");
			// Avoid processing any queued messages if the session is closing
			return;
		}

		// Add the command to the new queue commands buffer
		m_newRequests.push_back(std::move(request));

		// Queue buffer processing if no message is currently processed and we are not waiting for a response
		if (!(m_currentlyProcessingQueuedMessage || this->anyWaitingForResponse())) {
			this->queueBufferProcessingIfNeeded();
		}
		break;

	default:
		OT_LOG_EAS("Invalid message type received (" + std::to_string(static_cast<int>(request.messageType)) + ")");
		break;
	}
}

void WebsocketClient::slotSocketDisconnected() {
	if (!m_isConnected) {
		// This message might be sent on an unsuccessful connection attempt (when the relay server is not yet ready).
		// In this case, we can safely ignore this message.
		return;
	}

	OT_LOG_D("Relay server disconnected on websocket");
	m_isConnected = false;
	m_currentRequests.clear();
	m_newRequests.clear();

	if (!m_sessionIsClosing) {
		// This is an unexpected disconnect of the relay service -> we need to close the session
		m_unexpectedDisconnect = true;
		ot::JsonDocument doc;
		doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_ServiceConnectionLost, doc.GetAllocator()), doc.GetAllocator());
		
		
		ot::RelayedMessageHandler::Request data;
		data.message = doc.toJson();

		this->dispatchQueueRequest(data);
	}

	Q_EMIT connectionClosed();
}

void WebsocketClient::slotSslErrors(const QList<QSslError>& _errors) {
	for (const QSslError& error : _errors) {
		OT_LOG_E(error.errorString().toStdString());
	}
	m_webSocket.ignoreSslErrors();
}

void WebsocketClient::slotProcessMessageQueue() {
	m_bufferHandlingRequested = false;

	// Don't process buffer if we are already processing a message or if we are waiting for a response
	if (m_currentlyProcessingQueuedMessage || this->anyWaitingForResponse()) {
		return;
	}

	// Process the first command in the buffer (queue action will process all remaining commands in the buffer)
	if (!m_currentRequests.empty()) {
		ot::RelayedMessageHandler::Request request = std::move(m_currentRequests.front());
		m_currentRequests.pop_front();

		this->dispatchQueueRequest(request);
	}
	else {
		// Move up to 100 of the new commands to the current buffer
		for (int i = 0; i < 100 && !m_newRequests.empty(); i++) {
			m_currentRequests.push_back(std::move(m_newRequests.front()));
			m_newRequests.pop_front();
		}

		// Process the first command in the buffer (queue action will process all remaining commands in the buffer)
		if (!m_currentRequests.empty()) {
			ot::RelayedMessageHandler::Request request = std::move(m_currentRequests.front());
			m_currentRequests.pop_front();
			this->dispatchQueueRequest(request);
		}
	}
}

// ###############################################################################################################################################

// Private helper

void WebsocketClient::dispatchExecuteRequest(ot::RelayedMessageHandler::Request& _data) {
	ot::JsonDocument doc;
	doc.fromJson(_data.message);

	std::string action = ot::json::getString(doc, OT_ACTION_MEMBER);
	std::string response;

	if (action == OT_ACTION_CMD_Ping) {
		response = std::move(action);
	}
	else {
		response = ot::ReturnMessage::toJson(ot::ReturnMessage::Failed, "Frontend only supports \"" OT_ACTION_CMD_Ping "\" as execute requests. Received request: \"" + action + "\"");
	}

	const std::string responseData = m_messageHandler.createResponseRequest(_data.receiverUrl, response, _data.messageId);

	// Send the message
	m_webSocket.sendTextMessage(QString::fromStdString(responseData));
	m_webSocket.flush();
}

void WebsocketClient::dispatchQueueRequest(ot::RelayedMessageHandler::Request& _request) {
	// Dispatch the action to the external services component
	ExternalServicesComponent* ext = AppBase::instance()->getExternalServicesComponent();

	{
		ot::BasicScopedBoolWrapper procWrapper(m_currentlyProcessingQueuedMessage);

		// Dispatch the action
		ext->queueAction(_request.message, _request.receiverUrl);

		// Now dispatch all remaining actions in the buffer
		while (!m_currentRequests.empty() && m_isConnected) {
			ot::RelayedMessageHandler::Request bufferedRequest = std::move(m_currentRequests.front());
			m_currentRequests.pop_front();
			ext->queueAction(bufferedRequest.message, bufferedRequest.receiverUrl);
		}
	}

	// If we have received new commands while processing the current command, we queue buffer processing again
	this->queueBufferProcessingIfNeeded();
}

bool WebsocketClient::ensureConnection() {
	if (!m_isConnected && (m_sessionIsClosing || m_unexpectedDisconnect)) {
		return false;
	}

	int64_t startTime = ot::DateTime::msSinceEpoch();

	while (!m_isConnected) {
		if (m_webSocket.state() == QAbstractSocket::UnconnectedState) {
			// The relay service was probably not running when the connection was tried to establish
			// Try to connect again
			m_webSocket.open(QUrl(m_url));
		}
		
		QEventLoop eventLoop;
		eventLoop.processEvents(QEventLoop::ExcludeUserInputEvents | QEventLoop::WaitForMoreEvents);

		if (!m_isConnected && (ot::DateTime::msSinceEpoch() - startTime) > (5 * ot::msg::defaultTimeout)) {
			OT_LOG_E("Connection to relay service cancelled due to timeout");
			return false;
		}
	}

	if (!m_isConnected) {
		OT_LOG_E("Could not connect to relay service");
		return false;
	}
	else {
		return true;
	}
}

void WebsocketClient::queueBufferProcessingIfNeeded() {
	if (!m_bufferHandlingRequested && (!m_newRequests.empty() || !m_currentRequests.empty()) && m_isConnected) {
		m_bufferHandlingRequested = true;
		QMetaObject::invokeMethod(this, &WebsocketClient::slotProcessMessageQueue, Qt::QueuedConnection);
	}
}

bool WebsocketClient::anyWaitingForResponse() {
	return m_isConnected && m_messageHandler.anyWaitingForResponse();
}
