// @otlicense
// File: EntityMeshTetData.h
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

#include "EntityContainer.h"
#include "OldTreeIcon.h"

#include <list>
#include <string>

class EntityMeshTetFace;
class EntityMeshTetNodes;
class EntityMeshTetFaceData;
class EntityBinaryData;

class __declspec(dllexport) EntityMeshTetData : public EntityContainer
{
public:
	EntityMeshTetData() : EntityMeshTetData(0, nullptr, nullptr, nullptr) {};
	EntityMeshTetData(ot::UID ID, EntityBase *parent, EntityObserver *obs, ModelState *ms);
	virtual ~EntityMeshTetData();

	virtual bool considerForPropertyFilter(void) override { return true; };
	virtual bool considerChildrenForPropertyFilter(void) override { return false; };

	virtual bool updateFromProperties(void) override;

	virtual bool getEntityBox(double &xmin, double &xmax, double &ymin, double &ymax, double &zmin, double &zmax) override;

	void getNodeCoords(size_t node, double coord[3]);
	double getNodeCoordX(size_t node);
	double getNodeCoordY(size_t node);
	double getNodeCoordZ(size_t node);
	void   setNodeCoordX(size_t node, double x);
	void   setNodeCoordY(size_t node, double y);
	void   setNodeCoordZ(size_t node, double z);

	void setNumberOfNodes(size_t n);

	EntityMeshTetFace *getFace(int faceId);
	void setFace(int faceId, EntityMeshTetFace *face);

	virtual void storeToDataBase(void) override;
	void releaseMeshData(void);

	virtual void addVisualizationNodes(void) override;
	void addVisualizationItem(void);

	virtual std::string getClassName(void) const override { return "EntityMeshTetData"; };

	long long getMeshNodesStorageId(void) { return meshNodesStorageId; };
	long long getMeshFacesStorageId(void) { return meshFacesStorageId; };

	void setGmshDataStorageId(long long id) { gmshDataStorageId = id; };
	long long getGmshDataStorageId(void) { return gmshDataStorageId; };

	EntityMeshTetNodes *getMeshNodes(void) { return meshNodes; };
	EntityMeshTetFaceData *getMeshFaces(void) { return meshFaces; };

	virtual entityType getEntityType(void) const override { return TOPOLOGY; };

	virtual void removeChild(EntityBase *child) override;

	void storeMeshNodes(void);
	void storeMeshFaces(void);

private:
	virtual int getSchemaVersion(void) override { return 1; };
	virtual void addStorageData(bsoncxx::builder::basic::document &storage) override;
	virtual void readSpecificDataFromDataBase(bsoncxx::document::view &doc_view, std::map<ot::UID, EntityBase *> &entityMap) override;
	void EnsureFacesLoaded(void);
	void releaseMeshFaces(void);
	void EnsureNodesLoaded(void);
	void releaseMeshNodes(void);

	EntityMeshTetNodes *meshNodes;
	long long meshNodesStorageId;

	EntityMeshTetFaceData *meshFaces;
	long long meshFacesStorageId;

	long long gmshDataStorageId;
};



