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
#include "OTCore/Logger.h"
#include "OTCore/String.h"
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

void WebsocketClient::sendMessage(const std::string& _receiverUrl, MessageType _messageType, const std::string& _message, std::string& _response) {
	// Prepare the request
	std::string request;

	switch (_messageType) {
	case WebsocketClient::EXECUTE:
		request = "execute";
		break;

	case WebsocketClient::QUEUE:
		request = "queue";
		break;

	default:
		OT_LOG_EA("Invalid message type");
		request = "execute";
		break;
	}

	request.append("\n").append(_receiverUrl).append("\n").append(_message);
	
	// Make sure we are connected
	if (!this->ensureConnection()) {
		return;
	}

	// Check whether we are already waiting for a response from another service, this should not happen
	if (this->anyWaitingForResponse()) {
		std::string waitingFor;
		for (const auto& it : m_waitingForResponse) {
			if (it.second) {
				if (!waitingFor.empty()) {
					waitingFor.append(", ");
				}
				waitingFor.append("\"" + it.first + "\"");
			}
		}

		OT_LOG_W("Sending message to \"" + _receiverUrl + "\" while waiting for response from [" + waitingFor + "]");
	}

	OT_LOG("Sending message to (Receiver = \"" + _receiverUrl + "\"; Endpoint = " + (_messageType == EXECUTE ? "Execute" : (_messageType == QUEUE ? "Queue" : "<Unknown>")) + "). Message = \"" + _message + "\"", ot::OUTGOING_MESSAGE_LOG);

	// Now send our message	
	m_waitingForResponse[_receiverUrl] = true;
	m_webSocket.sendTextMessage(request.c_str());

	// Wait for the reponse
	while (this->isWaitingForResponse(_receiverUrl)) {
		QEventLoop eventLoop;
		eventLoop.connect(this, &WebsocketClient::responseReceived, &eventLoop, &QEventLoop::quit);
		eventLoop.exec(QEventLoop::ExcludeUserInputEvents);
	}

	// We have received a response and return the text
	_response = m_responseText;

	OT_LOG("...Sending message to (Receiver = \"" + _receiverUrl + "\"; Endpoint = " + (_messageType == EXECUTE ? "Execute" : (_messageType == QUEUE ? "Queue" : "<Unknown>")) + ") completed. Response = \"" + _response + "\"", ot::OUTGOING_MESSAGE_LOG);

	// Queue buffer processing if no message is currently processed and buffer is not empty
	this->queueBufferProcessingIfNeeded();
}

void WebsocketClient::prepareSessionClosing() {
	// Mark the session as closing, this will avoid processing any new queued messages
	m_sessionIsClosing = true;
	m_newQueueCommands.clear();
	m_commandsBuffer.clear();
}

// ###############################################################################################################################################

// Private Slots

void WebsocketClient::slotConnected() {
	OT_LOG_D("Connected to server");

	connect(&m_webSocket, &QWebSocket::textMessageReceived, this, &WebsocketClient::slotMessageReceived);
	m_isConnected = true;

#ifdef _DEBUG
	m_webSocket.sendTextMessage("noinactivitycheck");
#endif // _DEBUG

}

void WebsocketClient::slotMessageReceived(const QString& _message) {
	const std::string stdMessage = _message.toStdString();

	// Split the message into action, sender IP and data
	size_t index1 = stdMessage.find('\n');
	if (index1 == std::string::npos) {
		OT_LOG_EA("Invalid message received from relay service (no action line)");
		return;
	}

	int index2 = stdMessage.find('\n', index1 + 1);
	if (index2 == std::string::npos) {
		OT_LOG_EA("Invalid message received from relay service (no sender line)");
		return;
	}

	CommandData data;
	data.action = stdMessage.substr(0, index1);
	data.senderIp = stdMessage.substr(index1 + 1, index2 - index1 - 1);
	data.data = stdMessage.substr(index2 + 1);

	// Process the action
	if (data.action == "response") {
		m_responseText = std::move(data.data);
		m_waitingForResponse[data.senderIp] = false;

		Q_EMIT responseReceived();
	}
	else if (data.action == "execute") {
		OT_LOG_EA("No execute requests to frontend allowed");
	}
	else if (data.action == "queue") {
		if (m_sessionIsClosing) {
			OT_LOG_D("Ignoring queued message due to session close");
			// Avoid processing any queued messages if the session is closing
			return;
		}

		// Add the command to the new queue commands buffer
		m_newQueueCommands.push_back(std::move(data));

		// Queue buffer processing if no message is currently processed and we are not waiting for a response
		if (!(m_currentlyProcessingQueuedMessage || this->anyWaitingForResponse())) {
			this->queueBufferProcessingIfNeeded();
		}
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
	m_commandsBuffer.clear();
	m_newQueueCommands.clear();

	if (!m_sessionIsClosing) {
		// This is an unexpected disconnect of the relay service -> we need to close the session
		m_unexpectedDisconnect = true;
		ot::JsonDocument doc;
		doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_ServiceConnectionLost, doc.GetAllocator()), doc.GetAllocator());
		
		
		CommandData data;
		data.data = doc.toJson();

		this->dispatchQueueAction(data);
	}
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

	// Move new commands to the main buffer
	m_commandsBuffer.splice(m_commandsBuffer.end(), std::move(m_newQueueCommands));
	m_newQueueCommands.clear();

	// Process the first command in the buffer (queue action will process all remaining commands in the buffer)
	if (!m_commandsBuffer.empty()) {
		CommandData data = std::move(m_commandsBuffer.front());
		m_commandsBuffer.pop_front();

		this->dispatchQueueAction(data);
	}
}

// ###############################################################################################################################################

// Private helper

void WebsocketClient::processMessages()
{
	QEventLoop eventLoop;		
	eventLoop.processEvents(QEventLoop::ExcludeUserInputEvents | QEventLoop::WaitForMoreEvents);
}

void WebsocketClient::dispatchQueueAction(CommandData& _data) {
	// Dispatch the action to the external services component
	ExternalServicesComponent* ext = AppBase::instance()->getExternalServicesComponent();

	ot::BasicScopedBoolWrapper procWrapper(m_currentlyProcessingQueuedMessage);
	
	// Dispatch the action
	ext->queueAction(_data.data, _data.senderIp);
	
	// Now dispatch all remaining actions in the buffer
	while (!m_commandsBuffer.empty() && m_isConnected) {
		CommandData data = std::move(m_commandsBuffer.front());
		m_commandsBuffer.pop_front();
		ext->queueAction(data.data, data.senderIp);
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
	if (!m_bufferHandlingRequested && (!m_newQueueCommands.empty() || !m_commandsBuffer.empty()) && m_isConnected) {
		m_bufferHandlingRequested = true;
		QMetaObject::invokeMethod(this, &WebsocketClient::slotProcessMessageQueue, Qt::QueuedConnection);
	}
}

bool WebsocketClient::anyWaitingForResponse() const {
	if (!m_isConnected) {
		return false;
	}

	for (const auto& it : m_waitingForResponse) {
		if (it.second) {
			return true;
		}
	}
	return false;
}

bool WebsocketClient::isWaitingForResponse(const std::string& _senderIP) const {
	const auto it = m_waitingForResponse.find(_senderIP);
	if (it == m_waitingForResponse.end()) {
		return false;
	}
	else {
		return m_isConnected && it->second;
	}
}
