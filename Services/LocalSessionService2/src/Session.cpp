//! @file Session.cpp
//! @authors Alexander Kuester (alexk95)
//! @date June 2025
// ###########################################################################################################################################################################################################################################################################################################################

// LSS header
#include "Session.h"
#include "Service.h"
#include "SessionService.h"

// OpenTwin header
#include "OTSystem/PortManager.h"
#include "OTCore/JSON.h"
#include "OTCore/Logger.h"
#include "OTCommunication/Msg.h"
#include "OTCommunication/ActionTypes.h"

Session::Session(const std::string& _id, const std::string& _userName, const std::string& _projectName, const std::string& _collectionName, const std::string& _type) :
	m_id(_id), m_userName(_userName), m_projectName(_projectName), m_collectionName(_collectionName), m_type(_type), m_state(Session::NoState), m_healthCheckRunning(false)
{
	OTAssert(!_id.empty(), "Session ID must not be empty");

	OT_LOG_D("New session created { \"ID\": \"" + m_id + "\", \"UserName\": \"" + m_userName + "\", \"ProjectName\": \"" + 
		m_projectName + "\", \"CollectionName\": \"" + m_collectionName + "\", \"SessionType\": \"" + m_type + "\" }");
}

Session::~Session() {
	this->stopHealthCheck();
	
	if (!m_id.empty()) {
		OT_LOG_D("Session destroyed { \"ID\": \"" + m_id + "\", \"UserName\": \"" + m_userName + "\", \"ProjectName\": \"" +
			m_projectName + "\", \"CollectionName\": \"" + m_collectionName + "\", \"SessionType\": \"" + m_type + "\" }");
	}
}

// ###########################################################################################################################################################################################################################################################################################################################

// Setter / Getter

std::list<std::string> Session::getToolBarTabOrder(void) {
	std::list<std::string> tabOrder;

	tabOrder.push_back("File");
	tabOrder.push_back("View");
	tabOrder.push_back("Model");
	tabOrder.push_back("Modeling");
	tabOrder.push_back("Mesh");
	tabOrder.push_back("FIT-TD");
	tabOrder.push_back("PHREEC");
	tabOrder.push_back("GetDP");
	tabOrder.push_back("ElmerFEM");
	tabOrder.push_back("Post Processing");
	
	return tabOrder;
}

// ###########################################################################################################################################################################################################################################################################################################################

// Service management

Service& Session::addService(Service&& _service) {
	if (this->getServiceFromID(_service.getServiceID())) {
		OT_LOG_E("Service already registered { \"ServiceID\": " + std::to_string(_service.getServiceID()) + ", \"SessionID\": \"" + m_id + "\" }");
		throw ot::Exception::ObjectAlreadyExists("Service already registered { \"ServiceID\": " + std::to_string(_service.getServiceID()) + ", \"SessionID\": \"" + m_id + "\" }");
	}

	m_services.push_back(std::move(_service));
	return m_services.back();
}

void Session::serviceDisconnected(ot::serviceID_t _serviceID, bool _notifyOthers) {
	for (auto it = m_services.begin(); it != m_services.end(); ++it) {
		if (it->getServiceID() == _serviceID) {
			// Update state
			it->setAlive(false);
			it->setShuttingDown(false);

			if (_notifyOthers) {
				this->broadcastBasicAction(_serviceID, OT_ACTION_CMD_ServiceDisconnected);
			}

			m_services.erase(it);

			OT_LOG_D("Service was removed from session { \"ServiceID\": " + std::to_string(_serviceID) + ", \"SessionID\": \"" + m_id + "\" }");

			return;
		}
	}
}

std::optional<Service&> Session::getServiceFromID(ot::serviceID_t _serviceID) {
	for (Service& service : m_services) {
		if (service.getServiceID() == _serviceID) {
			return std::optional<Service&>(service);
		}
	}

	OT_LOG_D("Service not found { \"ServiceID\": " + std::to_string(_serviceID) + " }");
	return std::optional<Service&>();
}

std::optional<const Service&> Session::getServiceFromID(ot::serviceID_t _serviceID) const {
	for (const Service& service : m_services) {
		if (service.getServiceID() == _serviceID) {
			return std::optional<const Service&>(service);
		}
	}

	OT_LOG_D("Service not found { \"ServiceID\": " + std::to_string(_serviceID) + " }");
	return std::optional<const Service&>();
}

