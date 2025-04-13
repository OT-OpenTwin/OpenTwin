//! @file Service.cpp
//! @authors Alexander Kuester (alexk95)
//! @date November 2020
// ###########################################################################################################################################################################################################################################################################################################################

// Service header
#include "Service.h"
#include "Session.h"

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTCommunication/actionTypes.h"

// std header
#include <cassert>

Service::Service(const std::string& _url, const std::string& _name, ot::serviceID_t _id, const std::string& _type, Session* _session, bool _showDebugInfo) :
	m_name(_name), m_url(_url), m_id(_id), m_session(_session), m_type(_type), m_receiveBroadcastMessages(true), m_isVisible(false)
{
	if (ot::LogDispatcher::mayLog(ot::DETAILED_LOG)) {
		ot::JsonDocument dbg;
		this->addDebugInfoToJsonObject(dbg, dbg.GetAllocator());

		ot::LogDispatcher::instance().dispatch("Service created: " + dbg.toJson(), __FUNCTION__, ot::DETAILED_LOG);
	}
}

Service::~Service() {
	if (ot::LogDispatcher::mayLog(ot::DETAILED_LOG)) {
		ot::JsonDocument dbg;
		this->addDebugInfoToJsonObject(dbg, dbg.GetAllocator());

		ot::LogDispatcher::instance().dispatch("Service destroyed: " + dbg.toJson(), __FUNCTION__, ot::DETAILED_LOG);
	}
}

// ###########################################################################################################################################################################################################################################################################################################################

// Management

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

// ###########################################################################################################################################################################################################################################################################################################################

// Setter / Getter

std::list<unsigned long long> Service::getPortNumbers(void) const {
	std::list<unsigned long long> ports;

	size_t colonIndex = m_url.rfind(':');
	if (colonIndex != std::string::npos) {
		std::string port = m_url.substr(colonIndex + 1);

		ports.push_back(std::stol(port));
	}

	assert(!ports.empty()); // The port number could not be extracted from the ip address
	return ports;
}

// ###########################################################################################################################################################################################################################################################################################################################

// Serialization

void Service::addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const {
	_object.AddMember(OT_ACTION_PARAM_SERVICE_URL, ot::JsonString(m_url, _allocator), _allocator);
	_object.AddMember(OT_ACTION_PARAM_SERVICE_NAME, ot::JsonString(m_name, _allocator), _allocator);
	_object.AddMember(OT_ACTION_PARAM_SERVICE_ID, m_id, _allocator);
	_object.AddMember(OT_ACTION_PARAM_SERVICE_TYPE, ot::JsonString(m_type, _allocator), _allocator);
}

void Service::addDebugInfoToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const {
	this->addToJsonObject(_object, _allocator);
	OTAssertNullptr(m_session);
	_object.AddMember(OT_ACTION_PARAM_ReceiveBroadcast, m_receiveBroadcastMessages, _allocator);
	_object.AddMember(OT_ACTION_PARAM_Visible, m_isVisible, _allocator);
	_object.AddMember(OT_ACTION_PARAM_SESSION_ID, ot::JsonString(m_session->getId(), _allocator), _allocator);
}

void Service::setFromJsonObject(const ot::ConstJsonObject& _object) {
	m_url = ot::json::getString(_object, OT_ACTION_PARAM_SERVICE_URL);
	m_name = ot::json::getString(_object, OT_ACTION_PARAM_SERVICE_NAME);
	m_id = ot::json::getUInt(_object, OT_ACTION_PARAM_SERVICE_ID);
	m_type = ot::json::getString(_object, OT_ACTION_PARAM_SERVICE_TYPE);
	
	if (_object.HasMember(OT_ACTION_PARAM_ReceiveBroadcast)) {
		m_receiveBroadcastMessages = ot::json::getBool(_object, OT_ACTION_PARAM_ReceiveBroadcast);
	}
	if (_object.HasMember(OT_ACTION_PARAM_Visible)) {
		m_isVisible = ot::json::getBool(_object, OT_ACTION_PARAM_Visible);
	}
	if (_object.HasMember(OT_ACTION_PARAM_SESSION_ID)) {
		if (m_session->getId() != ot::json::getString(_object, OT_ACTION_PARAM_SESSION_ID)) {
			OT_LOG_E("Invalid session ID");
		}
	}
}
