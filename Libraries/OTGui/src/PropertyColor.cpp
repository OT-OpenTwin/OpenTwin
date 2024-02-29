//! @file PropertyColor.cpp
//! @author Alexander Kuester (alexk95)
//! @date February 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/JSON.h"
#include "OTCore/Logger.h"
#include "OTGui/PropertyColor.h"

void ot::PropertyColor::getPropertyData(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const {
	JsonObject colorObj;
	m_value.addToJsonObject(colorObj, _allocator);
	_object.AddMember("Value", colorObj, _allocator);
}

void ot::PropertyColor::setPropertyData(const ot::ConstJsonObject& _object) {
	ConstJsonObject colorObj = json::getObject(_object, "Value");
	m_value.setFromJsonObject(colorObj);
}