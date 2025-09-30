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
#include "OTCore/LogDispatcher.h"
#include "OTCommunication/Msg.h"
#include "OTCommunication/ActionTypes.h"
#include "OTCommunication/ServiceLogNotifier.h"

Session::Session(const std::string& _id, const std::string& _userName, const std::string& _projectName, const std::string& _collectionName, const std::string& _type) :
	m_id(_id), m_userName(_userName), m_projectName(_projectName), m_collectionName(_collectionName), m_type(_type), m_state(Session::NoState), m_healthCheckRunning(false)
{
	OTAssert(!_id.empty(), "Session ID must not be empty");

	OT_LOG_D("New session created { \"ID\": \"" + m_id + "\", \"UserName\": \"" + m_userName + "\", \"ProjectName\": \"" + 
		m_projectName + "\", \"CollectionName\": \"" + m_collectionName + "\", \"SessionType\": \"" + m_type + "\" }");
}

Session::Session(Session&& _other) noexcept :
	m_state(Session::NoState), m_healthCheckRunning(false)
{
	*this = std::move(_other);
}

Session::~Session() {
	this->stopHealthCheck();
	
	if (!m_id.empty()) {
		OT_LOG_D("Session destroyed { \"ID\": \"" + m_id + "\", \"UserName\": \"" + m_userName + "\", \"ProjectName\": \"" +
			m_projectName + "\", \"CollectionName\": \"" + m_collectionName + "\", \"SessionType\": \"" + m_type + "\" }");
	}
}

Session& Session::operator=(Session&& _other) noexcept {
	if (this != &_other) {
		// Stop health check if running
		this->stopHealthCheck();
		if (_other.m_healthCheckRunning) {
			m_healthCheckRunning = true;
		}
		_other.stopHealthCheck();

		// Move
		m_state = _other.m_state;
		
		m_id = std::move(_other.m_id);
		m_userName = std::move(_other.m_userName);
		m_projectName = std::move(_other.m_projectName);
		m_collectionName = std::move(_other.m_collectionName);
		m_type = std::move(_other.m_type);
		
		m_userCredentials = std::move(_other.m_userCredentials);
		m_dbCredentials = std::move(_other.m_dbCredentials);
		m_userCollection = std::move(_other.m_userCollection);
		
		m_services = std::move(_other.m_services);
		m_serviceIdManager = std::move(_other.m_serviceIdManager);
		
		// Clear the other session
		_other.m_id.clear();
		_other.m_services.clear();
		_other.m_state = Session::NoState;

		// Start health check if it was running in the other session object
		if (m_healthCheckRunning) {
			m_healthCheckRunning = false;
			this->startHealthCheck();
		}
	}

	return *this;
}

// ###########################################################################################################################################################################################################################################################################################################################

// Setter / Getter

