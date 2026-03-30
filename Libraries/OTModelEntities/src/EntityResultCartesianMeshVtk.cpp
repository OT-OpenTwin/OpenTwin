// @otlicense
// File: EntityResultCartesianMeshVtk.cpp
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
#include "OTModelEntities/EntityResultCartesianMeshVtk.h"
#include "OTModelEntities/DataBase.h"

static EntityFactoryRegistrar<EntityResultCartesianMeshVtk> registrar("EntityResultCartesianMeshVtk");

EntityResultCartesianMeshVtk::EntityResultCartesianMeshVtk(ot::UID ID, EntityBase* parent, EntityObserver* obs, ModelState* ms)
	:EntityBase(ID, parent, obs, ms)
{}

EntityResultCartesianMeshVtk::~EntityResultCartesianMeshVtk()
{
	clearAllBinaryData();
}

void EntityResultCartesianMeshVtk::clearAllBinaryData()
{
	if (_vtkDataAbs != nullptr)
	{
		delete _vtkDataAbs;
		_vtkDataAbs = nullptr;
	}

	if (_vtkDataArg != nullptr)
	{
		delete _vtkDataArg;
		_vtkDataArg = nullptr;
	}
}

bool EntityResultCartesianMeshVtk::getEntityBox(double & xmin, double & xmax, double & ymin, double & ymax, double & zmin, double & zmax)
{
	return false;
}

void EntityResultCartesianMeshVtk::setComplexData(const std::string& quantityName, eQuantityType quantityType, EntityBinaryData*& dataAbs, EntityBinaryData*& dataArg)
{
	clearAllBinaryData();

	_quantityName = quantityName;
	_quantityType = quantityType;

	_vtkDataAbs = dataAbs;
	_vtkDataArg = dataArg;

	_vtkDataIDAbs = _vtkDataAbs->getEntityID();
	_vtkDataVersionAbs = _vtkDataAbs->getEntityStorageVersion();

	_vtkDataIDArg = _vtkDataArg->getEntityID();
	_vtkDataVersionArg = _vtkDataArg->getEntityStorageVersion();

	dataAbs = nullptr;
	dataArg = nullptr;
}

void EntityResultCartesianMeshVtk::getComplexData(std::string &quantityName, eQuantityType& quantityType, std::vector<char> &dataAbs, std::vector<char>& dataArg)
{
	quantityName.clear();

	if (_vtkDataAbs == nullptr && _vtkDataIDAbs != -1 && _vtkDataVersionAbs != -1)
	{
		std::map<ot::UID, EntityBase*> entityMap;
		_vtkDataAbs = dynamic_cast<EntityBinaryData*>(readEntityFromEntityIDAndVersion(this, _vtkDataIDAbs, _vtkDataVersionAbs, entityMap));
		assert(_vtkDataAbs != nullptr);
	}

	if (_vtkDataArg == nullptr && _vtkDataIDArg != -1 && _vtkDataVersionArg != -1)
	{
		std::map<ot::UID, EntityBase*> entityMap;
		_vtkDataArg = dynamic_cast<EntityBinaryData*>(readEntityFromEntityIDAndVersion(this, _vtkDataIDArg, _vtkDataVersionArg, entityMap));
		assert(_vtkDataArg != nullptr);
	}

	if (_vtkDataAbs == nullptr || _vtkDataArg == nullptr)
	{
		return; // The data is missing for this entity.
	}

	quantityName = _quantityName;
	quantityType = _quantityType;
	dataAbs      = _vtkDataAbs->getData();
	dataArg      = _vtkDataArg->getData();
}

void EntityResultCartesianMeshVtk::addStorageData(bsoncxx::builder::basic::document & storage)
{
	EntityBase::addStorageData(storage);

	if (_vtkDataAbs != nullptr)
	{
		_vtkDataIDAbs = _vtkDataAbs->getEntityID();
		_vtkDataVersionAbs = _vtkDataAbs->getEntityStorageVersion();
	}

	if (_vtkDataArg != nullptr)
	{
		_vtkDataIDArg = _vtkDataArg->getEntityID();
		_vtkDataVersionArg = _vtkDataArg->getEntityStorageVersion();
	}

	storage.append(
		bsoncxx::builder::basic::kvp("quantityName",		_quantityName),
		bsoncxx::builder::basic::kvp("quantityType",		(int) _quantityType),
		bsoncxx::builder::basic::kvp("vtkDataIDAbs",		_vtkDataIDAbs),
		bsoncxx::builder::basic::kvp("vtkDataVersionAbs",	_vtkDataVersionAbs),
		bsoncxx::builder::basic::kvp("vtkDataIDArg",		_vtkDataIDArg),
		bsoncxx::builder::basic::kvp("vtkDataVersionArg",	_vtkDataVersionArg),
		bsoncxx::builder::basic::kvp("resultType",			(long long)getResultType())
	);
}

void EntityResultCartesianMeshVtk::readSpecificDataFromDataBase(const bsoncxx::document::view & doc_view, std::map<ot::UID, EntityBase*>& entityMap)
{
	EntityBase::readSpecificDataFromDataBase(doc_view, entityMap);

	_quantityName		= doc_view["quantityName"].get_string();
	_quantityType		= (eQuantityType) (int) (doc_view["quantityType"].get_int32());
	_vtkDataIDAbs		= doc_view["vtkDataIDAbs"].get_int64().value;
	_vtkDataVersionAbs	= doc_view["vtkDataVersionAbs"].get_int64().value;
	_vtkDataIDArg		= doc_view["vtkDataIDArg"].get_int64().value;
	_vtkDataVersionArg	= doc_view["vtkDataVersionArg"].get_int64().value;

	setResultType(static_cast<tResultType>((long long)doc_view["resultType"].get_int64()));
}
