// @otlicense
// File: Result1DManager.h
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

#include <map>
#include <list>

#include "RunIDContainer.h"

class InfoFileManager;

class Result1DManager
{
public:
	Result1DManager(char* dataBuffer, size_t dataBufferLength);
	Result1DManager() = delete;
	~Result1DManager();

	void addResult1DInformation(InfoFileManager& infoFileManager);
	std::list<int> getRunIDList();
	
	RunIDContainer* getContainer(int runID);

private:
	size_t readIntegerFromBuffer(size_t &bufferIndex, char* dataBuffer, size_t dataBufferLength);

	std::map<int, RunIDContainer*> runIDtoContainerMap;
};
