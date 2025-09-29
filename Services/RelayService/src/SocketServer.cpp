// Relay Service header
#include "SocketServer.h"

// OpenTwin header
#include "OTSystem/AppExitCodes.h"
#include "OTCore/JSON.h"						// rapidjson wrapper
#include "OTCore/LogDispatcher.h"						// Logger
#include "OTCore/ReturnMessage.h"
#include "OTCommunication/Msg.h"				// message sending
#include "OTCommunication/ActionTypes.h"		// action member and types definition
#include "OTCommunication/ServiceInitData.h"

// Qt header
#include <QtCore/qfile.h>
#include <QtCore/qdebug.h>
#include <QtCore/qeventloop.h>
#include <QtCore/qcoreapplication.h>
#include <QtNetwork/qsslkey.h>
#include <QtNetwork/qsslcertificate.h>
#include <QtWebSockets/qwebsocket.h>
#include <QtWebSockets/qwebsocketserver.h>
#include <QProcessEnvironment>

// std header
#include <iostream>

SocketServer& SocketServer::instance() {
	static SocketServer g_instance;
	return g_instance;
}

bool SocketServer::startServer() {
	OT_LOG_D("Setting up websocket \"" + m_websocketIp + "\"");

	QHostAddress serverAddress;
	serverAddress.setAddress(m_websocketIp.c_str());

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

	m_pWebSocketServer = new QWebSocketServer(QStringLiteral("SSL Websocket Server"), QWebSocketServer::SecureMode, this);
	m_pWebSocketServer->setSslConfiguration(sslConfiguration);

	// Initialize the performance counters
	m_systemLoad.initialize();

	OT_LOG_D("Starting websocket");
	if (m_pWebSocketServer->listen(serverAddress, m_websocketPort))
	{
		OT_LOG_I("Websocket server listening on: " + m_websocketIp + ":" + std::to_string(m_websocketPort));
		connect(m_pWebSocketServer, &QWebSocketServer::newConnection, this, &SocketServer::onNewConnection);
		connect(m_pWebSocketServer, &QWebSocketServer::closed, this, &SocketServer::slotSocketClosed);
		connect(m_pWebSocketServer, &QWebSocketServer::sslErrors, this, &SocketServer::onSslErrors);
		return true;
	}
	else {
		OT_LOG_E("Failed to start websocket server");
		return false;
	}
}

void SocketServer::startSessionServiceHealthCheck(const std::string& _lssUrl) {
	if (m_lssHealthCheckRunning || m_lssHealthCheckThread) {
		OT_LOG_W("Local session service health check already running");
		return;
	}

	m_lssUrl = _lssUrl;
	m_lssHealthCheckRunning = true;
	m_lssHealthCheckThread = new std::thread(&SocketServer::lssHealthCheckWorker, this);
}

// ###########################################################################################################################################################################################################################################################################################################################

// Public: Slots

QString SocketServer::performAction(const char* _json, const char* _senderIP)
{
	try {
		ot::JsonDocument doc;
		doc.fromJson(_json);

		std::string action = ot::json::getString(doc, OT_ACTION_MEMBER);

		OT_LOG("Received HTTP execute message \"" + action + "\"", ot::INBOUND_MESSAGE_LOG);

		if (action == OT_ACTION_CMD_GetSystemInformation)
		{
			return getSystemInformation().c_str();
		}
		else if (action == OT_ACTION_CMD_CheckRelayStartupCompleted) {
			if (!m_pWebSocketServer) {
				return QString::fromStdString(ot::ReturnMessage::toJson(ot::ReturnMessage::False));
			}
			else if (m_pWebSocketServer->isListening()) {
				ot::ServiceInitData initData;
				initData.setFromJsonObject(ot::json::getObject(doc, OT_ACTION_PARAM_IniData));

				m_serviceId = initData.getServiceID();
				this->startSessionServiceHealthCheck(initData.getSessionServiceURL());
				
				return QString::fromStdString(ot::ReturnMessage::toJson(ot::ReturnMessage::True));
			}
			else {
				return QString::fromStdString(ot::ReturnMessage::toJson(ot::ReturnMessage::False));
			}
		}
		else if (action == OT_ACTION_CMD_ShutdownRequestedByService) {
			shutdown();
			return OT_ACTION_RETURN_VALUE_OK;
		}
		else if (action == OT_ACTION_CMD_Init || action == OT_ACTION_CMD_Run) {
			this->sendQueueWSMessage(_senderIP, _json);
			return QString::fromStdString(ot::ReturnMessage::toJson(ot::ReturnMessage::Ok));
		}
		else if (action == OT_ACTION_CMD_Ping) {
			std::string response;
			if (this->sendProcessWSMessage(_senderIP, _json, response)) {
				return QString::fromStdString(response);
			}
			else {
				return QString::fromStdString(ot::ReturnMessage::toJson(ot::ReturnMessage::Failed, "Failed to send ping action document trough websocket"));
			}
		}

		OT_LOG_E("Received HTTP execute message (not yet suported by relay service): " + action);

		//std::string response = sendProcessWSMessage("execute", _senderIP, _json);

		//QString retVal = response.c_str();

		//OT_LOG_D("Returning received websocket answer to HTTP sender");

		return "";
	}
	catch (const std::exception& e) {
		OT_LOG_E(std::string(e.what()));
		return OT_ACTION_RETURN_INDICATOR_Error + QString(e.what());
	}
	catch (...) {
		OT_LOG_E("[FATAL] Unknown error");
		return OT_ACTION_RETURN_INDICATOR_Error "[FATAL] Unknown error";
	}
}