std::list<std::string> Session::getToolBarTabOrder() {
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

bool Session::hasRequestedServices(ot::serviceID_t _ignoredService) {
	std::lock_guard<std::mutex> lock(m_mutex);

	for (const Service& service : m_services) {
		if (service.getServiceID() != _ignoredService && service.isRequested()) {
			return true;
		}
	}

	return false;
}

bool Session::hasAliveServices() {
	std::lock_guard<std::mutex> lock(m_mutex);

	for (const Service& service : m_services) {
		if (service.isAlive()) {
			return true;
		}
	}

	return false;
}

bool Session::hasShuttingDownServices() {
	std::lock_guard<std::mutex> lock(m_mutex);

	for (const Service& service : m_services) {
		if (service.isShuttingDown()) {
			return true;
		}
	}

	return false;
}

ot::ServiceInitData Session::createServiceInitData(ot::serviceID_t _serviceID) {
	ot::ServiceInitData initData;

	const SessionService& sessionService = SessionService::instance();

	std::lock_guard<std::mutex> lock(m_mutex);
	const Service& service = this->getServiceFromID(_serviceID);
	OTAssert(service.getServiceID() == _serviceID, "Service ID mismatch");

	// Logging
	initData.setLoggerUrl(ot::ServiceLogNotifier::instance().loggingServiceURL());
	initData.setLogFlags(ot::LogDispatcher::instance().getLogFlags());

	// Service
	initData.setServiceName(service.getServiceName());
	initData.setServiceType(service.getServiceType());
	initData.setServiceID(_serviceID);

	// Session
	initData.setAuthorizationUrl(sessionService.getAuthorisationServiceUrl());
	initData.setSessionServiceURL(sessionService.getUrl());
	initData.setSessionID(m_id);
	initData.setSessionType(m_type);

	// DataBase
	initData.setDatabaseUrl(sessionService.getDatabaseUrl());
	initData.setUsername(m_userCredentials.getUserName());
	initData.setPassword(m_userCredentials.getEncryptedPassword());
	initData.setDatabaseUsername(m_dbCredentials.getUserName());
	initData.setDatabasePassword(m_dbCredentials.getEncryptedPassword());
	initData.setUserCollection(m_userCollection);

	return initData;
}

ot::ServiceRunData Session::createServiceRunData(ot::serviceID_t _serviceID) {
	std::lock_guard<std::mutex> lock(m_mutex);
	return this->createServiceRunDataImpl(_serviceID);
}

// ###########################################################################################################################################################################################################################################################################################################################

// Service management

void Session::addToJsonObject(ot::JsonValue& _jsonObject, ot::JsonAllocator& _allocator) {
	std::lock_guard<std::mutex> lock(m_mutex);

	if (m_healthCheckRunning) {
		_jsonObject.AddMember("HealthCheckRunning", true, _allocator);
	}
	else {
		_jsonObject.AddMember("HealthCheckRunning", false, _allocator);
	}

	ot::JsonArray stateArr;
	if (m_state.flagIsSet(Session::ShuttingDown)) {
		stateArr.PushBack(ot::JsonString("ShuttingDown", _allocator), _allocator);
	}
	_jsonObject.AddMember("StateFlags", stateArr, _allocator);

	_jsonObject.AddMember(OT_ACTION_PARAM_SESSION_ID, ot::JsonString(m_id, _allocator), _allocator);
	_jsonObject.AddMember(OT_ACTION_PARAM_USER_NAME, ot::JsonString(m_userName, _allocator), _allocator);
	_jsonObject.AddMember(OT_ACTION_PARAM_PROJECT_NAME, ot::JsonString(m_projectName, _allocator), _allocator);
	_jsonObject.AddMember(OT_ACTION_PARAM_COLLECTION_NAME, ot::JsonString(m_collectionName, _allocator), _allocator);
	_jsonObject.AddMember(OT_ACTION_PARAM_SESSION_TYPE, ot::JsonString(m_type, _allocator), _allocator);

	_jsonObject.AddMember(OT_PARAM_SETTINGS_USERCOLLECTION, ot::JsonString(m_userCollection, _allocator), _allocator);

	ot::JsonArray servicesArr;
	for (const Service& service : m_services) {
		ot::JsonObject serviceObj;
		service.addToJsonObject(serviceObj, _allocator);
		servicesArr.PushBack(serviceObj, _allocator);
	}
	_jsonObject.AddMember("Services", servicesArr, _allocator);
}

Service& Session::addRequestedService(const ot::ServiceBase& _serviceInformation) {
	std::lock_guard<std::mutex> lock(m_mutex);

	Service newService(_serviceInformation, m_id);
	newService.setRequested(true);
	newService.setServiceID(this->generateNextServiceID());
	return this->addService(std::move(newService));
}

void Session::setServiceAlive(ot::serviceID_t _serviceID, bool _notifyOthers) {
	_notifyOthers |= !this->hasRequestedServices(_serviceID);

	std::lock_guard<std::mutex> lock(m_mutex);

	Service& service = this->getServiceFromID(_serviceID);
	
	service.setRequested(false);
	service.setAlive(true);

	if (_notifyOthers) {
		this->broadcastBasicAction(_serviceID, OT_ACTION_CMD_ServiceConnected, false);
	}
}

void Session::setServiceAlive(ot::serviceID_t _serviceID, const std::string& _serviceUrl, bool _notifyOthers) {
	_notifyOthers |= !this->hasRequestedServices(_serviceID);

	std::lock_guard<std::mutex> lock(m_mutex);

	Service& service = this->getServiceFromID(_serviceID);

	service.setRequested(false);
	service.setAlive(true);
	service.setServiceURL(_serviceUrl);

	if (_notifyOthers)  {
		this->broadcastBasicAction(_serviceID, OT_ACTION_CMD_ServiceConnected, false);
	}
}

void Session::showService(ot::serviceID_t _serviceID) {
	std::thread t(&Session::showWorker, this, _serviceID);
	t.detach();
}

ot::ServiceBase Session::getServiceInfo(ot::serviceID_t _serviceID) {
	std::lock_guard<std::mutex> lock(m_mutex);
	return this->getServiceFromID(_serviceID);
}

void Session::setServiceShutdownCompleted(ot::serviceID_t _serviceID) {
	std::lock_guard<std::mutex> lock(m_mutex);

	for (auto it = m_services.begin(); it != m_services.end(); it++) {
		if (it->getServiceID() == _serviceID) {
			m_services.erase(it);
			break;
		}
	}
}

void Session::serviceDisconnected(ot::serviceID_t _serviceID, bool _notifyOthers) {
	std::lock_guard<std::mutex> lock(m_mutex);

	for (auto it = m_services.begin(); it != m_services.end(); ++it) {
		if (it->getServiceID() == _serviceID) {
			// Update state
			it->setAlive(false);
			it->setShuttingDown(false);

			if (_notifyOthers) {
				this->broadcastBasicAction(_serviceID, OT_ACTION_CMD_ServiceDisconnected, false);
			}

			m_services.erase(it);

			OT_LOG_D("Service was removed from session { \"ServiceID\": " + std::to_string(_serviceID) + ", \"SessionID\": \"" + m_id + "\" }");

			return;
		}
	}
}

void Session::addAliveServicesToJsonArray(ot::JsonArray& _array, ot::JsonAllocator& _allocator) {
	std::lock_guard<std::mutex> lock(m_mutex);

	for (const Service& service : m_services) {
		if (service.isAlive() && !service.isHidden()) {
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

void Session::sendRunCommand() {
	std::thread t(&Session::runWorker, this);
	t.detach();
}

// ###########################################################################################################################################################################################################################################################################################################################

// Session management

void Session::prepareSessionForShutdown(ot::serviceID_t _requestingService) {
	this->stopHealthCheck();

	std::lock_guard<std::mutex> lock(m_mutex);
	m_state.setFlag(Session::ShuttingDown, true);

	for (auto it = m_services.begin(); it != m_services.end(); it++) {
		it->setAlive(false);
		it->setRunning(false);
		it->setRequested(false);
		it->setShuttingDown();

		if (it->getServiceID() == _requestingService && !it->getWebsocketUrl().empty()) {
			// Remove requesting service except when it has a relay service running
			it = m_services.erase(it);
		}
	}

	
}

void Session::shutdownSession(ot::serviceID_t _senderServiceID, bool _emergencyShutdown, ot::PortManager& _debugPortManager) {
	std::lock_guard<std::mutex> lock(m_mutex);

	// In case of regular shutdown send pre shutdown command before shutting down the session.
	if (!_emergencyShutdown) {
		this->broadcastBasicAction(_senderServiceID, OT_ACTION_CMD_ServicePreShutdown, true);
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
	this->broadcast(_senderServiceID, doc.toJson(), false, true);

	// Remove debug services since the shutdown won't be detected by the LDS
	for (auto it = m_services.begin(); it != m_services.end(); ) {
		if (it->isDebug()) {
			// Free used debug ports
			for (ot::port_t port : it->getPortNumbers()) {
				_debugPortManager.freePort(port);
			}

			// Remove service from session
			it = m_services.erase(it);
		}
		else {
			it++;
		}
	}
}

void Session::sendBroadcast(ot::serviceID_t _senderServiceID, const std::string& _message) {
	std::lock_guard<std::mutex> lock(m_mutex);
	this->broadcast(_senderServiceID, _message, false, false);
}

void Session::removeFailedService(ot::serviceID_t _failedServiceID) {
	std::lock_guard<std::mutex> lock(m_mutex);

	// Perform health check
	for (auto it = m_services.begin(); it != m_services.end(); it++) {
		if (it->getServiceID() == _failedServiceID) {
			it->setAlive(false);
			it->setShuttingDown(false);
			it->setRequested(false);
			m_services.erase(it);

			break;
		}
	}
}

// ###########################################################################################################################################################################################################################################################################################################################

// Private: Messaging

void Session::broadcastBasicAction(ot::serviceID_t _senderServiceID, const std::string& _action, bool _forceSend) {
	ot::JsonDocument doc;
	this->prepareBroadcastDocument(doc, _action, _senderServiceID);
	this->broadcast(_senderServiceID, doc.toJson(), false, _forceSend);
}

void Session::broadcast(ot::serviceID_t _senderServiceID, const std::string& _message, bool _async, bool _forceSend) {
	if (_async) {
		std::thread t(&Session::broadcastWorker, this, _senderServiceID, _message, _forceSend);
		t.detach();
	}
	else {
		this->broadcastImpl(_senderServiceID, _message, _forceSend);
	}
}

// ###########################################################################################################################################################################################################################################################################################################################

// Private: Helper

Service& Session::addService(Service&& _service) {
	if (this->hasService(_service.getServiceID())) {
		OT_LOG_E("Service already registered { \"ServiceID\": " + std::to_string(_service.getServiceID()) + ", \"SessionID\": \"" + m_id + "\" }");
		throw ot::Exception::ObjectAlreadyExists("Service already registered { \"ServiceID\": " + std::to_string(_service.getServiceID()) + ", \"SessionID\": \"" + m_id + "\" }");
	}

	m_services.push_back(std::move(_service));
	return m_services.back();
}

bool Session::hasService(ot::serviceID_t _serviceID) const {
	for (const Service& service : m_services) {
		if (service.getServiceID() == _serviceID) {
			return true;
		}
	}

	return false;
}

Service& Session::getServiceFromID(ot::serviceID_t _serviceID) {
	for (Service& service : m_services) {
		if (service.getServiceID() == _serviceID) {
			return service;
		}
	}

	OT_LOG_D("Service not found { \"ServiceID\": " + std::to_string(_serviceID) + " }");
	throw ot::Exception::ObjectNotFound("Service not found { \"ServiceID\": " + std::to_string(_serviceID) + " }");
}

const Service& Session::getServiceFromID(ot::serviceID_t _serviceID) const {
	for (const Service& service : m_services) {
		if (service.getServiceID() == _serviceID) {
			return service;
		}
	}

	OT_LOG_D("Service not found { \"ServiceID\": " + std::to_string(_serviceID) + " }");
	throw ot::Exception::ObjectNotFound("Service not found { \"ServiceID\": " + std::to_string(_serviceID) + " }");
}

void Session::broadcastImpl(ot::serviceID_t _senderServiceID, const std::string& _message, bool _forceSend) {
	std::string lssUrl = SessionService::instance().getUrl();

	for (const Service& service : m_services) {
		if (!service.getServiceURL().empty() && (_forceSend || (service.isRunning() && !service.isShuttingDown())) && service.getServiceID() != _senderServiceID) {
			std::string response;
			if (!ot::msg::send(lssUrl, service.getServiceURL(), ot::EXECUTE, _message, response, ot::msg::defaultTimeout, ot::msg::DefaultFlagsNoExit)) {
				OT_LOG_E("Failed to send broadcast message to service { \"id\": \"" + std::to_string(service.getServiceID()) + "\", \"url\": \"" + service.getServiceURL() + "\" }");
			}
		}
	}
}

void Session::prepareBroadcastDocument(ot::JsonDocument& _doc, const std::string& _action, ot::serviceID_t _senderService) {
	_doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(_action, _doc.GetAllocator()), _doc.GetAllocator());
	
	if (_senderService != ot::invalidServiceID) {
		if (this->hasService(_senderService)) {
			Service& service = this->getServiceFromID(_senderService);
			_doc.AddMember(OT_ACTION_PARAM_SERVICE_ID, service.getServiceID(), _doc.GetAllocator());
			_doc.AddMember(OT_ACTION_PARAM_SERVICE_URL, ot::JsonString(service.getServiceURL(), _doc.GetAllocator()), _doc.GetAllocator());
			_doc.AddMember(OT_ACTION_PARAM_SERVICE_NAME, ot::JsonString(service.getServiceName(), _doc.GetAllocator()), _doc.GetAllocator());
			_doc.AddMember(OT_ACTION_PARAM_SERVICE_TYPE, ot::JsonString(service.getServiceType(), _doc.GetAllocator()), _doc.GetAllocator());
		}
		else {
			// This may happen when the service was deregistered already
			_doc.AddMember(OT_ACTION_PARAM_SERVICE_ID, _senderService, _doc.GetAllocator());
		}
	}

	_doc.AddMember(OT_ACTION_PARAM_SESSION_ID, ot::JsonString(m_id, _doc.GetAllocator()), _doc.GetAllocator());
}

ot::ServiceRunData Session::createServiceRunDataImpl(ot::serviceID_t _serviceID) {
	ot::ServiceRunData runData;

	// Services
	for (const Service& service : m_services) {
		if (service.getServiceID() != _serviceID && service.isAlive() && !service.isHidden()) {
			runData.addService(service);
		}
	}

	return runData;
}

// ###########################################################################################################################################################################################################################################################################################################################

// Private: Worker

void Session::broadcastWorker(ot::serviceID_t _senderServiceID, std::string _message, bool _forceSend) {
	std::lock_guard<std::mutex> lock(m_mutex);
	this->broadcastImpl(_senderServiceID, _message, _forceSend);
}

void Session::healthCheckWorker() {
	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_Ping, doc.GetAllocator()), doc.GetAllocator());
	std::string msg = doc.toJson();

	std::string lssUrl = SessionService::instance().getUrl();
	bool failureDetected = false;
	ot::serviceID_t failedServiceID = ot::invalidServiceID;

	while (m_healthCheckRunning && !failureDetected) {
		{
			std::lock_guard<std::mutex> lock(m_mutex);

			// Perform health check
			for (auto it = m_services.begin(); it != m_services.end(); it++) {
				if (it->isAlive() && !it->isShuttingDown()) {
					std::string response;
					if (!ot::msg::send(lssUrl, it->getServiceURL(), ot::EXECUTE, msg, response, ot::msg::defaultTimeout, ot::msg::DefaultFlagsNoExit)) {
						failureDetected = true;
						failedServiceID = it->getServiceID();
						break;
					}
					else if (response != OT_ACTION_CMD_Ping) {
						failureDetected = true;
						failedServiceID = it->getServiceID();
						break;
					}
				}
			}
		} // lock guard scope

		if (failureDetected) {
			std::thread notificationThread(&Session::healthCheckFailedNotifier, m_id, failedServiceID);
			notificationThread.detach();
		}
		else {
			// Sleep for a while before the next health check
			for(int i = 0 ; i < 3000 && m_healthCheckRunning; i++) {
				std::this_thread::sleep_for(std::chrono::milliseconds(10));
			}
		}
	}
}

void Session::runWorker() {
	std::string lssUrl = SessionService::instance().getUrl();

	std::lock_guard<std::mutex> lock(m_mutex);

	if (m_state & Session::ShuttingDown) {
		OT_LOG_W("Session is shutting down. Run command will not be sent { \"SessionID\": \"" + m_id + "\" }");
		return;
	}

	// Send run command to all services that are alive but not yet running
	for (Service& service : m_services) {
		if (service.isAlive() && !service.isRunning() && !service.isShuttingDown()) {
			ot::JsonDocument runDoc;
			runDoc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_Run, runDoc.GetAllocator()), runDoc.GetAllocator());
			runDoc.AddMember(OT_ACTION_PARAM_RunData, ot::JsonObject(this->createServiceRunDataImpl(service.getServiceID()), runDoc.GetAllocator()), runDoc.GetAllocator());

			std::string response;
			if (!ot::msg::send(lssUrl, service.getServiceURL(), ot::EXECUTE, runDoc.toJson(), response, ot::msg::defaultTimeout, ot::msg::DefaultFlagsNoExit)) {
				OT_LOG_E("Failed to send run command to service { \"id\": \"" + std::to_string(service.getServiceID()) + "\", \"url\": \"" + service.getServiceURL() + "\" }");
			}
			else {
				service.setRunning();
			}
		}
	}
}

void Session::showWorker(ot::serviceID_t _visibleService) {
	try {
		std::lock_guard<std::mutex> lock(m_mutex);
		if (m_state & Session::ShuttingDown) {
			OT_LOG_W("Session is shutting down. Show command will not be processed { \"SessionID\": \"" + m_id + "\" }");
			return;
		}

		Service& service = this->getServiceFromID(_visibleService);
		if (!service.isHidden()) {
			OT_LOG_W("Service is not hidden { \"ServiceID\": " + std::to_string(service.getServiceID()) + ", \"ServiceName\": \"" + service.getServiceName() + "\", \"SessionID\": \"" + m_id + "\"");
			return;
		}

		service.setHidden(false);

		this->broadcastBasicAction(_visibleService, OT_ACTION_CMD_ServiceConnected, false);
	}
	catch (const std::exception& _e) {
		OT_LOG_E(_e.what());
		this->healthCheckFailedNotifier(m_id, _visibleService);
	}
}

void Session::healthCheckFailedNotifier(std::string _sessionID, ot::serviceID_t _failedServiceID) {
	SessionService::instance().serviceFailure(_sessionID, _failedServiceID);
}
