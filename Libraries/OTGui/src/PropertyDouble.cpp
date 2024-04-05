//! @file PropertyDouble.cpp
//! @author Alexander Kuester (alexk95)
//! @date February 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/JSON.h"
#include "OTCore/Logger.h"
#include "OTGui/PropertyDouble.h"

ot::PropertyDouble::PropertyDouble(PropertyFlags _flags)
	: Property(_flags), m_value(false), m_min(DBL_MIN), m_max(DBL_MAX), m_precision(2)
{}

ot::PropertyDouble::PropertyDouble(double _value, PropertyFlags _flags )
	: Property(_flags), m_value(_value), m_min(DBL_MIN), m_max(DBL_MAX), m_precision(2)
{}

ot::PropertyDouble::PropertyDouble(const std::string& _name, double _value, PropertyFlags _flags)
	: Property(_name, _flags), m_value(_value), m_min(DBL_MIN), m_max(DBL_MAX), m_precision(2)
{}

ot::Property* ot::PropertyDouble::createCopy(void) const {
	ot::PropertyDouble* newProp = new ot::PropertyDouble;
	newProp->setFromOther(this);

	newProp->setValue(this->value());
	newProp->setMin(this->min());
	newProp->setMax(this->max());
	newProp->setPrecision(this->precision());

	return newProp;
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