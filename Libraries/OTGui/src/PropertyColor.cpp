//! @file PropertyColor.cpp
//! @author Alexander Kuester (alexk95)
//! @date February 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/JSON.h"
#include "OTCore/LogDispatcher.h"
#include "OTGui/PropertyColor.h"
#include "OTGui/PropertyFactoryRegistrar.h"

static ot::PropertyFactoryRegistrar<ot::PropertyColor> propertyColorRegistrar(ot::PropertyColor::propertyTypeString());

ot::PropertyColor::PropertyColor(const PropertyColor* _other) 
	: Property(_other), m_value(_other->m_value), m_includeAlpha(_other->m_includeAlpha)
{}

ot::PropertyColor::PropertyColor(const PropertyBase& _base)
	: Property(_base), m_includeAlpha(false)
{}

ot::PropertyColor::PropertyColor(PropertyFlags _flags) :
	Property(_flags), m_includeAlpha(false)
{
}

ot::PropertyColor::PropertyColor(ot::Color _value, PropertyFlags _flags) :
	Property(_flags), m_value(_value), m_includeAlpha(false)
{}

ot::PropertyColor::PropertyColor(const std::string & _name, ot::Color _value, PropertyFlags _flags) :
	Property(_name, _flags), m_value(_value), m_includeAlpha(false)
{}

void ot::PropertyColor::mergeWith(const Property* _other, const MergeMode& _mergeMode) {
	Property::mergeWith(_other, _mergeMode);

	const PropertyColor* other = dynamic_cast<const PropertyColor*>(_other);
	OTAssertNullptr(other);

	if (_mergeMode & PropertyBase::MergeValues) {	
		m_value = other->m_value;
	}
	if (_mergeMode & PropertyBase::MergeConfig) {
		m_includeAlpha = other->m_includeAlpha;
	}
}

ot::Property* ot::PropertyColor::createCopy(void) const {
	return new PropertyColor(this);
}

void ot::PropertyColor::getPropertyData(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const {
	JsonObject colorObj;
	m_value.addToJsonObject(colorObj, _allocator);
	_object.AddMember("Value", colorObj, _allocator);
	_object.AddMember("IncludeAlpha", m_includeAlpha, _allocator);
}

void ot::PropertyColor::setPropertyData(const ot::ConstJsonObject& _object) {
	if (_object.HasMember("IncludeAlpha")) {
		m_includeAlpha = json::getBool(_object, "IncludeAlpha");
	}
	
	ConstJsonObject colorObj = json::getObject(_object, "Value");
	m_value.setFromJsonObject(colorObj);
}