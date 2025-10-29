// @otlicense

//! @file PropertyFactory.cpp
//! @author Alexander Kuester (alexk95)
//! @date February 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/LogDispatcher.h"
#include "OTGui/Property.h"
#include "OTGui/PropertyFactory.h"

ot::PropertyFactory& ot::PropertyFactory::instance(void) {
	static PropertyFactory g_instance;
	return g_instance;
}

ot::Property* ot::PropertyFactory::createProperty(const ConstJsonObject& _jsonObject) {
	std::string type = json::getString(_jsonObject, OT_JSON_MEMBER_Property_Type);
	Property* prop = PropertyFactory::createProperty(type);
	if (prop) {
		prop->setFromJsonObject(_jsonObject);
	}
	return prop;
}

ot::Property* ot::PropertyFactory::createProperty(const std::string& _propertyType) {
	PropertyFactory& factory = PropertyFactory::instance();
	auto it = factory.m_constructors.find(_propertyType);
	if (it == factory.m_constructors.end()) {
		OT_LOG_E("Property \"" + _propertyType + "\" is not registered");
		return nullptr;
	}
	else {
		return it->second();
	}
}

bool ot::PropertyFactory::registerProperty(const std::string& _type, const PropertyConstructor& _constructor) {
	PropertyFactory& factory = PropertyFactory::instance();
	auto it = factory.m_constructors.find(_type);
	if (it != factory.m_constructors.end()) {
		OT_LOG_E("Property \"" + _type + "\" already registered");
		return false;
	}
	factory.m_constructors.insert_or_assign(_type, _constructor);
	return true;
}

ot::PropertyFactory::PropertyFactory() {

}

ot::PropertyFactory::~PropertyFactory() {

}