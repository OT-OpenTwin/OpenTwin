//! @file PropertyDouble.cpp
//! @author Alexander Kuester (alexk95)
//! @date February 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/JSON.h"
#include "OTCore/LogDispatcher.h"
#include "OTGui/PropertyDouble.h"
#include "OTGui/PropertyFactoryRegistrar.h"

static ot::PropertyFactoryRegistrar<ot::PropertyDouble> propertyDoubleRegistrar(ot::PropertyDouble::propertyTypeString());

ot::PropertyDouble::PropertyDouble(const PropertyDouble* _other) 
	: Property(_other), m_value(_other->m_value), m_min(_other->m_min), m_max(_other->m_max), m_precision(_other->m_precision), m_suffix(_other->m_suffix)
{}

ot::PropertyDouble::PropertyDouble(const PropertyBase & _base)
	: Property(_base), m_value(0.), m_min(std::numeric_limits<double>::lowest()), m_max(std::numeric_limits<double>::max()), m_precision(2)
{}

ot::PropertyDouble::PropertyDouble(PropertyFlags _flags)
	: Property(_flags), m_value(0.), m_min(std::numeric_limits<double>::lowest()), m_max(std::numeric_limits<double>::max()), m_precision(2)
{}

ot::PropertyDouble::PropertyDouble(double _value, PropertyFlags _flags )
	: Property(_flags), m_value(_value), m_min(std::numeric_limits<double>::lowest()), m_max(std::numeric_limits<double>::max()), m_precision(2)
{}

ot::PropertyDouble::PropertyDouble(const std::string& _name, double _value, PropertyFlags _flags)
	: Property(_name, _flags), m_value(_value), m_min(std::numeric_limits<double>::lowest()), m_max(std::numeric_limits<double>::max()), m_precision(2)
{}

ot::PropertyDouble::PropertyDouble(const std::string& _name, double _value, double _min, double _max, PropertyFlags _flags)
	: Property(_name, _flags), m_value(_value), m_min(_min), m_max(_max), m_precision(2)
{}

void ot::PropertyDouble::mergeWith(const Property* _other, const MergeMode& _mergeMode) {
	Property::mergeWith(_other, _mergeMode);

	const PropertyDouble* other = dynamic_cast<const PropertyDouble*>(_other);
	OTAssertNullptr(other);

	if (_mergeMode & PropertyBase::MergeValues) {
		m_value = other->m_value;
	}
	if (_mergeMode & PropertyBase::MergeConfig) {
		m_min = other->m_min;
		m_max = other->m_max;
		m_precision = other->m_precision;
		m_suffix = other->m_suffix;
	}
}

ot::Property* ot::PropertyDouble::createCopy(void) const {
	return new PropertyDouble(this);
}

void ot::PropertyDouble::getPropertyData(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const {
	_object.AddMember("Value", m_value, _allocator);
	_object.AddMember("Min", m_min, _allocator);
	_object.AddMember("Max", m_max, _allocator);
	_object.AddMember("Precision", m_precision, _allocator);
	_object.AddMember("Suffix", JsonString(m_suffix, _allocator), _allocator);
}

void ot::PropertyDouble::setPropertyData(const ot::ConstJsonObject& _object) {
	m_value = json::getDouble(_object, "Value");
	m_min = json::getDouble(_object, "Min");
	m_max = json::getDouble(_object, "Max");
	m_precision = json::getInt(_object, "Precision");
	if (_object.HasMember("Suffix")) {
		m_suffix = json::getString(_object, "Suffix");
	}
}