std::optional<Service&> Session::getServiceFromURL(const std::string& _serviceURL) {
	for (Service& service : m_services) {
		if (service.getServiceURL() == _serviceURL) {
			return std::optional<Service&>(service);
		}
	}

	OT_LOG_D("Service not found { \"ServiceURL\": \"" + _serviceURL + "\" }");
	return std::optional<Service&>();
}

std::optional<const Service&> Session::getServiceFromURL(const std::string& _serviceURL) const {
	for (const Service& service : m_services) {
		if (service.getServiceURL() == _serviceURL) {
			return std::optional<const Service&>(service);
		}
	}

	OT_LOG_D("Service not found { \"ServiceURL\": \"" + _serviceURL + "\" }");
	return std::optional<const Service&>();
}

void Session::addAliveServicesToJsonArray(ot::JsonArray& _array, ot::JsonAllocator& _allocator) const {
	for (const Service& service : m_services) {
		if (service.isAlive()) {
			ot::JsonObject obj;
			service.addToJsonObject(obj, _allocator);
			_array.PushBack(obj, _allocator);
		}
	}
}

void Session::startHealthCheck() {
	if (m_healthCheckRunning) {
		OT_LOG_W("Health check already running { \"SessionID\": \"" + m_id + "\" }");
		return;
	}

	m_healthCheckRunning = true;
	m_healthCheckThread = std::thread(&Session::healthCheckWorker, this);
}

void Session::stopHealthCheck() {
	if (!m_healthCheckRunning) {
		OT_LOG_D("Health check not running { \"SessionID\": \"" + m_id + "\" }");
		return;
	}

	m_healthCheckRunning = false;
	if (m_healthCheckThread.joinable()) {
		m_healthCheckThread.join();
	}

	OT_LOG_D("Health check stopped { \"SessionID\": \"" + m_id + "\" }");
}

// ###########################################################################################################################################################################################################################################################################################################################

// Session management

void Session::prepareSessionForShutdown() {
	m_state.setFlag(Session::ShuttingDown, true);

	this->stopHealthCheck();
}

void Session::shutdownSession(ot::serviceID_t _senderServiceID, bool _emergencyShutdown) {
	// In case of regular shutdown send pre shutdown command before shutting down the session.
	if (!_emergencyShutdown) {
		this->broadcastBasicAction(_senderServiceID, OT_ACTION_CMD_ServicePreShutdown);
	}

	// Prepare the shutdown document
	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_PARAM_SESSION_EXISTS, false, doc.GetAllocator());

	if (_emergencyShutdown) {
		doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_ServiceEmergencyShutdown, doc.GetAllocator()), doc.GetAllocator());
	}
	else {
		doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_ServiceShutdown, doc.GetAllocator()), doc.GetAllocator());
	}
	
	// Broadcast the shutdown document
	this->broadcast(_senderServiceID, doc.toJson(), false);

	// Flag all services as shutting down
	for (Service& service : m_services) {
		service.setAlive(false);
		service.setShuttingDown(true);
	}

	// Remove the service that initiated the shutdown
	for (auto it = m_services.begin(); it != m_services.end(); it++) {
		if (it->getServiceID() == _senderServiceID) {
			m_services.erase(it);
			break;
		}
	}
}

// ###########################################################################################################################################################################################################################################################################################################################

// Messaging

void Session::broadcastMessage(ot::serviceID_t _senderServiceID, const std::string& _message) {
	ot::JsonDocument doc;
	this->prepareBroadcastDocument(doc, OT_ACTION_CMD_Message, _senderServiceID);
	doc.AddMember(OT_ACTION_PARAM_MESSAGE, ot::JsonString(_message, doc.GetAllocator()), doc.GetAllocator());
	this->broadcast(_senderServiceID, doc.toJson(), false);
}

void Session::broadcastBasicAction(ot::serviceID_t _senderServiceID, const std::string& _action) {
	ot::JsonDocument doc;
	this->prepareBroadcastDocument(doc, _action, _senderServiceID);
	this->broadcast(_senderServiceID, doc.toJson(), false);
}

