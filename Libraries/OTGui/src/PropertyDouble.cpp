//! @file PropertyDouble.cpp
//! @author Alexander Kuester (alexk95)
//! @date February 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/JSON.h"
#include "OTCore/Logger.h"
#include "OTGui/PropertyDouble.h"
#include "OTGui/PropertyFactoryRegistrar.h"

static ot::PropertyFactoryRegistrar<ot::PropertyDouble> propertyDoubleRegistrar(OT_PROPERTY_TYPE_Double);

ot::PropertyDouble::PropertyDouble(const PropertyDouble* _other) 
	: Property(_other), m_value(_other->m_value), m_min(_other->m_min), m_max(_other->m_max), m_precision(_other->m_precision)
{}

ot::PropertyDouble::PropertyDouble(const PropertyBase & _base)
	: Property(_base), m_value(0.), m_min(DBL_MIN), m_max(DBL_MAX), m_precision(2)
{}

ot::PropertyDouble::PropertyDouble(PropertyFlags _flags)
	: Property(_flags), m_value(0.), m_min(DBL_MIN), m_max(DBL_MAX), m_precision(2)
{}

ot::PropertyDouble::PropertyDouble(double _value, PropertyFlags _flags )
	: Property(_flags), m_value(_value), m_min(DBL_MIN), m_max(DBL_MAX), m_precision(2)
{}

ot::PropertyDouble::PropertyDouble(const std::string& _name, double _value, PropertyFlags _flags)
	: Property(_name, _flags), m_value(_value), m_min(DBL_MIN), m_max(DBL_MAX), m_precision(2)
{}

ot::Property* ot::PropertyDouble::createCopy(void) const {
	return new PropertyDouble(this);
}

void ot::PropertyDouble::getPropertyData(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const {
	_object.AddMember("Value", m_value, _allocator);
	_object.AddMember("Min", m_min, _allocator);
	_object.AddMember("Max", m_max, _allocator);
	_object.AddMember("Precision", m_precision, _allocator);
}

void ot::PropertyDouble::setPropertyData(const ot::ConstJsonObject& _object) {
	m_value = json::getDouble(_object, "Value");
	m_min = json::getDouble(_object, "Min");
	m_max = json::getDouble(_object, "Max");
	m_precision = json::getInt(_object, "Precision");
}