//! @file Service.cpp
//! @authors Alexander Kuester (alexk95)
//! @date June 2025
// ###########################################################################################################################################################################################################################################################################################################################

// Service header
#include "Service.h"
#include "Session.h"

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTCore/String.h"
#include "OTCommunication/ActionTypes.h"

Service::Service(const ot::ServiceBase& _serviceInfo, const std::string& _sessionId) :
	ot::ServiceBase(_serviceInfo), m_sessionId(_sessionId), m_state(Service::NoState), m_id(ot::invalidServiceID)
{
	OT_LOG_D([=]() {
		ot::JsonDocument dbg;
		this->addToJsonObject(dbg, dbg.GetAllocator());
		return "Service created: " + dbg.toJson();
	}());
}

Service::~Service() {
	OT_LOG_D([=]() {
		ot::JsonDocument dbg;
		this->addToJsonObject(dbg, dbg.GetAllocator());
		return "Service destroyed: " + dbg.toJson();
		}());
}

// ###########################################################################################################################################################################################################################################################################################################################

// Setter / Getter

std::list<ot::port_t> Service::getPortNumbers(void) const {
	std::list<ot::port_t> ports;

	size_t colonIndex = m_url.rfind(':');
	if (colonIndex != std::string::npos) {
		std::string port = m_url.substr(colonIndex + 1);
		bool fail = false;
		ot::port_t portNumber = ot::String::toNumber<ot::port_t>(port, fail);
		if (fail) {
			OT_LOG_E("Failed to convert port number from URL: \"" + m_url + "\"");
		}
		else {
			ports.push_back(portNumber);
		}
	}

	colonIndex = m_websocketUrl.rfind(':');
	if (colonIndex != std::string::npos) {
		std::string port = m_websocketUrl.substr(colonIndex + 1);
		bool fail = false;
		ot::port_t portNumber = ot::String::toNumber<ot::port_t>(port, fail);
		if (fail) {
			OT_LOG_E("Failed to convert port number from WebSocket URL: \"" + m_websocketUrl + "\"");
		}
		else {
			ports.push_back(portNumber);
		}
	}

	if (ports.empty()) {
		OT_LOG_EA("Failed to excract port numbers");
	}

	return ports;
}

// ###########################################################################################################################################################################################################################################################################################################################

// Serialization

void Service::addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const {
	_object.AddMember(OT_ACTION_PARAM_SERVICE_ID, m_id, _allocator);
	_object.AddMember(OT_ACTION_PARAM_SERVICE_URL, ot::JsonString(m_url, _allocator), _allocator);
	_object.AddMember(OT_ACTION_PARAM_SERVICE_NAME, ot::JsonString(m_name, _allocator), _allocator);
	_object.AddMember(OT_ACTION_PARAM_SERVICE_TYPE, ot::JsonString(m_type, _allocator), _allocator);

	_object.AddMember(OT_ACTION_PARAM_Visible, static_cast<bool>(m_state & Service::Visible), _allocator);
	_object.AddMember(OT_ACTION_PARAM_IsShutdown, static_cast<bool>(m_state & Service::ShuttingDown), _allocator);
	
	_object.AddMember(OT_ACTION_PARAM_SESSION_ID, ot::JsonString(m_sessionId, _allocator), _allocator);
}
