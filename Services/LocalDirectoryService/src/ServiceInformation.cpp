//! @file ServiceInformation.cpp
//! @author Alexander Kuester (alexk95)
//! @date September 2022
// ###########################################################################################################################################################################################################################################################################################################################

// LDS header
#include "ServiceInformation.h"

ServiceInformation::ServiceInformation() : 
	m_maxCrashRestarts(0), m_maxStartupRestarts(0), m_initializeAttempt(0), m_startCounter(0)
{}

ServiceInformation::ServiceInformation(const std::string& _name, const std::string& _type) :
	m_name(_name), m_type(_type), m_maxCrashRestarts(0), m_maxStartupRestarts(0), m_initializeAttempt(0), m_startCounter(0)
{}

ServiceInformation::ServiceInformation(const std::string& _name, const std::string & _type, const SessionInformation& _sessionInfo) :
	m_name(_name), m_type(_type), m_session(_sessionInfo), m_maxCrashRestarts(0), m_maxStartupRestarts(0), m_initializeAttempt(0), m_startCounter(0)
{}

ServiceInformation::ServiceInformation(const ServiceInformation& _other) :
	m_name(_other.m_name), m_type(_other.m_type), m_session(_other.m_session),
	m_initializeAttempt(_other.m_initializeAttempt), m_startCounter(_other.m_startCounter),
	m_maxCrashRestarts(_other.m_maxCrashRestarts), m_maxStartupRestarts(_other.m_maxStartupRestarts)
{}

ServiceInformation::ServiceInformation(ServiceInformation && _other) noexcept :
	m_name(std::move(_other.m_name)), m_type(std::move(_other.m_type)), m_session(std::move(_other.m_session)),
	m_initializeAttempt(_other.m_initializeAttempt), m_startCounter(_other.m_startCounter),
	m_maxCrashRestarts(_other.m_maxCrashRestarts), m_maxStartupRestarts(_other.m_maxStartupRestarts)
{}

ServiceInformation::~ServiceInformation() {}

ServiceInformation& ServiceInformation::operator = (const ServiceInformation& _other) {
	if (this != &_other) {
		m_name = _other.m_name;
		m_type = _other.m_type;
		m_session = _other.m_session;

		m_initializeAttempt = _other.m_initializeAttempt;
		m_startCounter = _other.m_startCounter;

		m_maxCrashRestarts = _other.m_maxCrashRestarts;
		m_maxStartupRestarts = _other.m_maxStartupRestarts;
	}
	
	return *this;
}

ServiceInformation& ServiceInformation::operator=(ServiceInformation&& _other) noexcept {
	if (this != &_other) {
		m_name = std::move(_other.m_name);
		m_type = std::move(_other.m_type);
		m_session = std::move(_other.m_session);

		m_initializeAttempt = _other.m_initializeAttempt;
		m_startCounter = _other.m_startCounter;

		m_maxCrashRestarts = _other.m_maxCrashRestarts;
		m_maxStartupRestarts = _other.m_maxStartupRestarts;
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

void ServiceInformation::addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const {
	_object.AddMember("Name", ot::JsonString(m_name, _allocator), _allocator);
	_object.AddMember("Type", ot::JsonString(m_type, _allocator), _allocator);

	_object.AddMember("InitializeAttempt", m_initializeAttempt, _allocator);
	_object.AddMember("StartCounter", m_startCounter, _allocator);

	_object.AddMember("MaxCrashRestarts", m_maxCrashRestarts, _allocator);
	_object.AddMember("MaxStartupRestarts", m_maxStartupRestarts, _allocator);

	ot::JsonObject sessionObj;
	m_session.addToJsonObject(sessionObj, _allocator);
	_object.AddMember("Session", sessionObj, _allocator);
}
