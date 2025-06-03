//! @file ServiceInformation.cpp
//! @author Alexander Kuester (alexk95)
//! @date September 2022
// ###########################################################################################################################################################################################################################################################################################################################

// GDS header
#include "ServiceInformation.h"

ServiceInformation::ServiceInformation() {}

ServiceInformation::ServiceInformation(const std::string& _name, const std::string& _type) :
	m_name(_name), m_type(_type)
{}

ServiceInformation::ServiceInformation(const std::string & _name, const std::string & _type, const SessionInformation & _session) :
	m_name(_name), m_type(_type), m_session(_session)
{}

ServiceInformation::ServiceInformation(const std::string & _name, const std::string & _type, const std::string & _sessionId, const std::string & _sessionServiceURL) :
	m_name(_name), m_type(_type), m_session(_sessionId, _sessionServiceURL)
{}

ServiceInformation::ServiceInformation(const ServiceInformation& _other) :
	m_name(_other.m_name), m_type(_other.m_type), m_session(_other.m_session)
{}

ServiceInformation::ServiceInformation(ServiceInformation && _other) noexcept :
	m_name(std::move(_other.m_name)), m_type(std::move(_other.m_type)), m_session(std::move(_other.m_session))
{}

ServiceInformation::~ServiceInformation() {}

ServiceInformation& ServiceInformation::operator = (const ServiceInformation& _other) {
	if (this != &_other) {
		m_name = _other.m_name;
		m_type = _other.m_type;
		m_session = _other.m_session;
	}
	
	return *this;
}

ServiceInformation& ServiceInformation::operator=(ServiceInformation&& _other) noexcept {
	if (this != &_other) {
		m_name = std::move(_other.m_name);
		m_type = std::move(_other.m_type);
		m_session = std::move(_other.m_session);
	}

	return *this;
}

bool ServiceInformation::operator == (const ServiceInformation& _other) const {
	return m_name == _other.m_name && m_type == _other.m_type && m_session == _other.m_session;
}

bool ServiceInformation::operator != (const ServiceInformation& _other) const {
	return !(*this == _other);
}

// ###########################################################################################################################################################################################################################################################################################################################

// Serialization

void ServiceInformation::addToJsonObject(ot::JsonValue& _jsonObject, ot::JsonAllocator& _allocator) const {
	_jsonObject.AddMember("Name", ot::JsonString(m_name, _allocator), _allocator);
	_jsonObject.AddMember("Type", ot::JsonString(m_type, _allocator), _allocator);

	ot::JsonObject sessionObj;
	m_session.addToJsonObject(sessionObj, _allocator);
	_jsonObject.AddMember("Session", sessionObj, _allocator);
}
