// Service header
#include "Timeouts.h"
#include "Application.h"
#include "CommunicationHandler.h"
#include "SubprocessManager.h"

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTCore/ReturnMessage.h"
#include "OTCommunication/ActionTypes.h"
#include "OTServiceFoundation/UiComponent.h"

// Qt header
#include <QtCore/qeventloop.h>
#include <QtNetwork/qlocalsocket.h>

// std header
#include <thread>

CommunicationHandler::CommunicationHandler(SubprocessManager* _manager, const std::string& _serverName)
	: m_manager(_manager), m_serverName(_serverName), m_client(nullptr), m_clientState(ClientState::Disconnected),
	m_frontendUrlSet(false), m_modelUrlSet(false), m_databaseInfoSet(false), m_isInitializingClient(false), m_serviceAndSessionInfoSet(false)
{
	OTAssertNullptr(m_manager);

	this->connect(this, &CommunicationHandler::newConnection, this, &CommunicationHandler::slotNewConnection);
	this->listen(QString::fromStdString(_serverName));
}

CommunicationHandler::~CommunicationHandler() {
}

bool CommunicationHandler::sendToClient(const ot::JsonDocument& _document, std::string& _response) {
	if (!this->waitForClient()) {
		return false;
	}

	QByteArray request = QByteArray::fromStdString(_document.toJson());
	request.append('\n');
	return this->sendToClient(request, true, _response);	
}

bool CommunicationHandler::sendConfigToClient(void) {
	OT_LOG_D("Sending configuration to client");

	if (!this->sendServiceInfoToClient()) {
		return false;
	}
	if (!this->sendModelConfigToClient()) {
		return false;
	}
	if (!this->sendFrontendConfigToClient()) {
		return false;
	}
	if (!this->sendDataBaseConfigToClient()) {
		return false;
	}
	
	OT_LOG_D("Client configuration completed");

	m_isInitializingClient = false;
	return true;
}

void CommunicationHandler::cleanupAfterCrash(void) {
	this->slotClientDisconnected();
}

void CommunicationHandler::slotMessageReceived(void) {
	if (!m_client) {
		OT_LOG_EA("Client not set");
		return;
	}

	// Read message
	std::string message;
	message = m_client->readAll().toStdString();
	
	QMetaObject::invokeMethod(this, "slotProcessMessage", Qt::QueuedConnection, Q_ARG(std::string, message));
}

void CommunicationHandler::slotClientDisconnected(void) {
	OT_LOG_D("Client disconnected");
	if (m_client) {
		delete m_client;
		m_client = nullptr;
	}

	m_clientState = ClientState::Disconnected;
	
	m_serviceAndSessionInfoSet = false;
	m_modelUrlSet = false;
	m_frontendUrlSet = false;
	m_databaseInfoSet = false;

	m_isInitializingClient = false;
}

void CommunicationHandler::processNextEvent(void) {
	QEventLoop loop;
	loop.processEvents(QEventLoop::ProcessEventsFlag::AllEvents);
}

bool CommunicationHandler::sendServiceInfoToClient(void) {
	if (!m_serviceAndSessionInfoSet) {
		ot::JsonDocument doc;
		doc.AddMember(OT_ACTION_PARAM_MODEL_ActionName, ot::JsonString(OT_ACTION_CMD_Init, doc.GetAllocator()), doc.GetAllocator());
		doc.AddMember(OT_ACTION_PARAM_SERVICE_NAME, ot::JsonString(OT_INFO_SERVICE_TYPE_PYRIT, doc.GetAllocator()), doc.GetAllocator());
		doc.AddMember(OT_ACTION_PARAM_SESSION_COUNT, Application::instance()->getSessionCount(), doc.GetAllocator());
		doc.AddMember(OT_ACTION_PARAM_SERVICE_ID, Application::instance()->getServiceID(), doc.GetAllocator());

		QByteArray request = QByteArray::fromStdString(doc.toJson());
		request.append('\n');

		std::string response;
		if (!this->sendToClient(request, true, response)) {
			OT_LOG_E("Failed to send model info to client");
			return false;
		}

		m_serviceAndSessionInfoSet = true;
	}

	return true;
}

