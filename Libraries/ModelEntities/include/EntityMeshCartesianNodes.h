// @otlicense
// File: EntityMeshCartesianNodes.h
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

class __declspec(dllexport) EntityMeshCartesianNodes : public EntityBase
{
public:
	EntityMeshCartesianNodes() : EntityMeshCartesianNodes(0, nullptr, nullptr, nullptr) {};
	EntityMeshCartesianNodes(ot::UID ID, EntityBase *parent, EntityObserver *obs, ModelState *ms);
	virtual ~EntityMeshCartesianNodes();

	virtual bool getEntityBox(double &xmin, double &xmax, double &ymin, double &ymax, double &zmin, double &zmax) override;

	double getNodeCoordX(size_t node) { assert(node < nodeCoordsX.size()); return nodeCoordsX[node]; };
	double getNodeCoordY(size_t node) { assert(node < nodeCoordsY.size()); return nodeCoordsY[node]; };
	double getNodeCoordZ(size_t node) { assert(node < nodeCoordsZ.size()); return nodeCoordsZ[node]; };
	void   setNodeCoordX(size_t node, double x) { assert(node < nodeCoordsX.size()); nodeCoordsX[node] = x; setModified(); };
	void   setNodeCoordY(size_t node, double y) { assert(node < nodeCoordsY.size()); nodeCoordsY[node] = y; setModified(); };
	void   setNodeCoordZ(size_t node, double z) { assert(node < nodeCoordsZ.size()); nodeCoordsZ[node] = z; setModified(); };

	void setNumberOfNodes(size_t n);
	size_t getNumberOfNodes(void) { return nodeCoordsX.size(); }

	virtual std::string getClassName(void) const override { return "EntityMeshCartesianNodes"; };

	virtual entityType getEntityType(void) const override { return DATA; };

private:
	virtual int getSchemaVersion(void) override { return 1; };
	virtual void addStorageData(bsoncxx::builder::basic::document &storage) override;
	virtual void readSpecificDataFromDataBase(bsoncxx::document::view &doc_view, std::map<ot::UID, EntityBase *> &entityMap) override;

	std::vector<double> nodeCoordsX;
	std::vector<double> nodeCoordsY;
	std::vector<double> nodeCoordsZ;
};



