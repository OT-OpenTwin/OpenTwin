// @otlicense
// File: EntityMeshTet.h
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
#pragma warning(disable : 4251)

#include "EntityMesh.h"

#include <list>
#include <map>

class EntityMeshTetData;
class EntityMeshTetFace;

class __declspec(dllexport) EntityMeshTet : public EntityMesh
{
public:
	EntityMeshTet() : EntityMeshTet(0, nullptr, nullptr, nullptr) {};
	EntityMeshTet(ot::UID ID, EntityBase *parent, EntityObserver *obs, ModelState *ms);
	virtual ~EntityMeshTet();

	void createProperties(const std::string materialsFolder, ot::UID materialsFolderID);

	virtual bool updateFromProperties(void) override;

	virtual bool considerForPropertyFilter(void) override { return true; };
	virtual bool considerChildrenForPropertyFilter(void) override { return false; };

	void   getNodeCoords(size_t node, double coord[3]);
	double getNodeCoordX(size_t node);
	double getNodeCoordY(size_t node);
	double getNodeCoordZ(size_t node);
	void   setNodeCoordX(size_t node, double x);
	void   setNodeCoordY(size_t node, double y);
	void   setNodeCoordZ(size_t node, double z);

	void setNumberOfNodes(size_t n);

	EntityMeshTetFace *getFace(int faceId);
	void setFace(int faceId, EntityMeshTetFace *face);

	EntityMeshTetData *getMeshData(void);

	virtual void storeToDataBase(void) override;

	virtual std::string getClassName(void) const override { return "EntityMeshTet"; };
	
	virtual entityType getEntityType(void) const override { return TOPOLOGY; };
	virtual void removeChild(EntityBase *child) override;

	bool getMeshValid(void) { return meshValid; }
	void setMeshValid(bool valid) { meshValid = valid; }

	void deleteMeshData(void);

	void storeMeshData(void);
	void releaseMeshData(void);

	long long getMeshDataStorageId(void) { return meshDataStorageId; }

	bool updatePropertyVisibilities(void);

private:
	void EnsureMeshDataLoaded(void);
	virtual int getSchemaVersion(void) override { return 1; };
	virtual void addStorageData(bsoncxx::builder::basic::document &storage) override;
	virtual void readSpecificDataFromDataBase(const bsoncxx::document::view &doc_view, std::map<ot::UID, EntityBase *> &entityMap) override;
	void recursivelySetMesh(const std::list<EntityBase *> &childList);

	EntityMeshTetData *meshData;
	long long meshDataStorageId;

	bool meshValid;
};



