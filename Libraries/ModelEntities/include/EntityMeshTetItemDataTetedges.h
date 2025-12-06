// @otlicense
// File: EntityMeshTetItemDataTetedges.h
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
#include <vector>

class __declspec(dllexport) EntityMeshTetItemTetEdge
{
public:
	EntityMeshTetItemTetEdge() : nodes{ 0, 0 } {};
	virtual ~EntityMeshTetItemTetEdge() {};

	void setNodes(size_t n[2]) { nodes[0] = n[0]; nodes[1] = n[1]; };
	void getNodes(size_t n[2]) { n[0] = nodes[0]; n[1] = nodes[1]; };

	size_t getNode(int index) { return nodes[index]; }
	void setNode(int index, size_t node) { nodes[index] = node; }

private:
	size_t nodes[2];
};

class __declspec(dllexport) EntityMeshTetItemDataTetedges : public EntityBase
{
public:
	EntityMeshTetItemDataTetedges() : EntityMeshTetItemDataTetedges(0, nullptr, nullptr, nullptr) {};
	EntityMeshTetItemDataTetedges(ot::UID ID, EntityBase *parent, EntityObserver *obs, ModelState *ms);
	virtual ~EntityMeshTetItemDataTetedges();

	virtual bool getEntityBox(double &xmin, double &xmax, double &ymin, double &ymax, double &zmin, double &zmax) override;

	void setNumberEdges(size_t nT) { edges.resize(nT); setModified(); };
	size_t getNumberEdges(void) { return edges.size(); };

	void setEdgeNodes(size_t nE, size_t n[2]) { assert(nE < edges.size()); edges[nE].setNodes(n); setModified(); };
	void getEdgeNodes(size_t nE, size_t n[2]) { assert(nE < edges.size()); edges[nE].getNodes(n); };

	virtual std::string getClassName(void) const override { return "EntityMeshTetItemDataTetedges"; };

	virtual entityType getEntityType(void) const override { return DATA; };

private:
	virtual int getSchemaVersion(void) override { return 1; };
	virtual void addStorageData(bsoncxx::builder::basic::document &storage) override;
	virtual void readSpecificDataFromDataBase(const bsoncxx::document::view &doc_view, std::map<ot::UID, EntityBase *> &entityMap) override;

	std::vector<EntityMeshTetItemTetEdge> edges;
};



