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

EntityBase* EntityFactory::create(ot::FileExtension::DefaultFileExtension _fileExtension) {
	auto it = m_fileExtensionCreators.find(_fileExtension);
	if (it != m_fileExtensionCreators.end()) {
		return it->second();
	}
	else {
		return nullptr;
	}
}

bool EntityFactory::registerClass(const std::string& _className, CreateEntityFunction _createFunction) {
	return m_creators.emplace(_className, std::move(_createFunction)).second;
}

bool EntityFactory::registerClass(ot::FileExtension::DefaultFileExtension _fileExtension, CreateEntityFunction _createFunction) {
	return m_fileExtensionCreators.emplace(_fileExtension, std::move(_createFunction)).second;
}
