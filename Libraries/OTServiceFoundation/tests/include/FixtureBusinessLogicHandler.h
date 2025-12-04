// @otlicense
// File: FixtureBusinessLogicHandler.h
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
#include <gtest/gtest.h>
#include "OTServiceFoundation/BusinessLogicHandler.h"

class FixtureBusinessLogicHandler : public testing::Test
{
public:
	std::vector<std::string> SetAnother5Names(std::list<std::string>& alreadySetNames)
	{
		return _handler.CreateNewUniqueTopologyNames(alreadySetNames, _folderName, _fileName, 5);
	}

	std::vector<std::string> SetAnother5NamesStartWith7(std::list<std::string>& alreadySetNames)
	{
		return _handler.CreateNewUniqueTopologyNames(alreadySetNames, _folderName, _fileName, 5,7);
	}

	std::vector<std::string> SetAnother5NamesAlwaysNumbered(std::list<std::string>& alreadySetNames)
	{
		return _handler.CreateNewUniqueTopologyNames(alreadySetNames, _folderName, _fileName, 5, 1,true);
	}


	std::string getFileName()
	{
		return "File";
	} 
	std::string getFolderName() 
	{
		return "Folder";
	}
private:
	BusinessLogicHandler _handler;
	const std::string _folderName = "Folder";
	const std::string _fileName = "File";

};
