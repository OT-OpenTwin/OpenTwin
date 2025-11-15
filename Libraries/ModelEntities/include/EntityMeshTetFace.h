// @otlicense
// File: EntityMeshTetFace.h
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
#include "BoundingBox.h"
#include "EntityBase.h"

#include <list>

class __declspec(dllexport) EntityMeshTetTriangle
{
public:
	EntityMeshTetTriangle() {};
	virtual ~EntityMeshTetTriangle() {};

	void setNumberOfNodes(int n) { assert(nodes.empty());  nodes.resize(n); }
	int getNumberOfNodes(void) { return (int) nodes.size(); }

	void   setNode(int i, size_t n) { assert(i < (int) nodes.size()); nodes[i] = n;};
	size_t getNode(int i) { assert(i < (int) nodes.size()); return nodes[i]; };

private:
	std::vector<size_t> nodes;
};

class __declspec(dllexport) EntityMeshTetFace : public EntityBase
{
public:
	EntityMeshTetFace() : EntityMeshTetFace(0, nullptr, nullptr, nullptr) {};
	EntityMeshTetFace(ot::UID ID, EntityBase *parent, EntityObserver *obs, ModelState *ms);
	virtual ~EntityMeshTetFace() {};

	void setNumberTriangles(size_t nT) { triangles.resize(nT); setModified(); };
	size_t getNumberTriangles(void) { return triangles.size(); };

	EntityMeshTetTriangle &getTriangle(size_t n) { assert(n < triangles.size()); return triangles[n]; };
	BoundingBox &getBoundingBox(void) { return boundingBox; };
	void setBoundingBox(const BoundingBox &box) { boundingBox = box; setModified(); };

	virtual bool getEntityBox(double &xmin, double &xmax, double &ymin, double &ymax, double &zmin, double &zmax) override;

	void setSurfaceId(int id) { surfaceId = id; setModified(); };
	int  getSurfaceId(void) { return surfaceId; };

	void addAnnotation(bool orientationForward, ot::UID id) {faceAnnotationIDs.push_back(std::make_pair(orientationForward, id)); setModified();};
	size_t getNumberAnnotations(void) { return faceAnnotationIDs.size(); };
	void getAnnotation(int index, bool &orientationForward, ot::UID &annotationEntityID) { orientationForward = faceAnnotationIDs[index].first; annotationEntityID = faceAnnotationIDs[index].second; };
	
	virtual std::string getClassName(void) const override { return "EntityMeshTetFace"; };

	virtual entityType getEntityType(void) const override { return DATA; };

private:
	virtual int getSchemaVersion(void) override { return 1; };
	virtual void addStorageData(bsoncxx::builder::basic::document &storage) override;
	virtual void readSpecificDataFromDataBase(bsoncxx::document::view &doc_view, std::map<ot::UID, EntityBase *> &entityMap) override;

	bsoncxx::document::value getBSON(std::vector<EntityMeshTetTriangle> &triangles);
	bsoncxx::document::value getBSON(std::vector<std::pair<bool, ot::UID>> &faceAnnotationIDs);
	void readBSON(bsoncxx::document::view &trianglesObj, std::vector<EntityMeshTetTriangle> &triangles);
	void readBSON(bsoncxx::document::view &annotationObj, std::vector<std::pair<bool, ot::UID>> &faceAnnotationIDs);

	int surfaceId;
	std::vector<EntityMeshTetTriangle> triangles;
	std::vector<std::pair<bool, ot::UID>> faceAnnotationIDs;
	BoundingBox boundingBox;
};
