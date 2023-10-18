//! @file BasicServiceInformation.cpp
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OpenTwinCore/BasicServiceInformation.h"

ot::BasicServiceInformation::BasicServiceInformation() {}

ot::BasicServiceInformation::BasicServiceInformation(const std::string& _serviceName, const std::string& _serviceType) : m_name(_serviceName), m_type(_serviceType) {}

ot::BasicServiceInformation::BasicServiceInformation(const BasicServiceInformation& _other) : m_name(_other.m_name), m_type(_other.m_type) {}

ot::BasicServiceInformation::~BasicServiceInformation() {}

ot::BasicServiceInformation& ot::BasicServiceInformation::operator = (const BasicServiceInformation& _other) {
	m_name = _other.m_name;
	m_type = _other.m_type;
	return *this;
}

bool ot::BasicServiceInformation::operator == (const BasicServiceInformation& _other) const {
	return m_name == _other.m_name && m_type == _other.m_type;
}

bool ot::BasicServiceInformation::operator != (const BasicServiceInformation& _other) const {
	return m_name != _other.m_name || m_type != _other.m_type;
}

bool ot::BasicServiceInformation::operator < (const BasicServiceInformation& _other) const {
	return m_name < _other.m_name && m_type < _other.m_type;
}

void ot::BasicServiceInformation::addToJsonObject(OT_rJSON_doc& _document, OT_rJSON_val& _object) const {
	ot::rJSON::add(_document, _object, "BSI.ServiceName", m_name);
	ot::rJSON::add(_document, _object, "BSI.ServiceType", m_type);
}

void ot::BasicServiceInformation::setFromJsonObject(OT_rJSON_val& _object) {
	m_name = ot::rJSON::getString(_object, "BSI.ServiceName");
	m_type = ot::rJSON::getString(_object, "BSI.ServiceType");
}
