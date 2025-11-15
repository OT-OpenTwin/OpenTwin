// @otlicense
// File: EntityMeshCartesianFaceList.h
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

#include <list>

class EntityMeshCartesianFace;

class __declspec(dllexport) EntityMeshCartesianFaceList : public EntityBase
{
public:
	EntityMeshCartesianFaceList() : EntityMeshCartesianFaceList(0, nullptr, nullptr, nullptr) {};
	EntityMeshCartesianFaceList(ot::UID ID, EntityBase *parent, EntityObserver *obs, ModelState *ms);
	virtual ~EntityMeshCartesianFaceList();

	static std::string className() { return "EntityMeshCartesianFaceList"; }
	virtual std::string getClassName(void) const override { return EntityMeshCartesianFaceList::className(); }

	virtual bool getEntityBox(double &xmin, double &xmax, double &ymin, double &ymax, double &zmin, double &zmax) override;

	EntityMeshCartesianFace *getFace(int faceId);
	void setFace(int faceId, EntityMeshCartesianFace *face);

	virtual void storeToDataBase(void) override;

	virtual entityType getEntityType(void) const override { return DATA; };
	virtual void removeChild(EntityBase *child) override;

private:
	virtual int getSchemaVersion(void) override { return 1; };
	virtual void addStorageData(bsoncxx::builder::basic::document &storage) override;
	virtual void readSpecificDataFromDataBase(bsoncxx::document::view &doc_view, std::map<ot::UID, EntityBase *> &entityMap) override;
	void EnsureFacesLoaded(void);
	void storeMeshFaces(void);
	void releaseMeshFaces(void);

	std::map<int, EntityMeshCartesianFace*> meshFaces;
	std::map<EntityBase*, int> meshFacesIndex;
	std::map<int, std::pair<ot::UID, ot::UID>> meshFaceStorageIds;
};



