// @otlicense
// File: EntityMeshCartesianItem.h
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
#include "Geometry.h"
#include "BoundingBox.h"
#include "OldTreeIcon.h"

class EntityMeshCartesianData;

#include <list>

//! The Entity class is the base class for model entities and provides basic access properties to model entities. 
class __declspec(dllexport) EntityMeshCartesianItem : public EntityBase
{
public:
	EntityMeshCartesianItem() : EntityMeshCartesianItem(0, nullptr, nullptr, nullptr, "") {};
	EntityMeshCartesianItem(ot::UID ID, EntityBase *parent, EntityObserver *obs, ModelState *ms, const std::string &owner);
	virtual ~EntityMeshCartesianItem();

	virtual bool getEntityBox(double &xmin, double &xmax, double &ymin, double &ymax, double &zmin, double &zmax) override;

	void setMesh(EntityMeshCartesianData *m) { mesh = m; };

	void setNumberFaces(size_t nT) { faces.resize(nT); setModified(); };

	int getFaceId(size_t nF) { return faces[nF]; };
	void setFace(size_t nF, int faceId) { faces[nF] = faceId; setModified(); };

	bool getFaceOrientation(size_t nF) { return getFaceId(nF) > 0; };
 
	//EntityMeshTetFace *getFace(size_t nF);

	virtual void storeToDataBase(void) override;

	virtual void addVisualizationNodes(void) override;

	void addVisualizationItem(bool isHidden);

	void setColor(double r, double g, double b) { colorRGB[0] = r; colorRGB[1] = g; colorRGB[2] = b; };

	virtual std::string getClassName(void) const override { return "EntityMeshCartesianItem"; };

	virtual entityType getEntityType(void) const override { return TOPOLOGY; };

	BoundingBox &getBoundingBox(void) { return boundingBox; }

private:

	virtual int getSchemaVersion(void) override { return 1; };
	virtual void addStorageData(bsoncxx::builder::basic::document &storage) override;
	virtual void readSpecificDataFromDataBase(bsoncxx::document::view &doc_view, std::map<ot::UID, EntityBase *> &entityMap) override;

	EntityMeshCartesianData *mesh;

	double colorRGB[3];

	std::vector<int> faces;

	BoundingBox boundingBox;
};



