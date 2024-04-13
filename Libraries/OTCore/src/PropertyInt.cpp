//! @file PropertyInt.cpp
//! @author Alexander Kuester (alexk95)
//! @date February 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/JSON.h"
#include "OTCore/Logger.h"
#include "OTCore/PropertyInt.h"
#include "OTCore/PropertyFactoryRegistrar.h"

static ot::PropertyFactoryRegistrar<ot::PropertyInt> propertyIntRegistrar(OT_PROPERTY_TYPE_Int);

ot::PropertyInt::PropertyInt(const PropertyInt* _other) 
	: Property(_other), m_min(_other->m_min), m_max(_other->m_max), m_value(_other->m_value)
{}

ot::PropertyInt::PropertyInt(const PropertyBase& _base)
	: Property(_base), m_value(0), m_min(INT_MIN), m_max(INT_MAX)
{}

ot::PropertyInt::PropertyInt(PropertyFlags _flags) 
	: Property(_flags), m_value(0), m_min(INT_MIN), m_max(INT_MAX)
{}

ot::PropertyInt::PropertyInt(int _value, PropertyFlags _flags)
	: Property(_flags), m_value(_value), m_min(INT_MIN), m_max(INT_MAX)
{}

ot::PropertyInt::PropertyInt(const std::string& _name, int _value, PropertyFlags _flags) 
	: Property(_name, _flags), m_value(_value), m_min(INT_MIN), m_max(INT_MAX)
{}

ot::Property* ot::PropertyInt::createCopy(void) const {
	return new PropertyInt(this);
}

void ot::PropertyInt::getPropertyData(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const {
	_object.AddMember("Value", m_value, _allocator);
	_object.AddMember("Min", m_min, _allocator);
	_object.AddMember("Max", m_max, _allocator);
}

void ot::PropertyInt::setPropertyData(const ot::ConstJsonObject& _object) {
	m_value = json::getInt(_object, "Value");
	m_min = json::getInt(_object, "Min");
	m_max = json::getInt(_object, "Max");
}