// Service header
#include "Timeouts.h"
#include "CommunicationHandler.h"

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTCore/ReturnMessage.h"
#include "OTCommunication/ActionTypes.h"

// Qt header
#include <QtCore/qeventloop.h>
#include <QtNetwork/qlocalsocket.h>

// std header
#include <thread>

CommunicationHandler::CommunicationHandler(SubprocessManager* _manager, const std::string& _serverName)
	: m_manager(_manager), m_serverName(_serverName), m_client(nullptr), m_clientState(ClientState::Disconnected)
{
	OTAssertNullptr(m_manager);

	this->connect(this, &CommunicationHandler::newConnection, this, &CommunicationHandler::slotNewConnection);
	this->listen(QString::fromStdString(_serverName));
}

CommunicationHandler::~CommunicationHandler() {
}


bool CommunicationHandler::sendToClient(const ot::JsonDocument& _document, std::string& _response) {
	return this->sendToClient(QByteArray::fromStdString(_document.toJson()), _response);	
}

void CommunicationHandler::slotMessageReceived(void) {
	if (!m_client) {
		OT_LOG_EA("Client not set");
		return;
	}

	// Read message
	std::string message;
	message = m_client->readAll().toStdString();
	
	// Check state
	if (m_clientState == ClientState::WaitForPing) {
		ot::JsonDocument responseDoc;
		if (!responseDoc.fromJson(message)) {
			OT_LOG_E("Invalid client ping response: \"" + message + "\"");
			m_client->disconnect();
			return;
		}
		ot::ReturnMessage msg;
		msg.setFromJsonObject(responseDoc.GetConstObject());
		if (msg.getStatus() != ot::ReturnMessage::Ok || msg.getWhat() != OT_ACTION_CMD_Ping) {
			OT_LOG_E("Invalid client ping response: \"" + message + "\"");
			m_client->disconnect();
			return;
		}
	}
	else if (m_clientState == ClientState::WaitForResponse) {
		m_response = std::move(message);
		m_clientState = ClientState::ReponseReceived;
	}
	else {
		OT_LOG_W("Client send unexpected message: \"" + message + "\"");
	}
}

void CommunicationHandler::slotClientDisconnected(void) {
	OT_LOG_D("Client disconnected");
	if (m_client) {
		delete m_client;
		m_client = nullptr;
	}
	m_clientState = ClientState::Disconnected;
}

void CommunicationHandler::processNextEvent(void) {
	QEventLoop loop;
	loop.processEvents(QEventLoop::ProcessEventsFlag::AllEvents);
}

bool CommunicationHandler::waitForClient(void) {
	const int tickTime = 10;
	int timeout = Timeouts::connectionTimeout / tickTime;
	while (m_clientState != ClientState::Ready) {
		if (timeout--) {
			std::this_thread::sleep_for(std::chrono::microseconds(tickTime));
			this->processNextEvent();
		}
		else {
			OT_LOG_E("Client connection timeout");
			return false;
		}
	}

	return true;
}

bool CommunicationHandler::sendToClient(const QByteArray& _data, std::string& _response) {
	// Ensure client is in ready state
	if (m_clientState != ClientState::Ready) {
		OT_LOG_EA("No client connected");
		return false;
	}

	// Ensure client exists
	if (!m_client) {
		OT_LOG_EA("Fatal Error: Client ready but no socket...");
		return false;
	}

	// Write request
	m_client->write(_data);
	m_client->flush();

	// We know the request was sent successfully, so we set the state to wait for response and wait..
	m_clientState = ClientState::WaitForResponse;

	const int tickTime = 10;
	int timeout = Timeouts::responseTimeout / tickTime;
	while (m_clientState == ClientState::WaitForResponse) {
		if (timeout--) {
			std::this_thread::sleep_for(std::chrono::microseconds(tickTime));
		}
		else {
			OT_LOG_EA("Client response timeout");
			return false;
		}
	}

	// Check if the client has received a response or disconnected.
	if (m_clientState == ClientState::ReponseReceived) {
		_response = m_response;
		m_clientState = ClientState::Ready;
		return true;
	}
	else {
		return false;
	}
}

void CommunicationHandler::slotNewConnection(void) {
	// Get next connection.
	QLocalSocket* newSocket = this->nextPendingConnection();
	if (!newSocket) {
		return;
	}

	// Ensure we don't have a client connected.
	if (m_client) {
		OT_LOG_EAS("Client already connected");

		newSocket->disconnect();
		delete newSocket;
		newSocket = nullptr;
		return;
	}

	// Ensure the state is set correctly
	if (m_clientState != ClientState::Disconnected) {
		OT_LOG_E("Client in undefined state (" + std::to_string((int)m_clientState) + ")");
		return;
	}

	// Store new client information and connect signals
	m_client = newSocket;
	this->connect(m_client, &QLocalSocket::readyRead, this, &CommunicationHandler::slotMessageReceived);
	this->connect(m_client, &QLocalSocket::disconnected, this, &CommunicationHandler::slotClientDisconnected);

	// Send ping to ensure stable connection
	m_clientState = ClientState::WaitForPing;

	ot::JsonDocument pingDoc;
	pingDoc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_Ping, pingDoc.GetAllocator()), pingDoc.GetAllocator());
	m_client->write(QByteArray::fromStdString(pingDoc.toJson()));
	m_client->flush();
}