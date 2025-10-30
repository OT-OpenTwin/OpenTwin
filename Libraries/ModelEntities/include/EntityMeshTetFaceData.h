// @otlicense
// File: EntityMeshTetFaceData.h
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

#include "EntityBase.h"
#include "OldTreeIcon.h"

#include <list>

class EntityMeshTetFace;
class EntityMeshTetInfo;

class __declspec(dllexport) EntityMeshTetFaceData : public EntityBase
{
public:
	EntityMeshTetFaceData() : EntityMeshTetFaceData(0, nullptr, nullptr, nullptr, "") {};
	EntityMeshTetFaceData(ot::UID ID, EntityBase *parent, EntityObserver *obs, ModelState *ms, const std::string &owner);
	virtual ~EntityMeshTetFaceData();

	virtual bool getEntityBox(double &xmin, double &xmax, double &ymin, double &ymax, double &zmin, double &zmax) override;

	EntityMeshTetFace *getFace(int faceId);
	void setFace(int faceId, EntityMeshTetFace *face);

	virtual void storeToDataBase(void) override;

	virtual std::string getClassName(void) const override { return "EntityMeshTetFaceData"; };

	void addMeshFaceStorageIds(EntityMeshTetInfo *meshInfo);

	virtual entityType getEntityType(void) const override { return DATA; };
	virtual void removeChild(EntityBase *child) override;

	std::map<int, EntityMeshTetFace*> getAllFaces(void) { return meshFaces; }

private:
	virtual int getSchemaVersion(void) override { return 1; };
	virtual void addStorageData(bsoncxx::builder::basic::document &storage) override;
	virtual void readSpecificDataFromDataBase(bsoncxx::document::view &doc_view, std::map<ot::UID, EntityBase *> &entityMap) override;
	void EnsureFacesLoaded(void);
	void storeMeshFaces(void);
	void releaseMeshFaces(void);

	std::map<int, EntityMeshTetFace*> meshFaces;
	std::map<EntityBase*, int> meshFacesIndex;
	std::map<int, std::pair<ot::UID, ot::UID>> meshFaceStorageIds;
};



