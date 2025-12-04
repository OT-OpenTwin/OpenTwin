// @otlicense
// File: EntityMeshTetItemDataFaces.cpp
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


#include "EntityMeshTetItemDataFaces.h"
#include "DataBase.h"
#include "OldTreeIcon.h"

#include <bsoncxx/builder/basic/array.hpp>

static EntityFactoryRegistrar<EntityMeshTetItemDataFaces> registrar("EntityMeshTetItemDataFaces");

EntityMeshTetItemDataFaces::EntityMeshTetItemDataFaces(ot::UID ID, EntityBase *parent, EntityObserver *obs, ModelState *ms) :
	EntityBase(ID, parent, obs, ms)
{
	
}

EntityMeshTetItemDataFaces::~EntityMeshTetItemDataFaces()
{

}

bool EntityMeshTetItemDataFaces::getEntityBox(double &xmin, double &xmax, double &ymin, double &ymax, double &zmin, double &zmax)
{
	return false;
}

void EntityMeshTetItemDataFaces::addStorageData(bsoncxx::builder::basic::document &storage)
{
	// We store the parent class information first 
	EntityBase::addStorageData(storage);

	// Add the specific data of the mesh item (face list)

	auto f = bsoncxx::builder::basic::array();

	for (auto face : faces)
	{
		f.append((int) face);
	}

	storage.append(bsoncxx::builder::basic::kvp("Faces", f));
}

void EntityMeshTetItemDataFaces::readSpecificDataFromDataBase(const bsoncxx::document::view &doc_view, std::map<ot::UID, EntityBase *> &entityMap)
{
	// We read the parent class information first 
	EntityBase::readSpecificDataFromDataBase(doc_view, entityMap);

	// Now we read the information about the mesh item (face list)

	auto face = doc_view["Faces"].get_array().value;

	size_t numberFaces = std::distance(face.begin(), face.end());

	auto f = face.begin();

	setNumberFaces(numberFaces);

	for (unsigned long findex = 0; findex < numberFaces; findex++)
	{
		setFace(findex, (int) DataBase::getIntFromArrayViewIterator(f));
		f++;
	}

	resetModified();
}
