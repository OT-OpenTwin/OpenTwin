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

#include <string>
#include <map>
#include <list>
#include <sstream>

#include "OTCore/CoreTypes.h"

class Application;

class InfoFileManager
{
public:
	InfoFileManager() : application(nullptr), hasChanged(false), infoEntityID(0), infoEntityVersion(0) {};
	~InfoFileManager() {};

	void setData(Application* app);
	void readInformation();
	void getShapes(std::map<std::string, bool>& shapes);
	void setShapeHash(const std::string& name, const std::string& hash);
	void deleteShape(const std::string& name);
	void writeInformation();
	void addDeletedShapesToList(std::list<std::string>& list);
	ot::UID getInfoEntityID() { return infoEntityID; }
	ot::UID getInfoEntityVersion() { return infoEntityVersion; }
	void clearResult1D(void);
	void setRunIDMetaHash(int runID, const std::string& hash);
	void setRunIDFileHash(int runID, const std::string& fileName, const std::string& hash);

private:
	void writeResult1DInformation(std::stringstream& dataContent);

	Application* application;
	bool hasChanged;
	std::map<std::string, std::string> shapeHashMap;
	std::list<std::string> deletedShapes;
	ot::UID infoEntityID;
	ot::UID infoEntityVersion;
	std::map<int, std::string> runIdMetaHash;
	std::map<int, std::map<std::string, std::string>> runIdToFileNameToHash;
};
