/*
 * session.cpp
 *
 *  Created on: December 03, 2020
 *	Author: Alexander Kuester
 *  Copyright (c) 2020 openTwin
 */

// Session service header
#include <Session.h>		// Corresponding header
#include <Service.h>		// service
#include <SessionService.h>
#include <RelayService.h>	// relayService
#include <Exception.h>		// Error handling

#include "OTCore/Logger.h"
#include "OTCommunication/Msg.h"
#include "OTCommunication/ActionTypes.h"

#define OT_BROADCASTMESSAGE_CREATE(___doc) ot::JsonDocument ___doc; ___doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_Message, ___doc.GetAllocator()), ___doc.GetAllocator());
#define OT_BROADCASTMESSAGE_ADDSENDERDATA(___doc, ___sender) ___doc.AddMember(OT_ACTION_PARAM_SERVICE_ID, ___sender->id(), ___doc.GetAllocator()); ___doc.AddMember(OT_ACTION_PARAM_SERVICE_URL, ot::JsonString(___sender->url(), ___doc.GetAllocator()), ___doc.GetAllocator()); ___doc.AddMember(OT_ACTION_PARAM_SERVICE_NAME, ot::JsonString(___sender->name(), ___doc.GetAllocator()), ___doc.GetAllocator()); ___doc.AddMember(OT_ACTION_PARAM_SERVICE_TYPE, ot::JsonString(___sender->type(), ___doc.GetAllocator()), ___doc.GetAllocator());
#define OT_BROADCASTACTION_ADDSENDERDATA(___doc, ___sender) ___doc.AddMember(OT_ACTION_PARAM_SERVICE_ID, ___sender->id(), ___doc.GetAllocator()); ___doc.AddMember(OT_ACTION_PARAM_SERVICE_URL, ot::JsonString(___sender->url(), ___doc.GetAllocator()), ___doc.GetAllocator()); ___doc.AddMember(OT_ACTION_PARAM_SERVICE_NAME, ot::JsonString(___sender->name(), ___doc.GetAllocator()), ___doc.GetAllocator()); ___doc.AddMember(OT_ACTION_PARAM_SERVICE_TYPE, ot::JsonString(___sender->type(), ___doc.GetAllocator()), ___doc.GetAllocator());

Session::Session(
	const std::string &						_ID,
	const std::string &						_userName,
	const std::string &						_projectName,
	const std::string &						_collectionName,
	const std::string &						_type
) : m_id(_ID), m_userName(_userName), m_projectName(_projectName), m_collectionName(_collectionName),
	m_type(_type)
{
	OT_LOG_D("New session created with (ID = \"" + m_id + "\", User.Name = \"" + m_userName + "\", Project.Name = \"" + 
		m_projectName + "\", Collection.Name = \"" + m_collectionName + "\", Session.Type = \"" + m_type + "\")");
}

Session::~Session() {
	for (auto p : m_debugPorts) ot::PortManager::instance().setPortNotInUse(p);
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
		if (s.second->name() == _serviceName && s.second->type() == _serviceType) {
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
		throw ErrorException(errorMessage.c_str());
	}
	Service * newService = new Service(_serviceIP, _serviceName, _serviceID, _serviceType, this, true);
	m_serviceMap.insert_or_assign(_serviceID, newService);

	OT_LOG_D("Service with { \"id\": \"" + std::to_string(_serviceID) + "\"} was registered at session with {\"id\":\"" + m_id + "\"}");

	return newService;
}

Service * Session::registerService(Service * _service) {
	assert(_service != nullptr);

	auto itm = m_serviceMap.find(_service->id());
	if (itm != m_serviceMap.end()) {
		std::string errorMessage("A service with the ID \"");
		errorMessage.append(std::to_string(_service->id()));
		errorMessage.append("\" was already registered");
		throw ErrorException(errorMessage.c_str());
	}

	m_serviceMap.insert_or_assign(_service->id(), _service);

	OT_LOG_D("The service (name = \"" + _service->name() + "\"; type = \"" + _service->type() + "\"; id = \"" + std::to_string(_service->id()) + "\") was registered at the session (id = \"" + m_id + "\")");

	return _service;
}

void Session::removeService(ot::serviceID_t _serviceID, bool _notifyOthers) {
	auto itm = m_serviceMap.find(_serviceID);
	if (itm == m_serviceMap.end()) {
		std::string errorMessage("A service with the ID \"");
		errorMessage.append(std::to_string(_serviceID));
		errorMessage.append("\" was not registered");
		throw ErrorException(errorMessage.c_str());
	}
	Service * theService = itm->second;
	m_serviceMap.erase(_serviceID);
	m_serviceIdManager.freeID(_serviceID);

	if (_notifyOthers) { broadcastAction(theService, OT_ACTION_CMD_ServiceDisconnected); }

	OT_LOG_D("The service " + theService->toJSON() + " was deregistered from the session " + infoToJSON());

	delete theService;
}