void SocketServer::queueAction(const char* _json, const char* _senderIP)
{
	try {
		ot::JsonDocument doc;
		doc.fromJson(_json);

		std::string action = ot::json::getString(doc, "action");

		OT_LOG("Received HTTP queue message: " + action, ot::QUEUED_INBOUND_MESSAGE_LOG);

		if (action == OT_ACTION_CMD_ShutdownRequestedByService) {
			shutdown();
		}

		this->sendQueueWSMessage(_senderIP, _json);

		if (_json) {
			delete[] _json;
			_json = nullptr;
		}
		if (_senderIP) {
			delete[] _senderIP;
			_senderIP = nullptr;
		}
	}
	catch (const std::exception& e) {
		OT_LOG_E(std::string(e.what()));
	}
	catch (...) {
		OT_LOG_EA("[FATAL] Unknown error");
	}
}

void SocketServer::deallocateData(const char* _data) {
	if (_data != nullptr) {
		delete[] _data;
	}
	_data = nullptr;
}

// ###########################################################################################################################################################################################################################################################################################################################

// Private: Slots

void SocketServer::onNewConnection()
{
	QWebSocket* pSocket = m_pWebSocketServer->nextPendingConnection();

	if (m_client) {
		OT_LOG_W("New client connection request received, but a client is already connected. Rejecting new connection.");
		pSocket->close();
		pSocket->deleteLater();
		return;
	}

	OT_LOG_D("New client connected on websocket");

    connect(pSocket, &QWebSocket::textMessageReceived, this, &SocketServer::messageReceived);
    connect(pSocket, &QWebSocket::disconnected, this, &SocketServer::socketDisconnected);

	m_lastReceiveTime = std::chrono::system_clock::now();

	if (m_keepAliveTimer == nullptr) {
		m_keepAliveTimer = new QTimer(this);
		connect(m_keepAliveTimer, SIGNAL(timeout()), this, SLOT(keepAlive()));
		m_keepAliveTimer->start(30000);
	}
	else {
		OT_LOG_E("Keep alive timer already set");
	}

	m_client = pSocket;
}

void SocketServer::messageReceived(const QString& _message) {
	m_lastReceiveTime = std::chrono::system_clock::now();
	OT_LOG("Message received trough websocket: " + _message.toStdString(), ot::OUTGOING_MESSAGE_LOG);
	try {
		ot::RelayedMessageHandler::Request request = m_messageHandler.requestReceived(_message.toStdString());
		std::string response;

		switch (request.messageType) {
		case ot::RelayedMessageHandler::Response:
			Q_EMIT responseReceived();
			break;

		case ot::RelayedMessageHandler::Queue:
			this->relayToHttp(request, response);
			break;

		case ot::RelayedMessageHandler::Execute:
		{
			this->relayToHttp(request, response);
			
			if (m_client) {
				std::string responseRequest = m_messageHandler.createResponseRequest(request.receiverUrl, response, request.messageId);
				m_client->sendTextMessage(QString::fromStdString(responseRequest));
			}
			else {
				OT_LOG_E("No client connected to websocket");
			}
		}
			break;

		case ot::RelayedMessageHandler::Control:
			if (request.message == "noinactivitycheck") {
				if (m_keepAliveTimer) {
					OT_LOG_W("Keep alive timer stopped");
					m_keepAliveTimer->stop();
					delete m_keepAliveTimer;
					m_keepAliveTimer = nullptr;
				}
			}
			else {
				ot::JsonDocument controlDoc;
				if (!controlDoc.fromJson(request.message)) {
					OT_LOG_W("Invalid control message: " + request.message);
					return;
				}

				if (!controlDoc.HasMember(OT_ACTION_MEMBER)) {
					OT_LOG_W("Invalid control message syntax (action missing): " + request.message);
					return;
				}

				std::string action = ot::json::getString(controlDoc, OT_ACTION_MEMBER);
				if (action == OT_ACTION_CMD_SetGlobalLogFlags) {
					ot::LogFlags flags = ot::logFlagsFromJsonArray(ot::json::getArray(controlDoc, OT_ACTION_PARAM_Flags));
					ot::LogDispatcher::instance().setLogFlags(flags);
				}
			}
			break;

		default:
			OT_LOG_E("Unknown message type received on websocket (" + std::to_string(static_cast<int>(request.messageType)) + ")");
			break;
		}
	}
	catch (const std::exception & e) {
		OT_LOG_E(std::string(e.what()));
	}
	catch (...) {
		OT_LOG_EA("[FATAL] Unknown error");
	}
}

