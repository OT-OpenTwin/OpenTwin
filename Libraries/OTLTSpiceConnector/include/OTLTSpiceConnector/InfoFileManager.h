// @otlicense
// File: InfoFileManager.h
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

#include "OTCore/CoreTypes.h"

#include <string>
#include <map>

class InfoFileManager
{
public:
	InfoFileManager(ot::UID infoEntityID, ot::UID infoEntityVersion);
	InfoFileManager() = delete;
	~InfoFileManager() {};

	std::string getTriangleHash(const std::string& shapeName);
	std::string getRunIDMetaHash(int runID);
	std::string getRunIDFileHash(int runID, const std::string& fileName);
	size_t getNumberOfFilesForRunID(int runID);

private:
	std::map<std::string, std::string> triangleHashMap;
	std::map<int, std::string> runIdMetaHash;
	std::map<int, std::map<std::string, std::string>> runIdToFileNameToHash;
};