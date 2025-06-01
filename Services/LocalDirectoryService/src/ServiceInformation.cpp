//! @file ServiceInformation.cpp
//! @author Alexander Kuester (alexk95)
//! @date September 2022
// ###########################################################################################################################################################################################################################################################################################################################

// LDS header
#include "ServiceInformation.h"

ServiceInformation::ServiceInformation() :
	m_id(ot::invalidServiceID), m_maxCrashRestarts(0), m_maxStartupRestarts(0), m_initializeAttempt(0), m_startCounter(0)
{}

ServiceInformation::ServiceInformation(const std::string& _name, const std::string & _type, ot::serviceID_t _serviceID, const SessionInformation& _sessionInfo) :
	m_name(_name), m_type(_type), m_id(_serviceID), m_session(_sessionInfo), m_maxCrashRestarts(0), m_maxStartupRestarts(0), m_initializeAttempt(0), m_startCounter(0)
{}

ServiceInformation::~ServiceInformation() {}

bool ServiceInformation::operator == (const ServiceInformation& _other) const {
	return m_name == _other.m_name && m_type == _other.m_type && m_id == _other.m_id && m_session == _other.m_session;
}

bool ServiceInformation::operator != (const ServiceInformation& _other) const {
	return !(*this == _other);
}

// ###########################################################################################################################################################################################################################################################################################################################

// Serialization

void ServiceInformation::addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const {
	_object.AddMember("Name", ot::JsonString(m_name, _allocator), _allocator);
	_object.AddMember("Type", ot::JsonString(m_type, _allocator), _allocator);
	_object.AddMember("ID", m_id, _allocator);

	_object.AddMember("InitializeAttempt", m_initializeAttempt, _allocator);
	_object.AddMember("StartCounter", m_startCounter, _allocator);

	_object.AddMember("MaxCrashRestarts", m_maxCrashRestarts, _allocator);
	_object.AddMember("MaxStartupRestarts", m_maxStartupRestarts, _allocator);

	ot::JsonObject sessionObj;
	m_session.addToJsonObject(sessionObj, _allocator);
	_object.AddMember("Session", sessionObj, _allocator);
}