void SocketServer::socketDisconnected() {
	OT_LOG_D("Client disconnected on websocket");

	if (m_client) {
		m_client->deleteLater();
	}

	exit(ot::AppExitCode::Success);
}

void SocketServer::onSslErrors(const QList<QSslError>& _errors)
{
	// TODO: HANDLE SSL ERRORS?
	for (const QSslError& e : _errors) {
		OT_LOG_E("SSL error occurred: " + e.errorString().toStdString());
	}
}

void SocketServer::slotSocketClosed() {
	OT_LOG_D("Socket closed");
	if (m_pWebSocketServer) {
		delete m_pWebSocketServer;
		m_pWebSocketServer = nullptr;
	}

	Q_EMIT connectionClosed();

	QCoreApplication::quit();
}

void SocketServer::keepAlive() const
{
	auto currentTime = std::chrono::system_clock::now();

	std::chrono::duration<double> elapsed_seconds = currentTime - m_lastReceiveTime;

	if (elapsed_seconds.count() > 120.0)
	{
		// We have not heard from the frontend for more than two minutes (altough we should at least get a ping every 60 seconds)
		// Therefore, we assume that the connection was interrupted, but we did not get a socket closed message.
		// In order to avoid blocking the session, we will now terminate 
		
		OT_LOG_E("No data received from frontend for more than 120 seconds. Assuming connection is lost");

		exit(ot::AppExitCode::KeepAliveFailed);
	}
}

// ###########################################################################################################################################################################################################################################################################################################################

// Private: Helper

SocketServer::SocketServer()
	: QObject(nullptr), m_pWebSocketServer(nullptr), m_websocketPort(0), m_keepAliveTimer(nullptr), 
	m_client(nullptr), m_serviceId(ot::invalidServiceID), m_lssHealthCheckThread(nullptr), m_lssHealthCheckRunning(false)
{

}

SocketServer::~SocketServer() {
	m_lssHealthCheckRunning = false;

	// Stop keep alive timer
	if (m_keepAliveTimer != nullptr) {
		m_keepAliveTimer->stop();
		delete m_keepAliveTimer;

		m_keepAliveTimer = nullptr;
	}

	// Disconnect client
	if (m_client) {
		disconnect(m_client, &QWebSocket::disconnected, this, &SocketServer::socketDisconnected);
		m_client->close();
		delete m_client;
		m_client = nullptr;
	}

	// Close server
	if (m_pWebSocketServer) {
		m_pWebSocketServer->close();
		delete m_pWebSocketServer;
		m_pWebSocketServer = nullptr;
	}

	// Stop LSS health check
	if (m_lssHealthCheckThread) {
		if (m_lssHealthCheckThread->joinable()) {
			m_lssHealthCheckThread->join();
		}
		delete m_lssHealthCheckThread;
		m_lssHealthCheckThread = nullptr;
	}
}

void SocketServer::shutdown() {
	OT_LOG_D("Shutting down relay service");

	// Exit the application
	exit(ot::AppExitCode::Success);
}

bool SocketServer::sendQueueWSMessage(const std::string& _senderIP, const std::string& _jsonData) {
	if (!m_client) {
		OT_LOG_E("No client connected to websocket");
		return false;
	}

	// Build and send the message through the websocket connection

	OT_LOG_D("Relaying received queue message to websocket");

	const std::string request = m_messageHandler.createQueueRequest(_senderIP, _jsonData);
	m_client->sendTextMessage(QString::fromStdString(request));

	return true;
}

