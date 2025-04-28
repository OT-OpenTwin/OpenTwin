//! @file Session.cpp
//! @authors Alexander Kuester (alexk95)
//! @date December 2020
// ###########################################################################################################################################################################################################################################################################################################################

// Service header
#include "Session.h"
#include "Service.h"
#include "SessionService.h"

// OpenTwin header
#include "OTSystem/Exception.h"
#include "OTCore/JSON.h"
#include "OTCore/Logger.h"
#include "OTCommunication/Msg.h"
#include "OTCommunication/ActionTypes.h"

#define OT_BROADCASTACTION_ADDSENDERDATA(___doc, ___sender) ___doc.AddMember(OT_ACTION_PARAM_SERVICE_ID, ___sender->getId(), ___doc.GetAllocator()); ___doc.AddMember(OT_ACTION_PARAM_SERVICE_URL, ot::JsonString(___sender->getUrl(), ___doc.GetAllocator()), ___doc.GetAllocator()); ___doc.AddMember(OT_ACTION_PARAM_SERVICE_NAME, ot::JsonString(___sender->getName(), ___doc.GetAllocator()), ___doc.GetAllocator()); ___doc.AddMember(OT_ACTION_PARAM_SERVICE_TYPE, ot::JsonString(___sender->getType(), ___doc.GetAllocator()), ___doc.GetAllocator());

Session::Session(const std::string& _id, const std::string& _userName, const std::string& _projectName, const std::string& _collectionName, const std::string& _type) :
	m_id(_id), m_userName(_userName), m_projectName(_projectName), m_collectionName(_collectionName), m_type(_type)
{
	OT_LOG_D("New session created with (ID = \"" + m_id + "\", User.Name = \"" + m_userName + "\", Project.Name = \"" + 
		m_projectName + "\", Collection.Name = \"" + m_collectionName + "\", Session.Type = \"" + m_type + "\")");
}

Session::Session(Session&& _other) noexcept :
	m_id(std::move(_other.m_id)),
	m_userName(std::move(_other.m_userName)),
	m_projectName(std::move(_other.m_projectName)),
	m_collectionName(std::move(_other.m_collectionName)),
	m_type(std::move(_other.m_type)),

	m_userCredentials(std::move(_other.m_userCredentials)),
	m_dbCredentials(std::move(_other.m_dbCredentials)),
	m_userCollection(std::move(_other.m_userCollection)),

	m_serviceMap(std::move(_other.m_serviceMap)),
	m_serviceIdManager(std::move(_other.m_serviceIdManager)),
	m_requestedServices(std::move(_other.m_requestedServices)),
	m_debugPorts(std::move(_other.m_debugPorts))
{
	// Clean other
	_other.m_serviceMap.clear();
	_other.m_requestedServices.clear();
	_other.m_debugPorts.clear();
}

Session::~Session() {
	for (auto p : m_debugPorts) {
		ot::PortManager::instance().setPortNotInUse(p);
	}
}

Session& Session::operator=(Session&& _other) noexcept {
	if (this != &_other) {
		m_id = std::move(_other.m_id);
		m_userName = std::move(_other.m_userName);
		m_projectName = std::move(_other.m_projectName);
		m_collectionName = std::move(_other.m_collectionName);
		m_type = std::move(_other.m_type);

		m_userCredentials = std::move(_other.m_userCredentials);
		m_dbCredentials = std::move(_other.m_dbCredentials);
		m_userCollection = std::move(_other.m_userCollection);

		m_serviceMap = std::move(_other.m_serviceMap);
		m_serviceIdManager = std::move(_other.m_serviceIdManager);
		m_requestedServices = std::move(_other.m_requestedServices);
		m_debugPorts = std::move(_other.m_debugPorts);

		// Clean other
		_other.m_serviceMap.clear();
		_other.m_requestedServices.clear();
		_other.m_debugPorts.clear();
	}

	return *this;
}

// ###########################################################################################################################################################################################################################################################################################################################

// Management

void Session::addRequestedService(const std::string& _serviceName, const std::string& _serviceType) {
	// Check for duplicate
	for (size_t i = 0; i < m_requestedServices.size(); i++) {
		if (m_requestedServices[i].first == _serviceName && m_requestedServices[i].second == _serviceType) {
			OT_LOG_W("Requested service: " + _serviceName + " (type = " + _serviceType + ") is already added as requested at the session (id = " + m_id + ")");
			return;
		}
	}

	// Check if the requested service is already alive
	for (const auto& s : m_serviceMap) {
		if (s.second.getServiceName() == _serviceName && s.second.getServiceType() == _serviceType) {
			OT_LOG_W("Requested service: " + _serviceName + " (type = " + _serviceType + ") is already alive at session (id = " + m_id + ")");
			return;
		}
	}

	m_requestedServices.push_back(std::pair<std::string, std::string>(_serviceName, _serviceType));
	OT_LOG_D("Added requested service: " + _serviceName + " (type = " + _serviceType + ") to session (id = " + m_id + ")");
}

