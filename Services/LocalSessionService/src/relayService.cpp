/*
 * relayService.cpp
 *
 *  Created on: January 08, 2020
 *	Author: Alexander Kuester
 *  Copyright (c) 2020 openTwin
 */

#include "RelayService.h"			// Corrsponding header

#include "OTCore/Logger.h"
#include "OTCommunication/ActionTypes.h"

#include <cassert>

RelayService::RelayService(
	const std::string &			_ip,
	const std::string &			_name,
	ot::serviceID_t				_id,
	const std::string &			_type,
	Session *					_session,
	const std::string &			_websocketIp,
	Service *					_connectedService
) : Service(_ip, _name, _id, _type, _session, false), m_connectedService(_connectedService), m_websocketIP(_websocketIp)
{
	assert(m_connectedService != nullptr);
	OT_LOG_D("Created relay service " + toJSON());
}

RelayService::~RelayService() { delete m_connectedService; }

std::string RelayService::toJSON(void) const {
	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_PARAM_SERVICE_NAME, ot::JsonString(m_name, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_SERVICE_TYPE, ot::JsonString(m_type, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_SERVICE_URL, ot::JsonString(m_url, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_WebsocketURL, ot::JsonString(m_websocketIP, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember("ConnectedService", ot::JsonString(m_connectedService->toJSON(), doc.GetAllocator()), doc.GetAllocator());
	return doc.toJson();
}

std::list<unsigned long long> RelayService::portNumbers(void) const {

	std::list<unsigned long long> ports = Service::portNumbers();

	size_t colonIndex = m_websocketIP.rfind(':');
	if (colonIndex != std::string::npos)
	{
		std::string port = m_websocketIP.substr(colonIndex + 1);

		ports.push_back(std::stol(port));
	}

	assert(!ports.empty()); // The port number could not be extracted from the ip address
	return ports;
}
