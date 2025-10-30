// @otlicense
// File: RunIDContainer.h
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

#include "Result1DData.h"

#include <string>
#include <map>
#include <list>

class InfoFileManager;

class RunIDContainer
{
public:
	RunIDContainer();
	~RunIDContainer();

	int readData(size_t& bufferIndex, char* dataBuffer, size_t dataBufferLength);
	void addResult1DInformation(int runID, InfoFileManager& infoFileManager);
	std::map<std::string, double>& getParameters() { return parameterValues; }
	std::list<std::string> getListOfCategories();
	std::map<std::string, Result1DData*> getResultsForCategory(const std::string &category);

private:
	size_t readIntegerFromBuffer(size_t& bufferIndex, char* dataBuffer, size_t dataBufferLength);
	std::string readStringFromBuffer(size_t& bufferIndex, char* dataBuffer, size_t dataBufferLength);
	void processParameterInformation(const std::string& data);
	void clear();
	void readLine(std::stringstream& dataContent, std::string& line);

	std::string metaHashValue;
	std::map<std::string, Result1DData*> fileNameToData;
	std::map<std::string, double> parameterValues;
};
