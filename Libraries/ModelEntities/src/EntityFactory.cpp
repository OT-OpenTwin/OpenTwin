//! @file EntityFactory.cpp
//! @author Alexander Kuester (alexk95)
//! @date October 2025
// ###########################################################################################################################################################################################################################################################################################################################

#include "EntityFactory.h"
#include "OTCore/LogDispatcher.h"

EntityFactory& EntityFactory::instance() {
	static EntityFactory g_instance;
	return g_instance;
}

EntityBase* EntityFactory::create(const std::string& _className) {
	auto it = m_creators.find(_className);
	if (it != m_creators.end()) {
		return it->second();
	}
	else {
		OT_LOG_W("Unknown class name: \"" + _className + "\"");
		return nullptr;
	}
}

bool EntityFactory::registerClass(const std::string& _className, CreateEntityFunction _createFunction) {
	return m_creators.emplace(_className, std::move(_createFunction)).second;
}
