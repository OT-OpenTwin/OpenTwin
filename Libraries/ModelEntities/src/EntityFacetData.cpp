// @otlicense
// File: EntityFacetData.cpp
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


#include "EntityFacetData.h"
#include "DataBase.h"
#include "OldTreeIcon.h"

#include <bsoncxx/builder/basic/array.hpp>

static EntityFactoryRegistrar<EntityFacetData> registrar(EntityFacetData::className());

EntityFacetData::EntityFacetData(ot::UID ID, EntityBase *parent, EntityObserver *obs, ModelState *ms, const std::string &owner) :
	EntityBase(ID, parent, obs, ms, owner)
{

}

EntityFacetData::~EntityFacetData()
{

}

void EntityFacetData::addStorageData(bsoncxx::builder::basic::document &storage)
{
	// We store the parent class information first 
	EntityBase::addStorageData(storage);

	// Store the facet information
	storage.append(
		bsoncxx::builder::basic::kvp("NumberOfNodes", (long long) nodes.size()),
		bsoncxx::builder::basic::kvp("NumberOfTriangles", (long long) triangles.size()),
		bsoncxx::builder::basic::kvp("NumberOfEdges", (long long) edges.size()),
		bsoncxx::builder::basic::kvp("Nodes", Geometry::getBSON(nodes)),
		bsoncxx::builder::basic::kvp("Triangles", Geometry::getBSON(triangles)),
		bsoncxx::builder::basic::kvp("Edges", Geometry::getBSON(edges)),
		bsoncxx::builder::basic::kvp("FaceNames", Geometry::getBSON(faceNameMap)),
		bsoncxx::builder::basic::kvp("Errors", errors)
	);
}

void EntityFacetData::readSpecificDataFromDataBase(bsoncxx::document::view &doc_view, std::map<ot::UID, EntityBase *> &entityMap)
{
	// We read the parent class information first 
	EntityBase::readSpecificDataFromDataBase(doc_view, entityMap);

	// Now we read the information about the facet data

	bsoncxx::document::view nodesObj      = doc_view["Nodes"].get_document().view();
	bsoncxx::document::view  trianglesObj = doc_view["Triangles"].get_document().view();
	bsoncxx::document::view  edgesObj = doc_view["Edges"].get_document().view();

	errors = doc_view["Errors"].get_utf8().value.data();

	Geometry::readBSON(nodesObj,     nodes);
	Geometry::readBSON(trianglesObj, triangles);
	Geometry::readBSON(edgesObj, edges);

	faceNameMap.clear();
	try 
	{
		bsoncxx::document::view  faceNamesObj = doc_view["FaceNames"].get_document().view();
		Geometry::readBSON(faceNamesObj, faceNameMap);
	}
	catch (std::exception)
	{

	}

	resetModified();
}
