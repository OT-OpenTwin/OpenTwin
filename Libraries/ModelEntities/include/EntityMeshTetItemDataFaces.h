// @otlicense
// File: EntityMeshTetItemDataFaces.h
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

class __declspec(dllexport) EntityMeshTetItemDataFaces : public EntityBase
{
public:
	EntityMeshTetItemDataFaces() : EntityMeshTetItemDataFaces(0, nullptr, nullptr, nullptr) {};
	EntityMeshTetItemDataFaces(ot::UID ID, EntityBase *parent, EntityObserver *obs, ModelState *ms);
	virtual ~EntityMeshTetItemDataFaces();

	virtual bool getEntityBox(double &xmin, double &xmax, double &ymin, double &ymax, double &zmin, double &zmax) override;

	void setNumberFaces(size_t nT) { faces.resize(nT); setModified(); };
	size_t getNumberFaces(void) { return faces.size(); };
	int getFaceId(size_t nF) { return faces[nF]; };
	void setFace(size_t nF, int faceId) { faces[nF] = faceId; setModified(); };

	virtual std::string getClassName(void) const override { return "EntityMeshTetItemDataFaces"; };

	virtual entityType getEntityType(void) const override { return DATA; };

private:
	virtual int getSchemaVersion(void) override { return 1; };
	virtual void addStorageData(bsoncxx::builder::basic::document &storage) override;
	virtual void readSpecificDataFromDataBase(const bsoncxx::document::view &doc_view, std::map<ot::UID, EntityBase *> &entityMap) override;

	std::vector<int> faces;
};



