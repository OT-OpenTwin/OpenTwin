// @otlicense
// File: IndexHandler.h
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
#include <string>
#include "ResultImportLogger.h"
#include "ResultDataStorageAPI.h"
#include "OTServiceFoundation/UiComponent.h"
class __declspec(dllexport) IndexHandler
{
public:
	IndexHandler(const std::string& _collectionName);
	
	void createDefaultIndexes();

	const static std::vector<std::string>& getDefaultIndexes() 
	{ 
		static std::vector<std::string> m_defaultIndexes = { "Quantity" ,"Series"};
		return m_defaultIndexes;
	}

	//! @brief Drop all indexes in the collection except for the default _id index.
	void dropAllIndexes();	

private:
	DataStorageAPI::ResultDataStorageAPI m_dataStorageAccess;
	std::vector<bool> m_defaultIndexesSet = { false,false };

	bool checkIfDefaultIndexesAreSet();
};
