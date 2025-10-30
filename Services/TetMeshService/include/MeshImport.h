// @otlicense
// File: MeshImport.h
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

class MeshWriter;
class EntityMeshTet;

class MeshImport
{
public:
	MeshImport(Application *app) : application(app) {};
	~MeshImport() {};

	std::string getFileExtensions(void);

	void importMesh(const std::string &meshName, const std::string &originalName, const std::string &content, ot::UID uncompressedDataLength);


private:
	void CreateTmpFileFromCompressedData(std::string fileName, const std::string& data, ot::UID uncompressedDataLength);
	void writeMeshEntities(MeshWriter* meshWriter, EntityMeshTet* mesh, const std::string& materialsFolder, ot::UID materialsFolderID);
	void getNewColor(size_t count, double& colorR, double& colorG, double& colorB);

	Application *application;
	std::string tmpDir;

};
