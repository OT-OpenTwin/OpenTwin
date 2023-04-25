#include "ServiceInformation.h"

ServiceInformation::ServiceInformation() 
	: m_MaxCrashRestarts(0), m_MaxStartupRestarts(0)
{}

ServiceInformation::ServiceInformation(const std::string& _name, const std::string& _type) 
	: m_Name(_name), m_Type(_type), m_MaxCrashRestarts(0), m_MaxStartupRestarts(0)
{}

ServiceInformation::ServiceInformation(const ServiceInformation& _other) 
	: m_Name(_other.m_Name), m_Type(_other.m_Type), m_MaxCrashRestarts(_other.m_MaxCrashRestarts), m_MaxStartupRestarts(_other.m_MaxStartupRestarts)
{}

ServiceInformation::~ServiceInformation() {}

ServiceInformation& ServiceInformation::operator = (const ServiceInformation& _other) {
	m_Name = _other.m_Name;
	m_Type = _other.m_Type;
	m_MaxCrashRestarts = _other.m_MaxCrashRestarts;
	m_MaxStartupRestarts = _other.m_MaxStartupRestarts;
	return *this;
}

bool ServiceInformation::operator == (const ServiceInformation& _other) const {
	return (m_Name == _other.m_Name && 
		m_Type == _other.m_Type && 
		m_MaxCrashRestarts == _other.m_MaxCrashRestarts && 
		m_MaxStartupRestarts == _other.m_MaxStartupRestarts);
}

bool ServiceInformation::operator != (const ServiceInformation& _other) const {
	return !(*this == _other);
}

void ServiceInformation::addToJsonObject(OT_rJSON_doc& _document, OT_rJSON_val& _object) const {
	ot::rJSON::add(_document, _object, "Name", m_Name);
	ot::rJSON::add(_document, _object, "Type", m_Type);
	ot::rJSON::add(_document, _object, "MaxCrashRestarts", m_MaxCrashRestarts);
	ot::rJSON::add(_document, _object, "MaxStartupRestarts", m_MaxStartupRestarts);
}

void ServiceInformation::setFromJsonObject(OT_rJSON_val& _object) {
	m_Name = ot::rJSON::getString(_object, "Name");
	m_Type = ot::rJSON::getString(_object, "Type");
	m_MaxCrashRestarts = ot::rJSON::getUInt(_object, "MaxCrashRestarts");
	m_MaxStartupRestarts = ot::rJSON::getUInt(_object, "MaxStartupRestarts");
}