bool CommunicationHandler::sendModelConfigToClient(void) {
	if (!m_modelUrl.empty() && !m_modelUrlSet) {
		ot::JsonDocument doc;
		doc.AddMember(OT_ACTION_PARAM_MODEL_ActionName, ot::JsonString(OT_ACTION_CMD_Init, doc.GetAllocator()), doc.GetAllocator());
		doc.AddMember(OT_ACTION_PARAM_SERVICE_NAME, ot::JsonString(OT_INFO_SERVICE_TYPE_MODEL, doc.GetAllocator()), doc.GetAllocator());
		doc.AddMember(OT_ACTION_PARAM_SERVICE_URL, ot::JsonString(m_modelUrl, doc.GetAllocator()), doc.GetAllocator());

		QByteArray request = QByteArray::fromStdString(doc.toJson());
		request.append('\n');

		std::string response;
		if (!this->sendToClient(request, true, response)) {
			OT_LOG_E("Failed to send model info to client");
			return false;
		}

		m_modelUrlSet = true;
	}

	return true;
}

bool CommunicationHandler::sendFrontendConfigToClient(void) {
	if (!m_frontendUrl.empty() && !m_frontendUrlSet) {
		ot::JsonDocument doc;
		doc.AddMember(OT_ACTION_PARAM_MODEL_ActionName, ot::JsonString(OT_ACTION_CMD_Init, doc.GetAllocator()), doc.GetAllocator());
		doc.AddMember(OT_ACTION_PARAM_SERVICE_NAME, ot::JsonString(OT_INFO_SERVICE_TYPE_UI, doc.GetAllocator()), doc.GetAllocator());
		doc.AddMember(OT_ACTION_PARAM_SERVICE_URL, ot::JsonString(m_frontendUrl, doc.GetAllocator()), doc.GetAllocator());

		QByteArray request = QByteArray::fromStdString(doc.toJson());
		request.append('\n');

		std::string response;
		if (!this->sendToClient(request, true, response)) {
			OT_LOG_E("Failed to send frontend info to client");
			return false;
		}

		m_frontendUrlSet = true;
	}

	return true;
}

bool CommunicationHandler::sendDataBaseConfigToClient(void) {
	if (m_databaseInfo.hasInfoSet() && !m_databaseInfoSet) {
		ot::JsonDocument doc;
		doc.AddMember(OT_ACTION_PARAM_MODEL_ActionName, ot::JsonString(OT_ACTION_CMD_Init, doc.GetAllocator()), doc.GetAllocator());
		doc.AddMember(OT_ACTION_PARAM_SERVICE_NAME, ot::JsonString(OT_INFO_SERVICE_TYPE_DataBase, doc.GetAllocator()), doc.GetAllocator());

		doc.AddMember(OT_ACTION_PARAM_SERVICE_URL, ot::JsonString(m_databaseInfo.getDataBaseUrl(), doc.GetAllocator()), doc.GetAllocator());
		doc.AddMember(OT_ACTION_PARAM_SITE_ID, ot::JsonString(m_databaseInfo.getSiteID(), doc.GetAllocator()), doc.GetAllocator());
		doc.AddMember(OT_ACTION_PARAM_COLLECTION_NAME, ot::JsonString(m_databaseInfo.getCollectionName(), doc.GetAllocator()), doc.GetAllocator());
		doc.AddMember(OT_PARAM_AUTH_USERNAME, ot::JsonString(m_databaseInfo.getUserName(), doc.GetAllocator()), doc.GetAllocator());
		doc.AddMember(OT_PARAM_AUTH_PASSWORD, ot::JsonString(m_databaseInfo.getUserPassword(), doc.GetAllocator()), doc.GetAllocator());

		QByteArray request = QByteArray::fromStdString(doc.toJson());
		request.append('\n');

		std::string response;
		if (!this->sendToClient(request, true, response)) {
			OT_LOG_E("Failed to send database info to client");
			return false;
		}

		m_databaseInfoSet = true;
	}

	return true;
}

