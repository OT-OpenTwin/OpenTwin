//! @file PropertyInputFactory.cpp
//! @author Alexander Kuester (alexk95)
//! @date February 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTCore/Property.h"
#include "OTWidgets/PropertyInput.h"
#include "OTWidgets/PropertyInputFactory.h"

ot::PropertyInputFactory& ot::PropertyInputFactory::instance(void) {
	static PropertyInputFactory g_instance;
	return g_instance;
}

ot::PropertyInput* ot::PropertyInputFactory::createInput(const std::string& _key) {
	PropertyInputFactory& factory = PropertyInputFactory::instance();
	auto it = factory.m_constructors.find(_key);
	if (it == factory.m_constructors.end()) {
		OT_LOG_E("PropertyInput \"" + _key + "\" not registered");
		return nullptr;
	}
	return it->second();
}

ot::PropertyInput* ot::PropertyInputFactory::createInput(const Property* _config) {
	OTAssertNullptr(_config);
	PropertyInput* inp = PropertyInputFactory::createInput(_config->getPropertyType());
	if (inp) {
		inp->setupFromConfiguration(_config);
	}
	return inp;
}

bool ot::PropertyInputFactory::registerPropertyInput(const std::string& _key, const PropertyInputConstructor& _constructor) {
	PropertyInputFactory& factory = PropertyInputFactory::instance();
	if (factory.m_constructors.find(_key) != factory.m_constructors.end()) {
		OT_LOG_E("PropertyInput \"" + _key + "\" already registered");
		return false;
	}
	factory.m_constructors.insert_or_assign(_key, _constructor);
	return true;
}