bool SocketServer::sendProcessWSMessage(const std::string& _senderIP, const std::string& _jsonData, std::string& _response) {
	if (!m_client) {
		OT_LOG_E("No client connected to websocket");
		return false;
	}

	// Build and send the message through the websocket connection

	std::string request;
	uint64_t messageId = 0;
	if (!m_messageHandler.createExecuteRequest(_senderIP, _jsonData, request, messageId)) {
		OT_LOG_E("Failed to create request message");
		return false;
	}

	OT_LOG_D("Relaying received execute message to websocket and wait for response");

	m_client->sendTextMessage(QString::fromStdString(request));

	while (m_messageHandler.isWaitingForResponse(messageId) && m_pWebSocketServer) {
		QEventLoop loop;
		connect(this, &SocketServer::responseReceived, &loop, &QEventLoop::quit);
		connect(this, &SocketServer::connectionClosed, &loop, &QEventLoop::quit);
		loop.exec(QEventLoop::ExcludeUserInputEvents);
	}

	_response = m_messageHandler.grabResponse(messageId);

	return true;
}

bool SocketServer::relayToHttp(const ot::RelayedMessageHandler::Request& _request, std::string& _response) {
	try {
		bool success = false;

		if (_request.messageType == ot::RelayedMessageHandler::Execute) {
			success = ot::msg::send(m_relayUrl, _request.receiverUrl, ot::EXECUTE, _request.message, _response);
		}
		else if (_request.messageType == ot::RelayedMessageHandler::Queue) {
			success = ot::msg::send(m_relayUrl, _request.receiverUrl, ot::QUEUE, _request.message, _response);
		}
		else {
			OT_LOG_EAS("Unknown message type (" + std::to_string(static_cast<int>(_request.messageType)) + ")");
			success = false;
		}

		return success;
	}
	catch (const std::exception& e) {
		OT_LOG_E(e.what());
		return false;
	}
	catch (...) {
		OT_LOG_EA("[FATAL] Unknown error");
		return false;
	}

	return false;
}

void SocketServer::lssHealthCheckWorker() {
	// Create ping request
	ot::JsonDocument pingDoc;
	pingDoc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_Ping, pingDoc.GetAllocator()), pingDoc.GetAllocator());
	std::string ping = pingDoc.toJson();

	OT_LOG_D("Starting LSS health check at \"" + m_lssUrl + "\"");

	bool alive = true;
	while (m_lssHealthCheckRunning) {
		// Try to send message and check the response
		std::string ret;
		try {
			if (!ot::msg::send("", m_lssUrl, ot::EXECUTE, ping, ret)) {
				alive = false;
				break;
			}
			else if (ret != OT_ACTION_CMD_Ping) {
				alive = false;
				break;
			}
		}
		catch (...) {
			alive = false;
			break;
		}

		int ct = 0;
		while (++ct <= 6000 && m_lssHealthCheckRunning) {
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
		}
	}

	if (!alive) {
		OT_LOG_E("Local session service \"" + m_lssUrl + "\" has died unexpectedly. Shutting down...");
		exit(ot::AppExitCode::LSSNotRunning);
	}
}

std::string SocketServer::getSystemInformation() {
	double globalCpuLoad = 0, globalMemoryLoad = 0, processCpuLoad = 0, processMemoryLoad = 0;

	m_systemLoad.getGlobalCPUAndMemoryLoad(globalCpuLoad, globalMemoryLoad);
	m_systemLoad.getCurrentProcessCPUAndMemoryLoad(processCpuLoad, processMemoryLoad);

	ot::JsonDocument reply;
	reply.AddMember(OT_ACTION_PARAM_GLOBAL_CPU_LOAD, globalCpuLoad, reply.GetAllocator());
	reply.AddMember(OT_ACTION_PARAM_GLOBAL_MEMORY_LOAD, globalMemoryLoad, reply.GetAllocator());
	reply.AddMember(OT_ACTION_PARAM_PROCESS_CPU_LOAD, processCpuLoad, reply.GetAllocator());
	reply.AddMember(OT_ACTION_PARAM_PROCESS_MEMORY_LOAD, processMemoryLoad, reply.GetAllocator());

	reply.AddMember(OT_ACTION_PARAM_SERVICE_TYPE, ot::JsonString("RelayService", reply.GetAllocator()), reply.GetAllocator());
	
	return reply.toJson();
}
