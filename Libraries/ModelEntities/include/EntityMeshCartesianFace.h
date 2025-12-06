// @otlicense
// File: EntityMeshCartesianFace.h
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
#include "BoundingBox.h"
#include "EntityContainer.h"

#include <list>

class __declspec(dllexport) EntityMeshCartesianFace : public EntityBase
{
public:
	enum eType {INDEX_BASED = 0, POINT_BASED = 1};

	EntityMeshCartesianFace() : EntityMeshCartesianFace(0, nullptr, nullptr, nullptr) {};
	EntityMeshCartesianFace(ot::UID ID, EntityBase *parent, EntityObserver *obs, ModelState *ms);
	virtual ~EntityMeshCartesianFace() {};

	void setType(eType t) { type = t; }
	eType getType() { return type; }

	// Access the index based data
	void setNumberCellFaces(int direction, size_t nT);
	size_t getNumberCellFaces(int direction);
	long long getCellFace(int direction, size_t n) { assert(direction >= 0 && direction < 3); assert(n < cellFaces[direction].size()); return cellFaces[direction][n]; };
	void setCellFace(int direction, size_t n, long long face) { assert(direction >= 0 && direction < 3); assert(n < cellFaces[direction].size()); cellFaces[direction][n] = face; };

	// Access the point based data
	void setNumberCellFaces(size_t nT);
	size_t getNumberCellFaces(void);
	void setCellFace(size_t n, long long p0, long long p1, long long p2, long long p3) { assert(n < pointIndices[0].size()); pointIndices[0][n] = p0; pointIndices[1][n] = p1; pointIndices[2][n] = p2; pointIndices[3][n] = p3; };

	BoundingBox &getBoundingBox(void) { return boundingBox; };
	void setBoundingBox(const BoundingBox &box) { boundingBox = box; setModified(); };

	virtual bool getEntityBox(double &xmin, double &xmax, double &ymin, double &ymax, double &zmin, double &zmax) override;

	void setSurfaceId(int id) { surfaceId = id; setModified(); };
	int  getSurfaceId(void) { return surfaceId; };
	
	virtual std::string getClassName(void) const override { return "EntityMeshCartesianFace"; };

	virtual entityType getEntityType(void) const override { return DATA; };

private:
	virtual int getSchemaVersion(void) override { return 1; };
	virtual void addStorageData(bsoncxx::builder::basic::document &storage) override;
	virtual void readSpecificDataFromDataBase(const bsoncxx::document::view &doc_view, std::map<ot::UID, EntityBase *> &entityMap) override;
	void readCellFaces(const bsoncxx::document::view &doc_view, int direction, const std::string &itemName);
	void readPoints(const bsoncxx::document::view &doc_view, int index, const std::string &itemName);

	eType type;
	int surfaceId;
	std::vector<long long> cellFaces[3];
	std::vector<long long> pointIndices[4];
	BoundingBox boundingBox;
};
