//! @file BasicServiceInformation.cpp
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/BasicServiceInformation.h"

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

void ot::BasicServiceInformation::addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const {
	_object.AddMember("BSI.ServiceName", JsonString(m_name, _allocator), _allocator);
	_object.AddMember("BSI.ServiceType", JsonString(m_type, _allocator), _allocator);
}

void ot::BasicServiceInformation::setFromJsonObject(const ConstJsonObject& _object) {
	m_name = json::getString(_object, "BSI.ServiceName");
	m_type = json::getString(_object, "BSI.ServiceType");
}
