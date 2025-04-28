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

Service::Service(const std::string& _url, const std::string& _name, ot::serviceID_t _id, const std::string& _type, const std::string& _sessionId, const ServiceFlags& _flags) :
	m_name(_name), m_url(_url), m_id(_id), m_sessionId(_sessionId), m_type(_type), m_flags(_flags)
{
	if (ot::LogDispatcher::mayLog(ot::DETAILED_LOG)) {
		ot::JsonDocument dbg;
		this->addDebugInfoToJsonObject(dbg, dbg.GetAllocator());

		ot::LogDispatcher::instance().dispatch("Service created: " + dbg.toJson(), __FUNCTION__, ot::DETAILED_LOG);
	}
}

Service::Service(Service&& _other) noexcept :
	m_name(std::move(_other.m_name)),
	m_url(std::move(_other.m_url)),
	m_id(_other.m_id),
	m_type(std::move(_other.m_type)),
	m_websocketUrl(std::move(_other.m_websocketUrl)),
	m_sessionId(std::move(_other.m_sessionId)),
	m_flags(std::move(_other.m_flags))
{
    _other.m_id = 0;
    _other.m_flags = ServiceFlag::NoServiceFlags;
}

Service::~Service() {
	if (ot::LogDispatcher::mayLog(ot::DETAILED_LOG)) {
		ot::JsonDocument dbg;
		this->addDebugInfoToJsonObject(dbg, dbg.GetAllocator());

		ot::LogDispatcher::instance().dispatch("Service destroyed: " + dbg.toJson(), __FUNCTION__, ot::DETAILED_LOG);
	}
}

Service& Service::operator=(Service&& _other) noexcept {
	if (this != &_other) {
		m_name = std::move(_other.m_name);
		m_url = std::move(_other.m_url);
		m_id = _other.m_id;
		m_type = std::move(_other.m_type);
		m_websocketUrl = std::move(_other.m_websocketUrl);
		m_sessionId = std::move(_other.m_sessionId);
		m_flags = std::move(_other.m_flags);

		// Reset the moved-from object
		_other.m_id = 0;
		_other.m_flags = ServiceFlag::NoServiceFlags;
	}

	return *this;
}

// ###########################################################################################################################################################################################################################################################################################################################

// Management

void Service::setVisible(void) {
	if (!m_flags.flagIsSet(ServiceFlag::IsVisible)) {
		m_flags.setFlag(ServiceFlag::IsVisible);
		OT_LOG_E("Implement");
		//m_session->broadcastAction(this, OT_ACTION_CMD_ServiceConnected, true);
	}
}

void Service::setHidden(void) {
	if (m_flags.flagIsSet(ServiceFlag::IsVisible)) {
		m_flags.removeFlag(ServiceFlag::IsVisible);
		OT_LOG_E("Implement");
		//m_session->broadcastAction(this, OT_ACTION_CMD_ServiceDisconnected, true);
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

	if (m_flags & ServiceFlag::HasWebsocket) {
		colonIndex = m_websocketUrl.rfind(':');
		if (colonIndex != std::string::npos) {
			std::string port = m_websocketUrl.substr(colonIndex + 1);

			ports.push_back(std::stol(port));
		}
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

	// Additional debug information
	OTAssertNullptr(m_session);
	_object.AddMember(OT_ACTION_PARAM_ReceiveBroadcast, this->getReceiveBroadcastMessages(), _allocator);
	_object.AddMember(OT_ACTION_PARAM_Visible, this->getIsVisible(), _allocator);
	_object.AddMember(OT_ACTION_PARAM_SESSION_ID, ot::JsonString(m_sessionId, _allocator), _allocator);
}

void Service::setFromJsonObject(const ot::ConstJsonObject& _object) {
	m_url = ot::json::getString(_object, OT_ACTION_PARAM_SERVICE_URL);
	m_name = ot::json::getString(_object, OT_ACTION_PARAM_SERVICE_NAME);
	m_id = ot::json::getUInt(_object, OT_ACTION_PARAM_SERVICE_ID);
	m_type = ot::json::getString(_object, OT_ACTION_PARAM_SERVICE_TYPE);
	
	if (_object.HasMember(OT_ACTION_PARAM_ReceiveBroadcast)) {
		m_flags.setFlag(ServiceFlag::ReceiveBroadcastMessages, ot::json::getBool(_object, OT_ACTION_PARAM_ReceiveBroadcast));
	}
	if (_object.HasMember(OT_ACTION_PARAM_Visible)) {
		m_flags.setFlag(ServiceFlag::IsVisible, ot::json::getBool(_object, OT_ACTION_PARAM_Visible));
	}
	if (_object.HasMember(OT_ACTION_PARAM_SESSION_ID)) {
		if (m_sessionId != ot::json::getString(_object, OT_ACTION_PARAM_SESSION_ID)) {
			OT_LOG_E("Invalid session ID");
		}
	}
}
