// @otlicense
// File: BusinessLogicHandler.cpp
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

#include "OTSystem/OTAssert.h"
#include "OTCore/EntityName.h"
#include "OTServiceFoundation/BusinessLogicHandler.h"
#include "OTModelAPI/ModelServiceAPI.h"

std::string BusinessLogicHandler::createNewUniqueTopologyName(const std::string& folderName, const std::string& fileName, int startNumber, bool alwaysNumbered)
{
	assert(_modelComponent != nullptr);
	std::list<std::string> folderItems = ot::ModelServiceAPI::getListOfFolderItems(folderName);
	ot::EntityName::NamingBehavior namingBehavior;
	namingBehavior.startNumber = startNumber;
	namingBehavior.alwaysNumbered = alwaysNumbered;
	return ot::EntityName::createUniqueEntityName(folderName, folderItems, fileName, namingBehavior);
}

std::vector<std::string> BusinessLogicHandler::createNewUniqueTopologyNames(std::list<std::string>& folderContent, const std::string& folderName, const std::string& fileName, uint64_t numberOfFiles, int startNumber, bool alwaysNumbered)
{
	std::vector<std::string> newNames;
	newNames.reserve(numberOfFiles);

	ot::EntityName::NamingBehavior namingBehavior;
	namingBehavior.startNumber = startNumber;
	namingBehavior.alwaysNumbered = alwaysNumbered;

	for (uint64_t i = 0; i < numberOfFiles; i++)
	{
		newNames.push_back(ot::EntityName::createUniqueEntityName(folderName, folderContent, fileName, namingBehavior));
		folderContent.push_back(newNames.back());
	}
	return newNames;
}

std::vector<std::string> BusinessLogicHandler::createNewUniqueTopologyNames(const std::string& folderName, const std::string& fileName, uint64_t numberOfFiles, int startNumber, bool alwaysNumbered)
{
	assert(_modelComponent != nullptr);
	std::list<std::string> folderItems = ot::ModelServiceAPI::getListOfFolderItems(folderName);
	return createNewUniqueTopologyNames(folderItems, folderName, fileName, numberOfFiles, startNumber, alwaysNumbered);
}

inline void BusinessLogicHandler::checkEssentials()
{
	OTAssert(_modelComponent != nullptr, "No model component set");
	OTAssert(_uiComponent != nullptr, "No UI component set");
}