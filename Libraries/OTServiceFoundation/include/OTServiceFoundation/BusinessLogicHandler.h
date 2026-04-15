// @otlicense
// File: BusinessLogicHandler.h
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

// OpenTwin header
#include "OTServiceFoundation/UiComponent.h"
#include "OTServiceFoundation/ModelComponent.h"
#include "OTServiceFoundation/FoundationAPIExport.h"

// std header
#include <list>
#include <string>
#include <vector>

//! @brief Common methods for all central service handler classes.
class OT_SERVICEFOUNDATION_API_EXPORT BusinessLogicHandler
{
	friend class FixtureBusinessLogicHandler;
public:
	void setUIComponent(ot::components::UiComponent * ui) 
	{ 
		_uiComponent = ui; 
		UIComponentWasSet();
	};
	void setModelComponent(ot::components::ModelComponent * model) 
	{
		_modelComponent = model;
		ModelComponentWasSet();
	};
	virtual ~BusinessLogicHandler() {};

protected:
	ot::components::UiComponent * _uiComponent = nullptr;
	ot::components::ModelComponent * _modelComponent = nullptr;

	inline void checkEssentials();

	std::string createNewUniqueTopologyName(const std::string& folderName, const std::string& fileName, int startNumber = 1, bool alwaysNumbered = false);
	std::vector<std::string> createNewUniqueTopologyNames(std::list<std::string>& folderContent, const std::string& folderName, const std::string& fileName, uint64_t numberOfFiles, int startNumber = 1, bool alwaysNumbered = false);
	std::vector<std::string> createNewUniqueTopologyNames(const std::string& folderName, const std::string& fileName, uint64_t numberOfFiles, int startNumber = 1, bool alwaysNumbered = false);
		
	virtual void UIComponentWasSet() {};
	virtual void ModelComponentWasSet() {};
};

