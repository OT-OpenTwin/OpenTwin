//! @file PropertyString.cpp
//! @author Alexander Kuester (alexk95)
//! @date February 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/JSON.h"
#include "OTCore/Logger.h"
#include "OTGui/PropertyString.h"

ot::PropertyString::PropertyString(PropertyFlags _flags)
	: Property(_flags), m_maxLength(0)
{}

ot::PropertyString::PropertyString(const std::string& _value, PropertyFlags _flags)
	: Property(_flags), m_value(_value), m_maxLength(0)
{}

ot::PropertyString::PropertyString(const std::string& _name, const std::string& _value, PropertyFlags _flags)
	: Property(_name, _flags), m_value(_value), m_maxLength(0)
{}

void ot::PropertyString::getPropertyData(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const {
	_object.AddMember("Value", JsonString(m_value, _allocator), _allocator);
	_object.AddMember("Placeholder", JsonString(m_placeholderText, _allocator), _allocator);
	_object.AddMember("MaxLength", m_maxLength, _allocator);
}

void ot::PropertyString::setPropertyData(const ot::ConstJsonObject& _object) {
	m_value = json::getString(_object, "Value");
	m_placeholderText = json::getString(_object, "Placeholder");
	m_maxLength = json::getUInt(_object, "MaxLength");
}