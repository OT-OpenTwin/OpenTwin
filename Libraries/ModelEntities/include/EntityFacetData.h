// @otlicense
// File: EntityFacetData.h
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
#include "OldTreeIcon.h"

#include <list>
#include <vector>

class __declspec(dllexport) EntityFacetData : public EntityBase
{
public:
	EntityFacetData() : EntityFacetData(0, nullptr, nullptr, nullptr) {};
	EntityFacetData(ot::UID ID, EntityBase *parent, EntityObserver *obs, ModelState *ms);
	virtual ~EntityFacetData();

	virtual bool getEntityBox(double &xmin, double &xmax, double &ymin, double &ymax, double &zmin, double &zmax) override {return false;};

	void resetErrors(void) { errors.clear(); };

	std::vector<Geometry::Node>   &getNodeVector(void)   { return nodes; };
	std::list<Geometry::Triangle> &getTriangleList(void) { return triangles; };
	std::list<Geometry::Edge>     &getEdgeList(void)     { return edges; };
	std::string					  &getErrorString(void)  { return errors; };
	std::map<ot::UID, std::string> &getFaceNameMap(void) { return faceNameMap; };

	static std::string className() { return "EntityFacetData"; };
	virtual std::string getClassName(void) const override { return EntityFacetData::className(); };

	virtual entityType getEntityType(void) const override { return DATA; };

private:
	virtual int getSchemaVersion(void) override { return 1; };
	virtual void addStorageData(bsoncxx::builder::basic::document &storage) override;
	virtual void readSpecificDataFromDataBase(const bsoncxx::document::view &doc_view, std::map<ot::UID, EntityBase *> &entityMap) override;

	std::vector<Geometry::Node> nodes;
	std::list<Geometry::Triangle> triangles;
	std::list<Geometry::Edge> edges;
	std::map<ot::UID, std::string> faceNameMap;
	std::string errors;
};



