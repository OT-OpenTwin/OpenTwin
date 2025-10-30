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

#include "BusinessLogicHandler.h"

#include "OTModelAPI/ModelServiceAPI.h"

std::string BusinessLogicHandler::CreateNewUniqueTopologyName(const std::string& folderName, const std::string& fileName)
{
	std::list<std::string> folderContent = ot::ModelServiceAPI::getListOfFolderItems(folderName);
	int count = 1;
	std::string fullFileName = folderName + "/" + fileName + "_" + std::to_string(count);
	while (std::find(folderContent.begin(), folderContent.end(), fullFileName) != folderContent.end())
	{
		fullFileName = folderName + "/" + fileName + "_" + std::to_string(count);
		count++;
	}
	return fullFileName;
}

std::string BusinessLogicHandler::CreateNewUniqueTopologyNamePlainPossible(const std::string& folderName, const std::string& fileName, std::list<std::string>& additionallyTakenNames)
{
	std::list<std::string> folderContent = ot::ModelServiceAPI::getListOfFolderItems(folderName);
	for (const std::string& takenName : additionallyTakenNames)
	{
		folderContent.push_back(takenName);
	}
	
	int count = 1;
	std::string fullFileName = folderName + "/" + fileName;
	while (std::find(folderContent.begin(), folderContent.end(), fullFileName) != folderContent.end())
	{
		fullFileName = folderName + "/" + fileName + "_" + std::to_string(count);
		count++;
	}
	additionallyTakenNames.push_back(fullFileName);
	return fullFileName;
}

std::vector<std::string> BusinessLogicHandler::CreateNewUniqueTopologyName(const std::string& folderName, const std::string& fileName, uint64_t numberOfFiles)
{
	std::list<std::string> uniqueFileNames = ot::ModelServiceAPI::getListOfFolderItems(folderName);
	std::vector<std::string> newNames;
	int count = 1;
	std::string fullFileName = folderName + "/" + fileName;
	for (uint64_t i = 0; i < numberOfFiles; i++)
	{
		while (std::find(uniqueFileNames.begin(), uniqueFileNames.end(), fullFileName) != uniqueFileNames.end())
		{
			fullFileName = folderName + "/" + fileName + "_" + std::to_string(count);
			count++;
		}

		uniqueFileNames.push_back(fullFileName);
		newNames.push_back(fullFileName);
		fullFileName = folderName + "/" + fileName + "_" + std::to_string(count);
	}
		
	return newNames;
}



void BusinessLogicHandler::CheckEssentials()
{
	if (_modelComponent == nullptr || _uiComponent == nullptr)
	{
		throw std::runtime_error("UIComponent or ModelComponent is not connected with DataSourceHandler");
	}
}