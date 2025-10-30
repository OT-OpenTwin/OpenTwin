// @otlicense
// File: LibraryManagementWrapper.h
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

#pragma once

// std header
#include <string>
#include <list>

#include "OTCore/JSON.h"
#include "OTCore/OTClassHelper.h"
#include "OTServiceFoundation/BusinessLogicHandler.h"

class LibraryManagementWrapper : public BusinessLogicHandler {
public:
	LibraryManagementWrapper() {};
	~LibraryManagementWrapper() {};

	std::list<std::string> getCircuitModels();
	std::string getCircuitModel(std::string _modelName);
	std::string requestCreateConfig(const ot::JsonDocument& _doc);
	void createModelTextEntity(const std::string& _modelInfo, const std::string& _folder, const std::string& _elementType, const std::string& _modelName);
	void updatePropertyOfEntity(ot::UID _entityID, bool _dialogConfirmed,const std::string& _propertyValue);
private:

	ot::UIDList m_entityIDsTopo;
	ot::UIDList m_entityVersionsTopo;
	ot::UIDList m_entityIDsData {};
	ot::UIDList m_entityVersionsData {};
	std::list<bool> m_forceVisible;

	void addModelToEntites();
};