/*
 * service.cpp
 *
 *  Created on: November 30, 2020
 *	Author: Alexander Kuester
 *  Copyright (c) 2020 openTwin
 */

#include <cassert>

#include <Service.h>			// Corresponding header
#include <Session.h>			// The session managing this servic

#include "OTCore/Logger.h"
#include "OTCommunication/actionTypes.h"	// 

Service::Service(
	const std::string &			_url,
	const std::string &			_name,
	ot::serviceID_t					_id,
	const std::string &			_type,
	Session *					_session,
	bool						_showDebugInfo
) : m_name(_name), m_url(_url), m_id(_id), m_session(_session), m_type(_type), m_receiveBroadcastMessages(true), m_isVisible(false)
{
	OT_LOG_D("[NEW] Created service " + toJSON());
}

void Service::setVisible(void) {
	if (!m_isVisible) {
		m_isVisible = true;
		m_session->broadcastAction(this, OT_ACTION_CMD_ServiceConnected, true);
	}
}

void Service::setHidden(void) {
	if (m_isVisible) {
		m_isVisible = false;
		m_session->broadcastAction(this, OT_ACTION_CMD_ServiceDisconnected, true);
	}
}

std::string Service::toJSON(void) const {
	ot::JsonDocument doc;
	this->writeDataToValue(doc, doc.GetAllocator());
	return doc.toJson();
}

std::list<unsigned long long> Service::portNumbers(void) const {

	std::list<unsigned long long> ports;

	size_t colonIndex = m_url.rfind(':');
	if (colonIndex != std::string::npos)
	{
		std::string port = m_url.substr(colonIndex + 1);

		ports.push_back(std::stol(port));
	}

	assert(!ports.empty()); // The port number could not be extracted from the ip address
	return ports;
}

void Service::writeDataToValue(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const {
	_object.AddMember(OT_ACTION_PARAM_SERVICE_ID, m_id, _allocator);
	_object.AddMember(OT_ACTION_PARAM_SERVICE_NAME, ot::JsonString(m_name, _allocator), _allocator);
	_object.AddMember(OT_ACTION_PARAM_SERVICE_TYPE, ot::JsonString(m_type, _allocator), _allocator);
	_object.AddMember(OT_ACTION_PARAM_SERVICE_URL, ot::JsonString(m_url, _allocator), _allocator);
}