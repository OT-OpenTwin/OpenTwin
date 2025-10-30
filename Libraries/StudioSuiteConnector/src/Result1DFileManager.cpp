// @otlicense
// File: Result1DFileManager.cpp
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

#include "StudioSuiteConnector/Result1DFileManager.h"

#include <cassert>

Result1DFileManager::Result1DFileManager(const std::string& baseDir, std::list<int>& runIds)
{
	// In a first step, we need to recursively read the data for each runid
	for (auto id : runIds)
	{
		std::string runIdDir = baseDir + std::to_string(id);

		Result1DRunIDContainer* container = new Result1DRunIDContainer;
		bool success = container->readData(runIdDir);
		assert(success);

		assert(runIDToDataMap.count(id) == 0);
		runIDToDataMap[id] = container;
	}
}

Result1DFileManager::~Result1DFileManager()
{
	clear();
}

void Result1DFileManager::clear()
{
	for (auto item : runIDToDataMap)
	{
		if (item.second != nullptr)
		{
			delete item.second;
		}
	}

	runIDToDataMap.clear();
}

Result1DRunIDContainer *Result1DFileManager::getRunContainer(int id)
{
	if (runIDToDataMap.count(id) == 0) return nullptr;

	return runIDToDataMap[id];
}
