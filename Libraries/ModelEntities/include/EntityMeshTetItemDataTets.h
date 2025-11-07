// @otlicense
// File: EntityMeshTetItemDataTets.h
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


class __declspec(dllexport) EntityMeshTetItemTet
{
public:
	EntityMeshTetItemTet() {};
	virtual ~EntityMeshTetItemTet() {};

	void setNumberOfNodes(int n) { assert(nodes.empty()); nodes.resize(n); }
	int getNumberOfNodes(void) { return (int)nodes.size(); }

	void   setNode(int i, size_t n) { assert(i < (int)nodes.size()); nodes[i] = n; };
	size_t getNode(int i) { assert(i < (int)nodes.size()); return nodes[i]; };

private:
	std::vector<size_t> nodes;
};

class __declspec(dllexport) EntityMeshTetItemDataTets : public EntityBase
{
public:
	EntityMeshTetItemDataTets() : EntityMeshTetItemDataTets(0, nullptr, nullptr, nullptr) {};
	EntityMeshTetItemDataTets(ot::UID ID, EntityBase *parent, EntityObserver *obs, ModelState *ms);
	virtual ~EntityMeshTetItemDataTets();

	virtual bool getEntityBox(double &xmin, double &xmax, double &ymin, double &ymax, double &zmin, double &zmax) override;

	void setNumberTets(size_t nT) { tets.resize(nT); setModified(); };
	size_t getNumberTets(void) { return tets.size(); };

	void setNumberOfTetNodes(size_t nT, int nNodes) { tets[nT].setNumberOfNodes(nNodes); };
	void setTetNode(size_t nT, int nodeIndex, size_t node) { assert(nT < tets.size()); tets[nT].setNode(nodeIndex, node); setModified(); };
	size_t getTetNode(size_t nT, int nodeIndex) { assert(nT < tets.size()); return tets[nT].getNode(nodeIndex); };

	virtual std::string getClassName(void) const override { return "EntityMeshTetItemDataTets"; };

	virtual entityType getEntityType(void) const override { return DATA; };

private:
	virtual int getSchemaVersion(void) override { return 1; };
	virtual void addStorageData(bsoncxx::builder::basic::document &storage) override;
	virtual void readSpecificDataFromDataBase(bsoncxx::document::view &doc_view, std::map<ot::UID, EntityBase *> &entityMap) override;

	std::vector<EntityMeshTetItemTet> tets;
};



