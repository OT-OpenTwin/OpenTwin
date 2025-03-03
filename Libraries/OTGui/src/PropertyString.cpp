//! @file PropertyString.cpp
//! @author Alexander Kuester (alexk95)
//! @date February 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/JSON.h"
#include "OTCore/Logger.h"
#include "OTGui/PropertyString.h"
#include "OTGui/PropertyFactoryRegistrar.h"

static ot::PropertyFactoryRegistrar<ot::PropertyString> propertyStringRegistrar(ot::PropertyString::propertyTypeString());

ot::PropertyString::PropertyString(const PropertyString* _other) 
	: Property(_other), m_value(_other->m_value), m_placeholderText(_other->m_placeholderText), m_maxLength(_other->m_maxLength)
{}

ot::PropertyString::PropertyString(const PropertyBase & _base)
	: Property(_base), m_maxLength(0)
{}

ot::PropertyString::PropertyString(PropertyFlags _flags)
	: Property(_flags), m_maxLength(0)
{}

ot::PropertyString::PropertyString(const std::string& _value, PropertyFlags _flags)
	: Property(_flags), m_value(_value), m_maxLength(0)
{}

ot::PropertyString::PropertyString(const std::string& _name, const std::string& _value, PropertyFlags _flags)
	: Property(_name, _flags), m_value(_value), m_maxLength(0)
{}

void ot::PropertyString::setValueFromOther(const Property* _other) {
	const PropertyString* other = dynamic_cast<const PropertyString*>(_other);
	m_value = other->m_value;
	m_placeholderText = other->m_placeholderText;
	m_maxLength = other->m_maxLength;
}

ot::Property* ot::PropertyString::createCopy(void) const {
	return new PropertyString(this);
}

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