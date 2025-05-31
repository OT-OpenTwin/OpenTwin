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
	m_id(_id), m_userName(_userName), m_projectName(_projectName), m_collectionName(_collectionName), m_type(_type)
{
	OT_LOG_D("New session created { \"ID\": \"" + m_id + "\", \"UserName\": \"" + m_userName + "\", \"ProjectName\": \"" + 
		m_projectName + "\", \"CollectionName\": \"" + m_collectionName + "\", \"SessionType\": \"" + m_type + "\" }");
}

Session::~Session() {
	
}

void Session::addRequestedService(const std::string& _serviceName, const std::string& _serviceType) {
	// Check for duplicate
	for (size_t i = 0; i < m_requestedServices.size(); i++) {
		if (m_requestedServices[i].first == _serviceName && m_requestedServices[i].second == _serviceType) {
			OT_LOG_W("Requested service: " + _serviceName + " (type = " + _serviceType + ") is already added as requested at the session (id = " + m_id + ")");
			return;
		}
	}

	// Check if the requested service is already alive
	for (auto s : m_serviceMap) {
		if (s.second->getName() == _serviceName && s.second->getType() == _serviceType) {
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

void Session::addDebugPortInUse(ot::port_t _port) {
	m_debugPorts.push_back(_port);
}

Service * Session::registerService(const std::string & _serviceIP, const std::string & _serviceName, const std::string & _serviceType, ot::serviceID_t _serviceID) {
	auto itm = m_serviceMap.find(_serviceID);
	if (itm != m_serviceMap.end()) {
		std::string errorMessage("A service with the ID \"");
		errorMessage.append(std::to_string(_serviceID));
		errorMessage.append("\" was already registered");
		throw std::out_of_range(errorMessage.c_str());
	}
	Service * newService = new Service(_serviceIP, _serviceName, _serviceID, _serviceType, this, true);
	m_serviceMap.insert_or_assign(_serviceID, newService);

	OT_LOG_D("Service with { \"id\": \"" + std::to_string(_serviceID) + "\"} was registered at session with {\"id\":\"" + m_id + "\"}");

	return newService;
}

Service * Session::registerService(Service * _service) {
	assert(_service != nullptr);

	auto itm = m_serviceMap.find(_service->getId());
	if (itm != m_serviceMap.end()) {
		std::string errorMessage("A service with the ID \"");
		errorMessage.append(std::to_string(_service->getId()));
		errorMessage.append("\" was already registered");
		throw std::out_of_range(errorMessage.c_str());
	}

	m_serviceMap.insert_or_assign(_service->getId(), _service);

	OT_LOG_D("The service (name = \"" + _service->getName() + "\"; type = \"" + _service->getType() + "\"; id = \"" + std::to_string(_service->getId()) + "\") was registered at the session (id = \"" + m_id + "\")");

	return _service;
}

void Session::removeService(ot::serviceID_t _serviceID, bool _notifyOthers) {
	auto itm = m_serviceMap.find(_serviceID);
	if (itm == m_serviceMap.end()) {
		OT_LOG_EAS("Service (" + std::to_string(_serviceID) + ") not found");
		return;
	}
	Service * theService = itm->second;
	m_serviceMap.erase(_serviceID);
	m_serviceIdManager.freeID(_serviceID);

	if (_notifyOthers) {
		broadcastAction(theService, OT_ACTION_CMD_ServiceDisconnected);
	}

	//OT_LOG_D("The service " + theService->toJSON() + " was deregistered from the session " + infoToJSON());

	delete theService;
}

Service * Session::getService(ot::serviceID_t _serviceID) {
	auto itm = m_serviceMap.find(_serviceID);
	if (itm == m_serviceMap.end()) {
		std::string errorMessage("A service with the ID \"");
		errorMessage.append(std::to_string(_serviceID));
		errorMessage.append("\" was not registered");
		throw std::out_of_range(errorMessage.c_str());
	}
	return itm->second;
}

Service * Session::getServiceFromURL(const std::string& _serviceURL) {
	for (auto s : m_serviceMap) {
		if (s.second->getUrl() == _serviceURL) return s.second;
	}
	std::string errorMessage("A service with the URL \"");
	errorMessage.append(_serviceURL);
	errorMessage.append("\" was not registered");
	throw std::out_of_range(errorMessage.c_str());
}

std::list<Service *> Session::getServicesByName(const std::string& _serviceName) {
	std::list<Service *> ret;
	for (auto s : m_serviceMap) {
		if (s.second->getName() == _serviceName) {
			ret.push_back(s.second);
		}
	}
	return ret;
}

void Session::addServiceListToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const {
	ot::JsonArray arr;
	for (const auto& s : m_serviceMap) {
		if (s.second->getIsVisible()) {
			ot::JsonObject obj;
			s.second->addToJsonObject(obj, _allocator);
			arr.PushBack(obj, _allocator);
		}
	}
	_object.AddMember(OT_ACTION_PARAM_SESSION_SERVICES, arr, _allocator);
}

void Session::servicesInformation(ot::JsonArray &servicesInfo, ot::JsonAllocator& allocator) const
{
	for (auto s : m_serviceMap)
	{
		ot::JsonValue infoDoc;
		infoDoc.SetObject();

		std::string serviceType = s.second->getType();
		if (serviceType == "UI") serviceType = "RelayService";

		infoDoc.AddMember(OT_ACTION_PARAM_SERVICE_TYPE, ot::JsonString(serviceType, allocator), allocator);
		infoDoc.AddMember(OT_ACTION_PARAM_SERVICE_URL, ot::JsonString(s.second->getUrl(), allocator), allocator);

		servicesInfo.PushBack(infoDoc, allocator);
	}
}

void Session::broadcastMessage(Service * _sender, const std::string& _message, bool _async) {
	OT_BROADCASTMESSAGE_CREATE(theMessageDoc);
	theMessageDoc.AddMember(OT_ACTION_PARAM_MESSAGE, ot::JsonString(_message, theMessageDoc.GetAllocator()), theMessageDoc.GetAllocator());

	if (_sender != nullptr)
	{
		OT_BROADCASTMESSAGE_ADDSENDERDATA(theMessageDoc, _sender);
	}

	broadcast(_sender, theMessageDoc, false, _async);
}

void Session::broadcastAction(Service * _sender, const std::string& _command, bool _async) {
	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(_command, doc.GetAllocator()), doc.GetAllocator());
	if (_sender != nullptr)
	{
		OT_BROADCASTACTION_ADDSENDERDATA(doc, _sender);
	}
	broadcast(_sender, doc, false, _async);
}

void Session::broadcast(Service * _sender, const ot::JsonDocument& _message, bool _shutdown, bool _async) {
	std::string msg = _message.toJson();
	OT_LOG_D("Sending broadcast message in session (id = \"" + m_id + "\"): " + msg);
	
	std::string response;
	std::string senderIP;
	if (_sender != nullptr) { senderIP = _sender->getUrl(); }

	for (auto itm : m_serviceMap) {
		// todo: check service visibility upon session start -> service start -> service.new action
		if (itm.second->getReceiveBroadcastMessages() && itm.second->getIsVisible()) {
			if (senderIP != itm.second->getUrl()) {
				// Send the message to the current reciever
				response.clear();
				ot::msg::RequestFlags reqFlags = ot::msg::DefaultFlagsNoExit;
				if (_async) {
					ot::msg::sendAsync(senderIP, itm.second->getUrl(), ot::QUEUE, msg, ot::msg::defaultTimeout, reqFlags);
				} else {
					if (_shutdown) {
						reqFlags |= ot::msg::IsShutdownMessage;
					}

					if (!ot::msg::send(senderIP, itm.second->getUrl(), ot::QUEUE, msg, response, ot::msg::defaultTimeout, reqFlags)) {
						OT_LOG_E("Failed to send broadcast message to: " + itm.second->getUrl());
					}
				}
			}
		}
	}
}

void Session::shutdown(Service * _sender) {
	broadcastAction(_sender, OT_ACTION_CMD_ServicePreShutdown);

	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_ServiceShutdown, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_SESSION_EXISTS, false, doc.GetAllocator());
	
	broadcast(_sender, doc, true);

	auto itm = m_serviceMap.begin();
	while (itm != m_serviceMap.end()) {
		if (itm->second != _sender) {
			SessionService::instance().serviceClosing(itm->second, false, false);
		}
		else {
			m_serviceMap.erase(_sender->getId());
		}
		itm = m_serviceMap.begin();
	}
}

void Session::serviceFailure(Service * _failedService) {
	if (_failedService != nullptr) {
		OT_LOG_W("Handling service failure (name = \"" + _failedService->getName() + "\"; type = \"" + _failedService->getType() + 
			"\"; id = \"" + std::to_string(_failedService->getId()) + "\"");
		SessionService::instance().serviceClosing(_failedService, false, false);
	}

	ot::JsonDocument shutdownDoc;
	shutdownDoc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_ServiceEmergencyShutdown, shutdownDoc.GetAllocator()), shutdownDoc.GetAllocator());
	std::string msg = shutdownDoc.toJson();
	std::string response;
	size_t oldCt;
	while (m_serviceMap.size() > 0) {
		auto s = m_serviceMap.begin();
		
		// Fire message
		ot::msg::sendAsync("", s->second->getUrl(), ot::QUEUE, msg, ot::msg::defaultTimeout, ot::msg::DefaultFlagsNoExit);

		oldCt = m_serviceMap.size();
		SessionService::instance().serviceClosing(s->second, false, false);
		if (oldCt == m_serviceMap.size()) {
			assert(0);
		}
	}
}

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
	return m_serviceIdManager.nextID();
}

// ###########################################################################################################################################################################################################################################################################################################################

// Private: Helper

void Session::prepareBroadcastDocument(ot::JsonDocument& _doc, const std::string& _action, ot::serviceID_t _senderService) const {
	_doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(_action, _doc.GetAllocator()), _doc.GetAllocator());
	
	if (_senderService != ot::invalidServiceID) {
		auto info = this->getServiceFromId(_senderService);
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