bool CommunicationHandler::waitForClient(void) {
	const int tickTime = Timeouts::defaultTickTime;
	int timeout = Timeouts::connectionTimeout / tickTime;
	while (m_clientState != ClientState::Ready || m_isInitializingClient) {
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

void CommunicationHandler::slotProcessMessage(std::string _message) {
	OT_LOG_D("Message from client: \"" + _message + "\"");

	while (_message.substr(0, 7) == "OUTPUT:")
	{
		std::string text = _message.substr(7, _message.length()-1-7); // There is always an additional \n at the end of the message which needs to be removed here

		//size_t msgStart = _message.find(':', 7);
		//std::string length = _message.substr(7, msgStart-7);
		//long msgLength = atol(length.c_str());
		//std::string text = _message.substr(msgStart+1, msgLength);
		//_message = _message.substr(msgStart+1 + msgLength + 1); // There is always an additional \n at the end of the message which needs to be removed here

		Application::instance()->uiComponent()->displayMessage(text);
		m_manager->addLogText(text);

		return;
	}

	//if (_message.empty())
	//{
	//	return;
	//}

	// Check state
	if (m_clientState == ClientState::WaitForPing) {
		ot::JsonDocument responseDoc;
		if (!responseDoc.fromJson(_message)) {
			OT_LOG_E("Invalid client ping response: \"" + _message + "\"");
			m_client->disconnect();
			return;
		}
		ot::ReturnMessage msg;
		msg.setFromJsonObject(responseDoc.GetConstObject());
		//if (msg.getStatus() != ot::ReturnMessage::Ok || msg.getWhat() != OT_ACTION_CMD_Ping) {
		if (msg.getStatus() != ot::ReturnMessage::Ok || msg.getWhat() != OT_ACTION_CMD_Ping) {
			OT_LOG_E("Invalid client ping response: \"" + _message + "\"");
			m_client->disconnect();
			return;
		}

		m_isInitializingClient = true;
		m_clientState = ClientState::Ready;

		if (!this->sendConfigToClient()) {
			m_client->disconnect();
		}
	}
	else if (m_clientState == ClientState::WaitForResponse) {
		m_response = std::move(_message);
		m_clientState = ClientState::ReponseReceived;
	}
	else {
		OT_LOG_W("Client send unexpected message: \"" + _message + "\"");
	}
}

bool CommunicationHandler::sendToClient(const QByteArray& _data, bool _expectResponse, std::string& _response) {
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
	OT_LOG_D("Writing to client: \"" + _data.toStdString() + "\"");

	m_client->write(_data);
	m_client->flush();

	if (!_expectResponse) {
		return true;
	}

	// We know the request was sent successfully, so we set the state to wait for response and wait..
	m_clientState = ClientState::WaitForResponse;

	while (m_clientState == ClientState::WaitForResponse) {
		std::this_thread::sleep_for(std::chrono::microseconds(Timeouts::defaultTickTime));
		this->processNextEvent();
	}

	// Check if the client has received a response or disconnected.
	if (m_clientState == ClientState::ReponseReceived) {
		OT_LOG_D("Client response received: \"" + _response + "\"");

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
	
	// Send ping to ensure stable connection
	m_clientState = ClientState::WaitForPing;

	this->connect(m_client, &QLocalSocket::readyRead, this, &CommunicationHandler::slotMessageReceived);
	this->connect(m_client, &QLocalSocket::disconnected, this, &CommunicationHandler::slotClientDisconnected);

	ot::JsonDocument pingDoc;
	pingDoc.AddMember(OT_ACTION_PARAM_MODEL_ActionName, ot::JsonString(OT_ACTION_CMD_Ping, pingDoc.GetAllocator()), pingDoc.GetAllocator());
	m_client->write(QByteArray::fromStdString(pingDoc.toJson()));
	m_client->flush();
}