void Session::removeRequestedService(const std::string& _serviceName, const std::string& _serviceType) {
	for (size_t i = 0; i < m_requestedServices.size(); i++) {
		if (m_requestedServices[i].first == _serviceName && m_requestedServices[i].second == _serviceType) {
			OT_LOG_D("Removed requested service: " + _serviceName + " (type = " + _serviceType + ") from session (id = " + m_id + ")");
			m_requestedServices.erase(m_requestedServices.begin() + i);
			return;
		}
	}
}

Service& Session::registerService(const std::string& _serviceUrl, const std::string& _serviceName, const std::string& _serviceType, ot::serviceID_t _serviceID) {
	Service newService(_serviceUrl, _serviceName, _serviceID, _serviceType, this->getId());
	return this->registerService(std::move(newService));
}

Service& Session::registerService(Service&& _service) {
	auto itm = m_serviceMap.find(_service.getServiceId());

	if (itm != m_serviceMap.end()) {
		std::string errorMessage("A service with the ID \"");
		errorMessage.append(std::to_string(_service.getServiceId()));
		errorMessage.append("\" was already registered");
		throw std::out_of_range(errorMessage.c_str());
	}

	m_serviceMap.insert_or_assign(_service.getServiceId(), _service);

	OT_LOG_D("The service (name = \"" + _service.getServiceName() +
		"\"; type = \"" + _service.getServiceType() + 
		"\"; id = \"" + std::to_string(_service.getServiceId()) +
		"\") was registered at the session (id = \"" + m_id + "\")"
	);

	return _service;
}

void Session::removeService(ot::serviceID_t _serviceID, bool _notifyOthers) {
	auto itm = m_serviceMap.find(_serviceID);
	
	if (itm == m_serviceMap.end()) {
		OT_LOG_EAS("Service (" + std::to_string(_serviceID) + ") not found");
		return;
	}

	std::string url = itm->second.getServiceUrl();

	m_serviceMap.erase(_serviceID);
	m_serviceIdManager.freeID(_serviceID);

	if (_notifyOthers) {
		this->broadcastAction(url, OT_ACTION_CMD_ServiceDisconnected);
	}
}

void Session::addServiceListToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const {
	ot::JsonArray arr;
	for (const auto& s : m_serviceMap) {
		if (s.second.getIsVisible()) {
			ot::JsonObject obj;
			s.second.addToJsonObject(obj, _allocator);
			arr.PushBack(obj, _allocator);
		}
	}
	_object.AddMember(OT_ACTION_PARAM_SESSION_SERVICES, arr, _allocator);
}

void Session::servicesInformation(ot::JsonArray& _servicesInfo, ot::JsonAllocator& _allocator) const
{
	for (auto& s : m_serviceMap)
	{
		ot::JsonValue infoDoc;
		infoDoc.SetObject();

		std::string serviceType = s.second.getServiceType();
		if (serviceType == OT_INFO_SERVICE_TYPE_UI) {
			serviceType = OT_INFO_SERVICE_TYPE_RelayService;
		}

		infoDoc.AddMember(OT_ACTION_PARAM_SERVICE_TYPE, ot::JsonString(serviceType, _allocator), _allocator);
		infoDoc.AddMember(OT_ACTION_PARAM_SERVICE_URL, ot::JsonString(s.second.getServiceUrl(), _allocator), _allocator);

		_servicesInfo.PushBack(infoDoc, _allocator);
	}
}

void Session::broadcastMessage(const std::string& _senderServiceUrl, const std::string& _message, bool _async) {
	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_Message, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_MESSAGE, ot::JsonString(_message, doc.GetAllocator()), doc.GetAllocator());

	if (!_senderServiceUrl.empty())
	{
		Service& service = this->findServiceFromURL(_senderServiceUrl);

		doc.AddMember(OT_ACTION_PARAM_SERVICE_ID, service.getServiceId(), doc.GetAllocator());
		doc.AddMember(OT_ACTION_PARAM_SERVICE_URL, ot::JsonString(service.getServiceUrl(), doc.GetAllocator()), doc.GetAllocator());
		doc.AddMember(OT_ACTION_PARAM_SERVICE_NAME, ot::JsonString(service.getServiceName(), doc.GetAllocator()), doc.GetAllocator());
		doc.AddMember(OT_ACTION_PARAM_SERVICE_TYPE, ot::JsonString(service.getServiceType(), doc.GetAllocator()), doc.GetAllocator());
	}

	broadcast(_senderServiceUrl, doc, false, _async);
}

void Session::broadcastAction(const std::string& _senderServiceUrl, const std::string& _command, bool _async) {
	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(_command, doc.GetAllocator()), doc.GetAllocator());
	
	if (!_senderServiceUrl.empty()) {
		Service& service = this->findServiceFromURL(_senderServiceUrl);

		doc.AddMember(OT_ACTION_PARAM_SERVICE_ID, service.getServiceId(), doc.GetAllocator());
		doc.AddMember(OT_ACTION_PARAM_SERVICE_URL, ot::JsonString(service.getServiceUrl(), doc.GetAllocator()), doc.GetAllocator());
		doc.AddMember(OT_ACTION_PARAM_SERVICE_NAME, ot::JsonString(service.getServiceName(), doc.GetAllocator()), doc.GetAllocator());
		doc.AddMember(OT_ACTION_PARAM_SERVICE_TYPE, ot::JsonString(service.getServiceType(), doc.GetAllocator()), doc.GetAllocator());
	}

	this->broadcast(_senderServiceUrl, doc, false, _async);
}

