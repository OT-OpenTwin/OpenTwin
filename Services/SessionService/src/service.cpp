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

#include "OpenTwinCore/Logger.h"
#include "OpenTwinCommunication/actionTypes.h"	// 

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
		m_session->broadcastAction(this, OT_ACTION_CMD_ServiceConnected);
	}
}

void Service::setHidden(void) {
	if (m_isVisible) {
		m_isVisible = false;
		m_session->broadcastAction(this, OT_ACTION_CMD_ServiceDisconnected);
	}
}

std::string Service::toJSON(void) const {
	OT_rJSON_createDOC(doc);
	ot::rJSON::add(doc, OT_ACTION_PARAM_SERVICE_ID, m_id);
	ot::rJSON::add(doc, OT_ACTION_PARAM_SERVICE_NAME, m_name);
	ot::rJSON::add(doc, OT_ACTION_PARAM_SERVICE_TYPE, m_type);
	ot::rJSON::add(doc, OT_ACTION_PARAM_SERVICE_URL, m_url);
	return ot::rJSON::toJSON(doc);
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

void Service::writeDataToValue(OT_rJSON_doc & _doc, OT_rJSON_val & _value) {
	_value.SetObject();
	ot::rJSON::add(_doc, _value, OT_ACTION_PARAM_SERVICE_ID, m_id);
	ot::rJSON::add(_doc, _value, OT_ACTION_PARAM_SERVICE_NAME, m_name);
	ot::rJSON::add(_doc, _value, OT_ACTION_PARAM_SERVICE_TYPE, m_type);
	ot::rJSON::add(_doc, _value, OT_ACTION_PARAM_SERVICE_URL, m_url);
}