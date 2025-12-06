// @otlicense
// File: EntityCartesianVector.cpp
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


#include "DataBase.h"
#include "EntityCartesianVector.h"
#include "EntityMeshCartesianData.h"

#include <bsoncxx/builder/basic/array.hpp>

static EntityFactoryRegistrar<EntityCartesianVector> registrar(EntityCartesianVector::className());

EntityCartesianVector::EntityCartesianVector(ot::UID ID, EntityBase *parent, EntityObserver *obs, ModelState *ms) :
	EntityCompressedVector(ID, parent, obs, ms)
{
}

EntityCartesianVector::~EntityCartesianVector()
{
}

bool EntityCartesianVector::getEntityBox(double &xmin, double &xmax, double &ymin, double &ymax, double &zmin, double &zmax)
{
	return false;
}

void EntityCartesianVector::storeToDataBase(void)
{
	EntityCompressedVector::storeToDataBase();
}

void EntityCartesianVector::addStorageData(bsoncxx::builder::basic::document &storage)
{
	// We store the parent class information first 
	EntityCompressedVector::addStorageData(storage);

	// Now add the actual cartesian vector data

	storage.append(
		bsoncxx::builder::basic::kvp("resultType", (long long)getResultType())
	);
}

void EntityCartesianVector::readSpecificDataFromDataBase(const bsoncxx::document::view &doc_view, std::map<ot::UID, EntityBase *> &entityMap)
{
	// We read the parent class information first 
	EntityCompressedVector::readSpecificDataFromDataBase(doc_view, entityMap);

	// Now read the cartesian vector data

	setResultType(static_cast<tResultType>((long long) doc_view["resultType"].get_int64()));

	resetModified();
}

void EntityCartesianVector::removeChild(EntityBase *child)
{
	EntityCompressedVector::removeChild(child);
}

