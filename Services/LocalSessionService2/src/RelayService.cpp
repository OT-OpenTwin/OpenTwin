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
}

RelayService::~RelayService() { delete m_connectedService; }

std::list<unsigned long long> RelayService::getPortNumbers(void) const {

	std::list<unsigned long long> ports = Service::getPortNumbers();

	size_t colonIndex = m_websocketIP.rfind(':');
	if (colonIndex != std::string::npos)
	{
		std::string port = m_websocketIP.substr(colonIndex + 1);

		ports.push_back(std::stol(port));
	}

	assert(!ports.empty()); // The port number could not be extracted from the ip address
	return ports;
}