Service * Session::getService(ot::serviceID_t _serviceID) {
	auto itm = m_serviceMap.find(_serviceID);
	if (itm == m_serviceMap.end()) {
		std::string errorMessage("A service with the ID \"");
		errorMessage.append(std::to_string(_serviceID));
		errorMessage.append("\" was not registered");
		throw ErrorException(errorMessage.c_str());
	}
	return itm->second;
}

Service * Session::getServiceFromURL(const std::string& _serviceURL) {
	for (auto s : m_serviceMap) {
		if (s.second->url() == _serviceURL) return s.second;
	}
	std::string errorMessage("A service with the URL \"");
	errorMessage.append(_serviceURL);
	errorMessage.append("\" was not registered");
	throw ErrorException(errorMessage.c_str());
}

std::list<Service *> Session::getServicesByName(const std::string& _serviceName) {
	std::list<Service *> ret;
	for (auto s : m_serviceMap) {
		if (s.second->name() == _serviceName) ret.push_back(s.second);
	}
	return ret;
}

std::string Session::getServiceListJSON(void) {
	std::string ret;
	if (m_serviceMap.size() > 0) {
		bool first = true;
		ret = "{\"" OT_ACTION_PARAM_SESSION_SERVICES "\":[";
		for (auto itm : m_serviceMap) {
			if (itm.second->isVisible()) {
				if (first) { first = false; }
				else { ret.append(","); }
				ret.append(itm.second->toJSON());
			}
		}
		ret.append("]}");
	}
	return ret;
}

std::string Session::infoToJSON(void) const {
	ot::JsonDocument infoDoc;
	infoDoc.AddMember(OT_ACTION_PARAM_SESSION_ID, ot::JsonString(m_id, infoDoc.GetAllocator()), infoDoc.GetAllocator());
	infoDoc.AddMember(OT_ACTION_PARAM_SESSION_TYPE, ot::JsonString(m_type, infoDoc.GetAllocator()), infoDoc.GetAllocator());
	return infoDoc.toJson();
}

void Session::servicesInformation(ot::JsonArray &servicesInfo, ot::JsonAllocator& allocator) const
{
	for (auto s : m_serviceMap)
	{
		ot::JsonValue infoDoc;
		infoDoc.SetObject();

		std::string serviceType = s.second->type();
		if (serviceType == "UI") serviceType = "RelayService";

		infoDoc.AddMember(OT_ACTION_PARAM_SERVICE_TYPE, ot::JsonString(serviceType, allocator), allocator);
		infoDoc.AddMember(OT_ACTION_PARAM_SERVICE_URL, ot::JsonString(s.second->url(), allocator), allocator);

		servicesInfo.PushBack(infoDoc, allocator);
	}
}

void Session::addServiceListToDocument(ot::JsonDocument& _doc) {
	ot::JsonArray serviceList;
	for (auto s : m_serviceMap)
	{
		if (s.second->isVisible()) {
			ot::JsonObject serviceObj;
			s.second->writeDataToValue(serviceObj, _doc.GetAllocator());
			serviceList.PushBack(serviceObj, _doc.GetAllocator());
		}
	}
	_doc.AddMember(OT_ACTION_PARAM_SESSION_SERVICES, serviceList, _doc.GetAllocator());
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
	if (_sender != nullptr) { senderIP = _sender->url(); }

	for (auto itm : m_serviceMap) {
		// todo: check service visibility upon session start -> service start -> service.new action
		if (itm.second->receiveBroadcastMessages() && itm.second->isVisible()) {
			if (senderIP != itm.second->url()) {
				// Send the message to the current reciever
				response.clear();
				if (_async) {
					ot::msg::sendAsync(senderIP, itm.second->url(), ot::QUEUE, msg, 3000);
				} else if (!ot::msg::send(senderIP, itm.second->url(), ot::QUEUE, msg, response, 3000, _shutdown)) {
					OT_LOG_E("Failed to send broadcast message to: " + itm.second->url());
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
			SessionService::instance()->serviceClosing(itm->second, false, false);
		}
		else {
			m_serviceMap.erase(_sender->id());
		}
		itm = m_serviceMap.begin();
	}
}

void Session::serviceFailure(Service * _failedService) {
	if (_failedService != nullptr) {
		OT_LOG_W("Handling service failure (name = \"" + _failedService->name() + "\"; type = \"" + _failedService->type() + 
			"\"; id = \"" + std::to_string(_failedService->id()) + "\"");
		SessionService::instance()->serviceClosing(_failedService, false, false);
	}

	ot::JsonDocument shutdownDoc;
	shutdownDoc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_ServiceEmergencyShutdown, shutdownDoc.GetAllocator()), shutdownDoc.GetAllocator());
	std::string msg = shutdownDoc.toJson();
	std::string response;
	size_t oldCt;
	while (m_serviceMap.size() > 0) {
		auto s = m_serviceMap.begin();
		
		// Fire message
		ot::msg::sendAsync("", s->second->url(), ot::QUEUE, msg);

		oldCt = m_serviceMap.size();
		SessionService::instance()->serviceClosing(s->second, false, false);
		if (oldCt == m_serviceMap.size()) {
			assert(0);
		}
	}
}

std::list<std::string> Session::toolBarTabOrder(void) {
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
