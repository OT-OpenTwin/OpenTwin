//! @file PropertyBool.cpp
//! @author Alexander Kuester (alexk95)
//! @date February 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/JSON.h"
#include "OTCore/Logger.h"
#include "OTGui/PropertyBool.h"
#include "OTGui/PropertyFactoryRegistrar.h"

static ot::PropertyFactoryRegistrar<ot::PropertyBool> propertyBoolRegistrar(ot::PropertyBool::propertyTypeString());

ot::PropertyBool::PropertyBool(const PropertyBool* _other)
	: Property(_other), m_value(_other->m_value)
{}

ot::PropertyBool::PropertyBool(const PropertyBase& _base)
	: Property(_base), m_value(false)
{}

ot::PropertyBool::PropertyBool(PropertyFlags _flags)
	: Property(_flags), m_value(false)
{}

ot::PropertyBool::PropertyBool(bool _value, PropertyFlags _flags)
	: Property(_flags), m_value(_value)
{}

ot::PropertyBool::PropertyBool(const std::string& _name, bool _value, PropertyFlags _flags)
	: Property(_name, _flags), m_value(_value) 
{}

void ot::PropertyBool::mergeWith(const Property* _other, const MergeMode& _mergeMode) {
	Property::mergeWith(_other, _mergeMode);

	if (_mergeMode & PropertyBase::MergeValues) {
		const PropertyBool* other = dynamic_cast<const PropertyBool*>(_other);
		OTAssertNullptr(other);
		m_value = other->m_value;
	}
}

ot::Property* ot::PropertyBool::createCopy(void) const {
	return new PropertyBool(this);
}

void ot::PropertyBool::getPropertyData(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const {
	_object.AddMember("Value", m_value, _allocator);
}

void ot::PropertyBool::setPropertyData(const ot::ConstJsonObject& _object) {
	m_value = json::getBool(_object, "Value");
}