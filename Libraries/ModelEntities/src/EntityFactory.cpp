// @otlicense
// File: EntityFactory.cpp
// 
// License:
// Copyright 2025 by OpenTwin
//  
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
//  
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// @otlicense-end

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
