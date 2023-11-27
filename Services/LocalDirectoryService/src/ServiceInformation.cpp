#include "ServiceInformation.h"

ServiceInformation::ServiceInformation() 
	: m_maxCrashRestarts(0), m_maxStartupRestarts(0)
{}

ServiceInformation::ServiceInformation(const std::string& _name, const std::string& _type) 
	: m_name(_name), m_type(_type), m_maxCrashRestarts(0), m_maxStartupRestarts(0)
{}

ServiceInformation::ServiceInformation(const ServiceInformation& _other) 
	: m_name(_other.m_name), m_type(_other.m_type), m_maxCrashRestarts(_other.m_maxCrashRestarts), m_maxStartupRestarts(_other.m_maxStartupRestarts)
{}

ServiceInformation::~ServiceInformation() {}

ServiceInformation& ServiceInformation::operator = (const ServiceInformation& _other) {
	m_name = _other.m_name;
	m_type = _other.m_type;
	m_maxCrashRestarts = _other.m_maxCrashRestarts;
	m_maxStartupRestarts = _other.m_maxStartupRestarts;
	return *this;
}

bool ServiceInformation::operator == (const ServiceInformation& _other) const {
	return (m_name == _other.m_name && 
		m_type == _other.m_type && 
		m_maxCrashRestarts == _other.m_maxCrashRestarts && 
		m_maxStartupRestarts == _other.m_maxStartupRestarts);
}

bool ServiceInformation::operator != (const ServiceInformation& _other) const {
	return !(*this == _other);
}

void ServiceInformation::addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const {
	_object.AddMember("Name", ot::JsonString(m_name, _allocator), _allocator);
	_object.AddMember("Type", ot::JsonString(m_type, _allocator), _allocator);
	_object.AddMember("MaxCrashRestarts", m_maxCrashRestarts, _allocator);
	_object.AddMember("MaxStartupRestarts", m_maxStartupRestarts, _allocator);
}

void ServiceInformation::setFromJsonObject(const ot::ConstJsonObject& _object) {
	m_name = ot::json::getString(_object, "Name");
	m_type = ot::json::getString(_object, "Type");
	m_maxCrashRestarts = ot::json::getUInt(_object, "MaxCrashRestarts");
	m_maxStartupRestarts = ot::json::getUInt(_object, "MaxStartupRestarts");
}