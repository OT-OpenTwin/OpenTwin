// @otlicense
// File: MeshExport.h
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

#include "OldTreeIcon.h"

#include <string>

class Application;
class ModelComponent;

class MeshExport
{
public:
	MeshExport(Application *app) : application(app) {};
	~MeshExport() {};

	std::string getFileExtensions(void);
	void getExportFileContent(const std::string &extension, std::string &fileContent, unsigned long long &uncompressedDataLength);

	bool loadMesh(ot::UID currentMeshDataID);
	void cleanUp(void);

private:
	void ReadFileContent(const std::string &fileName, std::string &fileContent, unsigned long long &uncompressedDataLength);
	ot::UID  getGmshDataStorageID(ot::UID currentMeshDataID);
	void loadGmshData(ot::UID gmshDataStorageID, std::vector<char> &meshContent);

	Application *application;
	std::string tmpDir;

};
