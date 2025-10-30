// @otlicense
// File: Result1DRunIDContainer.h
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
#include <map>
#include <vector>

#include "StudioSuiteConnector/Result1DFileData.h"

class InfoFileManager;

class Result1DRunIDContainer
{
public:
	Result1DRunIDContainer() : totalSize(0) {};
	~Result1DRunIDContainer();

	bool readData(const std::string& dataDir);
	void writeToBuffer(size_t runId, std::vector<char>& buffer);

	size_t getSize();
	size_t getNumberFiles();
	std::string getMetaDataHash();

	bool hasChanged(int runID, InfoFileManager& infoFileManager);

private:
	void calculateMetaDataHash();
	void calculateSize(); 
	void writeIntegerToBuffer(size_t value, std::vector<char>& buffer);
	void writeStringToBuffer(std::string value, std::vector<char>& buffer);

	std::map<std::string, Result1DFileData*> fileNameToDataMap;
	std::string metaHashValue;
	size_t totalSize;
};

