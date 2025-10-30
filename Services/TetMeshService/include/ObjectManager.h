// @otlicense
// File: ObjectManager.h
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

class Application;
class MeshWriter;
class FaceAnnotationsManager;
class ProgressLogger;

class EntityBase;
class EntityMeshTet;

#include <vector>
#include <map>
#include <string>

#include "OldTreeIcon.h"

namespace gmsh
{
	typedef std::vector<std::pair<int, int> > vectorpair;
}

class ObjectManager
{
public:
	ObjectManager(Application *app) : application(app) {};
	~ObjectManager() {};

	void storeMeshElementsForEntity(EntityBase *entity, gmsh::vectorpair &meshElements);
	gmsh::vectorpair &getMeshElementsForEntity(EntityBase *entity);

	bool hasAnyVolumes(void) { return !volumeTagToFacesMap.empty(); }

	void writeMeshEntities(MeshWriter *meshWriter, FaceAnnotationsManager *faceAnnotationsManager, ProgressLogger *progressLogger, EntityMeshTet *mesh,
						   const std::string& materialsFolder, ot::UID materialsFolderID);

private:
	void storeVolumeFaces(EntityBase *entity, gmsh::vectorpair &meshElements);

	Application *application;
	std::map<std::string, gmsh::vectorpair> entityNameToMeshElementsMap;
	std::map<std::string, EntityBase *>     nameToEntityMap;
	std::map<int, gmsh::vectorpair>         volumeTagToFacesMap;
	std::string								backgroundMeshEntityName;
};
