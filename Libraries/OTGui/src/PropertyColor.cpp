//! @file PropertyColor.cpp
//! @author Alexander Kuester (alexk95)
//! @date February 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/JSON.h"
#include "OTCore/Logger.h"
#include "OTGui/PropertyColor.h"
#include "OTGui/PropertyFactoryRegistrar.h"

static ot::PropertyFactoryRegistrar<ot::PropertyColor> propertyColorRegistrar(ot::PropertyColor::propertyTypeString());

ot::PropertyColor::PropertyColor(const PropertyColor* _other) 
	: Property(_other), m_value(_other->m_value)
{}

ot::PropertyColor::PropertyColor(const PropertyBase& _base)
	: Property(_base)
{}

ot::Property* ot::PropertyColor::createCopy(void) const {
	return new PropertyColor(this);
}

void ot::PropertyColor::getPropertyData(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const {
	JsonObject colorObj;
	m_value.addToJsonObject(colorObj, _allocator);
	_object.AddMember("Value", colorObj, _allocator);
}

void ot::PropertyColor::setPropertyData(const ot::ConstJsonObject& _object) {
	ConstJsonObject colorObj = json::getObject(_object, "Value");
	m_value.setFromJsonObject(colorObj);
}