void Session::broadcast(ot::serviceID_t _senderServiceID, const std::string& _message, bool _async) {
	if (_async) {
		std::thread t(&Session::broadcastWorker, this, _senderServiceID, _message);
		t.detach();
	}
	else {
		this->broadcastImpl(_senderServiceID, _message);
	}
}

// ###########################################################################################################################################################################################################################################################################################################################

// Private: Helper

void Session::broadcastImpl(ot::serviceID_t _senderServiceID, const std::string& _message) {
	std::lock_guard<std::mutex> lock(m_mutex);

	std::string lssUrl = SessionService::instance().getUrl();

	for (const Service& service : m_services) {
		if (service.isAlive() && !service.isShuttingDown() && service.getServiceID() != _senderServiceID) {
			std::string response;
			if (!ot::msg::send(lssUrl, service.getServiceURL(), ot::EXECUTE, _message, response, ot::msg::defaultTimeout, ot::msg::DefaultFlagsNoExit)) {
				OT_LOG_E("Failed to send broadcast message to service { \"id\": \"" + std::to_string(service.getServiceID()) + "\", \"url\": \"" + service.getServiceURL() + "\" }");
			}
		}
	}
}

void Session::prepareBroadcastDocument(ot::JsonDocument& _doc, const std::string& _action, ot::serviceID_t _senderService) const {
	_doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(_action, _doc.GetAllocator()), _doc.GetAllocator());
	
	if (_senderService != ot::invalidServiceID) {
		auto info = this->getServiceFromID(_senderService);
		if (info.has_value()) {
			_doc.AddMember(OT_ACTION_PARAM_SERVICE_ID, info->getId(), _doc.GetAllocator());
			_doc.AddMember(OT_ACTION_PARAM_SERVICE_URL, ot::JsonString(info->getUrl(), _doc.GetAllocator()), _doc.GetAllocator());
			_doc.AddMember(OT_ACTION_PARAM_SERVICE_NAME, ot::JsonString(info->getName(), _doc.GetAllocator()), _doc.GetAllocator());
			_doc.AddMember(OT_ACTION_PARAM_SERVICE_TYPE, ot::JsonString(info->getType(), _doc.GetAllocator()), _doc.GetAllocator());
		}
		else {
			OT_LOG_EAS("Failed to find service { \"id\": \"" + std::to_string(_senderService) + "\", \"SessionID\": \"" + m_id + "\" }");
			_doc.AddMember(OT_ACTION_PARAM_SERVICE_ID, _senderService, _doc.GetAllocator());
		}
	}
}

void Session::prepareServiceFailure(Service& _failedService) {
	_failedService.setAlive(false);
	_failedService.setShuttingDown(false);
	_failedService.setRequested(false);

	SessionService::instance().serviceFailure(m_id, _failedService.getServiceID());
}

// ###########################################################################################################################################################################################################################################################################################################################

// Private: Worker

void Session::broadcastWorker(ot::serviceID_t _senderServiceID, std::string _message) {
	this->broadcastImpl(_senderServiceID, _message);
}

void Session::healthCheckWorker() {
	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_Ping, doc.GetAllocator()), doc.GetAllocator());
	std::string msg = doc.toJson();

	std::string lssUrl = SessionService::instance().getUrl();
	bool failureDetected = false;

	while (m_healthCheckRunning && !failureDetected) {
		{
			std::lock_guard<std::mutex> lock(m_mutex);

			// Perform health check
			for (Service& service : m_services) {
				if (service.isAlive() && !service.isShuttingDown()) {
					std::string response;
					if (!ot::msg::send(lssUrl, service.getServiceURL(), ot::EXECUTE, msg, response, ot::msg::defaultTimeout, ot::msg::DefaultFlagsNoExit)) {
						failureDetected = true;
						this->prepareServiceFailure(service);
						break;
					}
					else if (response != OT_ACTION_CMD_Ping) {
						failureDetected = true;
						this->prepareServiceFailure(service);
						break;
					}
				}
			}
		}

		if (!failureDetected) {
			// Sleep for a while before the next health check
			std::this_thread::sleep_for(std::chrono::seconds(10));
		}
	}
}
