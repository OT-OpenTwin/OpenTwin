/*
 * relayService.cpp
 *
 *  Created on: January 08, 2020
 *	Author: Alexander Kuester
 *  Copyright (c) 2020 openTwin
 */

#include "RelayService.h"			// Corrsponding header

#include "OpenTwinCore/rJSON.h"
#include "OpenTwinCore/Logger.h"
#include "OpenTwinCommunication/ActionTypes.h"

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
	OT_rJSON_createDOC(doc);
	ot::rJSON::add(doc, OT_ACTION_PARAM_SERVICE_NAME, m_name);
	ot::rJSON::add(doc, OT_ACTION_PARAM_SERVICE_TYPE, m_type);
	ot::rJSON::add(doc, OT_ACTION_PARAM_SERVICE_URL, m_url);
	ot::rJSON::add(doc, OT_ACTION_PARAM_WebsocketURL, m_websocketIP);
	ot::rJSON::add(doc, "ConnectedService", m_connectedService->toJSON());
	return ot::rJSON::toJSON(doc);
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
