//! @file Service.cpp
//! @authors Alexander Kuester (alexk95)
//! @date June 2025
// ###########################################################################################################################################################################################################################################################################################################################

// Service header
#include "Service.h"
#include "Session.h"

// OpenTwin header
#include "OTCore/LogDispatcher.h"
#include "OTCore/String.h"
#include "OTCommunication/ActionTypes.h"

Service::Service(const ot::ServiceBase& _serviceInfo, const std::string& _sessionId) :
	ot::ServiceBase(_serviceInfo), m_state(Service::NoState)
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

	size_t colonIndex = this->getServiceURL().rfind(':');
	if (colonIndex != std::string::npos) {
		std::string port = this->getServiceURL().substr(colonIndex + 1);
		bool fail = false;
		ot::port_t portNumber = ot::String::toNumber<ot::port_t>(port, fail);
		if (fail) {
			OT_LOG_E("Failed to convert port number from URL: \"" + this->getServiceURL() + "\"");
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

std::string Service::debugLogString(const ot::ServiceBase& _serviceInfo, const std::string& _sessionID) {
	return "\"ServiceID\": " + std::to_string(_serviceInfo.getServiceID()) + ", \"ServiceName\": \"" + _serviceInfo.getServiceName() +
		"\", \"ServiceType\": \"" + _serviceInfo.getServiceType() + "\"" +
		(_serviceInfo.getServiceURL().empty() ? std::string() : ", \"ServiceUrl\": \"" + _serviceInfo.getServiceURL() + "\"") +
		", \"SessionID\": \"" + _sessionID + "\"";
}

std::string Service::debugLogString(const std::string& _sessionID) const {
	return Service::debugLogString(*this, _sessionID) + (m_websocketUrl.empty() ? std::string() : ", \"WebsocketUrl\": \"" + m_websocketUrl + "\"");
}

// ###########################################################################################################################################################################################################################################################################################################################

// Serialization

void Service::getDebugInfo(ot::LSSDebugInfo::ServiceInfo& _info) const {
	_info.id = this->getServiceID();
	_info.name = this->getServiceName();
	_info.type = this->getServiceType();
	_info.url = this->getServiceURL();
	_info.websocketUrl = this->getWebsocketUrl();
	_info.connectedType = m_connectedType;

	_info.isDebug = static_cast<bool>(m_state & Service::IsDebug);
	_info.isRequested = static_cast<bool>(m_state & Service::Requested);
	_info.isAlive = static_cast<bool>(m_state & Service::Alive);
	_info.isRunning = static_cast<bool>(m_state & Service::Running);
	_info.isShuttingDown = static_cast<bool>(m_state & Service::ShuttingDown);
	_info.isHidden = static_cast<bool>(m_state & Service::Hidden);
}

void Service::addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const {
	_object.AddMember(OT_ACTION_PARAM_SERVICE_ID, this->getServiceID(), _allocator);
	_object.AddMember(OT_ACTION_PARAM_SERVICE_URL, ot::JsonString(this->getServiceURL(), _allocator), _allocator);
	_object.AddMember(OT_ACTION_PARAM_SERVICE_NAME, ot::JsonString(this->getServiceName(), _allocator), _allocator);
	_object.AddMember(OT_ACTION_PARAM_SERVICE_TYPE, ot::JsonString(this->getServiceType(), _allocator), _allocator);

	_object.AddMember("IsRunning", static_cast<bool>(m_state & Service::Running), _allocator);
	_object.AddMember("IsAlive", static_cast<bool>(m_state & Service::Alive), _allocator);
	_object.AddMember("IsDebug", static_cast<bool>(m_state & Service::IsDebug), _allocator);
	_object.AddMember("IsRequested", static_cast<bool>(m_state & Service::Requested), _allocator);
	_object.AddMember("IsShuttingDown", static_cast<bool>(m_state & Service::ShuttingDown), _allocator);
	_object.AddMember("IsHidden", static_cast<bool>(m_state & Service::Hidden), _allocator);

	if (!m_websocketUrl.empty()) {
		_object.AddMember(OT_ACTION_PARAM_WebsocketURL, ot::JsonString(m_websocketUrl, _allocator), _allocator);
	}
}
