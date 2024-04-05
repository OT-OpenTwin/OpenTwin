//! @file PropertyBool.cpp
//! @author Alexander Kuester (alexk95)
//! @date February 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/JSON.h"
#include "OTCore/Logger.h"
#include "OTGui/PropertyBool.h"

ot::Property* ot::PropertyBool::createCopy(void) const {
	ot::PropertyBool* newProp = new ot::PropertyBool;
	newProp->setFromOther(this);

	newProp->setValue(this->value());

	return newProp;
}

void ot::PropertyBool::getPropertyData(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const {
	_object.AddMember("Value", m_value, _allocator);
}

void ot::PropertyBool::setPropertyData(const ot::ConstJsonObject& _object) {
	m_value = json::getBool(_object, "Value");
}