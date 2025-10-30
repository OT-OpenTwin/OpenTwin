// @otlicense
// File: Result1DManager.cpp
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

#include "Result1DManager.h"

#include <stdexcept>

Result1DManager::Result1DManager(char* dataBuffer, size_t dataBufferLength)
{
	size_t bufferIndex = 0;

	// First, we read the number of runids from the buffer
	size_t numberRunIds = readIntegerFromBuffer(bufferIndex, dataBuffer, dataBufferLength);

	for (size_t runIndex = 0; runIndex < numberRunIds; runIndex++)
	{
		RunIDContainer* container = new RunIDContainer();
		int runID = container->readData(bufferIndex, dataBuffer, dataBufferLength);

		runIDtoContainerMap[runID] = container;
	}
}

Result1DManager::~Result1DManager()
{
	for (auto item : runIDtoContainerMap)
	{
		delete item.second;
	}

	runIDtoContainerMap.clear();
}

size_t Result1DManager::readIntegerFromBuffer(size_t &bufferIndex, char *dataBuffer, size_t dataBufferLength)
{
	size_t value;
	char* valueBuffer = (char*)(&value);

	for (size_t index = 0; index < sizeof(size_t); index++)
	{
		if (bufferIndex == dataBufferLength) throw std::overflow_error("buffer overflow when reading data");

		valueBuffer[index] = dataBuffer[bufferIndex];
		bufferIndex++;
	}

	return value;
}

void Result1DManager::addResult1DInformation(InfoFileManager& infoFileManager)
{
	for (auto item : runIDtoContainerMap)
	{
		item.second->addResult1DInformation(item.first, infoFileManager);
	}
}

std::list<int> Result1DManager::getRunIDList()
{
	std::list<int> runIDs;

	for (auto item : runIDtoContainerMap)
	{
		runIDs.push_back(item.first);
	}

	runIDs.sort();

	return runIDs;
}

RunIDContainer* Result1DManager::getContainer(int runID)
{
	auto container = runIDtoContainerMap.find(runID);

	if (container == runIDtoContainerMap.end())
	{
		return nullptr;
	}

	return container->second;
}