void Session::broadcast(const std::string& _senderServiceUrl, const ot::JsonDocument& _message, bool _shutdown, bool _async) {
	std::string msg = _message.toJson();
	OT_LOG_D("Sending broadcast message in session (id = \"" + m_id + "\"): " + msg);
	
	for (const auto& itm : m_serviceMap) {
		if (itm.second.getReceiveBroadcastMessages() && itm.second.getIsVisible()) {
			std::string receiverUrl = itm.second.getServiceUrl();

			if (_senderServiceUrl != receiverUrl) {
				// Send the message to the current reciever
				std::string response;
				ot::msg::RequestFlags reqFlags = ot::msg::DefaultFlagsNoExit;
				if (_async) {
					ot::msg::sendAsync(_senderServiceUrl, receiverUrl, ot::QUEUE, msg, ot::msg::defaultTimeout, reqFlags);
				} else {
					if (_shutdown) {
						reqFlags |= ot::msg::IsShutdownMessage;
					}

					if (!ot::msg::send(_senderServiceUrl, receiverUrl, ot::QUEUE, msg, response, ot::msg::defaultTimeout, reqFlags)) {
						OT_LOG_E("Failed to send broadcast message to: " + receiverUrl);
					}
				}
			}
		}
	}
}

void Session::shutdownByService(const std::string& _serviceUrl) {
	broadcastAction(_serviceUrl, OT_ACTION_CMD_ServicePreShutdown);

	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_ServiceShutdown, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_SESSION_EXISTS, false, doc.GetAllocator());
	
	broadcast(_serviceUrl, doc, true);

	auto itm = m_serviceMap.begin();
	while (itm != m_serviceMap.end()) {
		if (itm->second.getServiceUrl() != _serviceUrl) {
			SessionService::instance().serviceClosing(itm->second, false, false);
		}
		else {
			m_serviceMap.erase(itm->first);
		}
		itm = m_serviceMap.begin();
	}
}

void Session::serviceFailure(const std::string& _serviceUrl) {
	OT_LOG_W("Handling service failure. Url: \"" + _serviceUrl + "\"");

	Service& service = this->findServiceFromURL(_serviceUrl);

	// Remove the service from the session
	SessionService::instance().serviceClosing(service, false, false);

	// Create emergency shutdown message
	ot::JsonDocument shutdownDoc;
	shutdownDoc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_ServiceEmergencyShutdown, shutdownDoc.GetAllocator()), shutdownDoc.GetAllocator());
	std::string msg = shutdownDoc.toJson();

	// Notify other service about shutdown.
	while (m_serviceMap.size() > 0) {
		auto serviceIt = m_serviceMap.begin();
		OTAssert(serviceIt != m_serviceMap.end(), "Data mismatch");

		// Send message
		std::string response;
		ot::msg::sendAsync("", _serviceUrl, ot::QUEUE, msg, ot::msg::defaultTimeout, ot::msg::DefaultFlagsNoExit);

		size_t oldCt = m_serviceMap.size();

		SessionService::instance().serviceClosing(serviceIt->second, false, false);
		if (oldCt == m_serviceMap.size()) {
			OT_LOG_EA("[FATAL] Service was not removed from the session during close call. Removing manually to avoid session lock...");
			m_serviceMap.erase(serviceIt);
		}
	}
}

void Session::addDebugPortInUse(ot::port_t _port) {
	m_debugPorts.push_back(_port);
}

bool Session::hasService(ot::serviceID_t _serviceId) const {
	return m_serviceMap.find(_serviceId) != m_serviceMap.end();
}

Service& Session::findService(ot::serviceID_t _serviceID) {
	auto itm = m_serviceMap.find(_serviceID);
	if (itm == m_serviceMap.end()) {
		throw ot::Exception::ObjectNotFound("A service with the ID \"" + std::to_string(_serviceID) + "\" was not registered");
	}

	return itm->second;
}

Service& Session::findServiceFromURL(const std::string& _serviceURL) {
	for (auto& it: m_serviceMap) {
		if (it.second.getServiceUrl() == _serviceURL) {
			return it.second;
		}
	}

	throw ot::Exception::ObjectNotFound("Service with the URL \"" + _serviceURL + "\" was not registered");
}

Service& Session::findServiceFromNameType(const std::string& _serviceName, const std::string& _serviceType) {
	for (auto& it : m_serviceMap) {
		if (it.second.getServiceName() == _serviceName && it.second.getServiceType() == _serviceType) {
			return it.second;
		}
	}

	throw ot::Exception::ObjectNotFound("Service was not registered { \"Name\": \"" + _serviceName + "\", \"Type\": \"" + _serviceType + "\" }");
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

ot::serviceID_t Session::generateNextServiceId(void) {
	return m_serviceIdManager.grabNextID();
}

// ###########################################################################################################################################################################################################################################################################################################################

// Private
