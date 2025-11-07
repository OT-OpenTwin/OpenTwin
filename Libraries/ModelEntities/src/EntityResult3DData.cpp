// @otlicense
// File: EntityResult3DData.cpp
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
#include "../include/EntityResult3DData.h"
#include "DataBase.h"

static EntityFactoryRegistrar<EntityResult3DData> registrar("EntityResult3DData");

EntityResult3DData::EntityResult3DData(ot::UID ID, EntityBase * parent, EntityObserver * obs, ModelState * ms)
	:EntityBase(ID,parent,obs,ms)
{}

EntityResult3DData::~EntityResult3DData()
{
	DeleteAllCompressedVectors();
}

void EntityResult3DData::DeleteAllCompressedVectors()
{
	if (_xComponentData != nullptr)
	{
		delete _xComponentData;
		_xComponentDataID = -1;
		_xComponentDataVersion = -1;
	}
	if (_yComponentData != nullptr)
	{
		delete _yComponentData;
		_yComponentDataID = -1;
		_yComponentDataVersion = -1;
	}
	if (_zComponentData != nullptr)
	{
		delete _zComponentData;
		_zComponentDataID = -1;
		_zComponentDataVersion = -1;
	}
}

bool EntityResult3DData::getEntityBox(double & xmin, double & xmax, double & ymin, double & ymax, double & zmin, double & zmax)
{
	return false;
}

void EntityResult3DData::storeToDataBase(void)
{
	if (  _xComponentDataID == -1 || _yComponentDataID == -1 || _zComponentDataID == -1 
		|| _xComponentDataVersion == -1 || _yComponentDataVersion == -1 || _zComponentDataVersion == -1
		|| _meshDataID == -1 || _meshDataVersion == -1)
	{
		throw std::invalid_argument("Not all component UIDs are set.");
	};
	
	EntityBase::storeToDataBase();
}

/**
 * @brief  .
 * 
 * @throws invalid_exception if a ot::UID is not set.
 */
void EntityResult3DData::LoadAllData(void)
{
	if (_xComponentDataID == -1 || _yComponentDataID == -1 || _zComponentDataID == -1 
		|| _xComponentDataVersion == -1 || _yComponentDataVersion == -1 || _zComponentDataVersion == -1
		|| _meshDataID == -1 || _meshDataVersion == -1)
	{
		throw std::invalid_argument("A component ot::UID is not set.");
	}
	std::map<ot::UID, EntityBase*> entitymap;

	std::list<std::pair<unsigned long long, unsigned long long>> prefetchIds
	{
		{_meshDataID,_meshDataVersion}, {_xComponentDataID,_xComponentDataVersion}, {_yComponentDataID,_yComponentDataVersion}, {_zComponentDataID,_zComponentDataVersion} 
	};
	DataBase::instance().prefetchDocumentsFromStorage(prefetchIds);
	
	_xComponentData = dynamic_cast<EntityCompressedVector*>(readEntityFromEntityIDAndVersion(this, _xComponentDataID, _xComponentDataVersion, entitymap));
	assert(_xComponentData != nullptr);

	_yComponentData = dynamic_cast<EntityCompressedVector*>(readEntityFromEntityIDAndVersion(this, _yComponentDataID, _yComponentDataVersion, entitymap));
	assert(_yComponentData != nullptr);

	_zComponentData = dynamic_cast<EntityCompressedVector*>(readEntityFromEntityIDAndVersion(this, _zComponentDataID, _zComponentDataVersion, entitymap));
	assert(_zComponentData != nullptr);
	
	_meshData = dynamic_cast<EntityMeshCartesianData*>(readEntityFromEntityIDAndVersion(this, _meshDataID, _meshDataVersion, entitymap));
	assert(_meshData != nullptr);
}

void EntityResult3DData::SetVectorData(EntityCompressedVector * xComponent, EntityCompressedVector * yComponent, EntityCompressedVector * zComponent)
{
	DeleteAllCompressedVectors();

	_xComponentData->storeToDataBase();
	_yComponentData->storeToDataBase();
	_zComponentData->storeToDataBase();

	_xComponentData = xComponent;
	_xComponentDataID = xComponent->getEntityID();
	_xComponentDataVersion = xComponent->getEntityStorageVersion();

	_yComponentData = yComponent;
	_yComponentDataID = yComponent->getEntityID();
	_yComponentDataVersion = yComponent->getEntityStorageVersion();

	_zComponentData = zComponent;
	_zComponentDataID = zComponent->getEntityID();
	_zComponentDataVersion = zComponent->getEntityStorageVersion();
}

void EntityResult3DData::SetVectorDataUIDs(ot::UID xComponentID, ot::UID xComponentVersion, ot::UID yComponentID, ot::UID yComponentVersion, ot::UID zComponentID, ot::UID zComponentVersion)
{
	DeleteAllCompressedVectors();
	_xComponentDataID      = xComponentID;
	_xComponentDataVersion = xComponentVersion;
	_yComponentDataID      = yComponentID;
	_yComponentDataVersion = yComponentVersion;
	_zComponentDataID      = zComponentID;
	_zComponentDataVersion = zComponentVersion;
}

void EntityResult3DData::setMeshData(ot::UID meshDataID, ot::UID meshDataVersion)
{
	_meshDataID = meshDataID;
	_meshDataVersion = meshDataVersion;
}

const double* EntityResult3DData::GetXComponentData(size_t & size)
{
	if (_xComponentData == nullptr)
	{
		if (_xComponentDataID == -1)
		{
			throw std::invalid_argument("Compressed vector xComponent ot::UID not existing.");
		}
		std::map<ot::UID, EntityBase *> entityMap;
		_xComponentData = dynamic_cast<EntityCompressedVector*>(readEntityFromEntityIDAndVersion(this, _xComponentDataID,_xComponentDataVersion, entityMap));
		assert(_xComponentData != nullptr);
	}

	size = _xComponentData->getUncompressedLength();
	double * values = new double[size];
	_xComponentData->getValues(values,size);
	return values;
}

const double* EntityResult3DData::GetYComponentData(size_t & size)
{
	if (_yComponentData == nullptr)
	{
		if (_yComponentDataID == -1)
		{
			throw std::invalid_argument("Compressed vector yComponent ot::UID not existing.");
		}
		std::map<ot::UID, EntityBase *> entityMap;
		_yComponentData = dynamic_cast<EntityCompressedVector*>(readEntityFromEntityIDAndVersion(this, _yComponentDataID, _yComponentDataVersion, entityMap));
		assert(_yComponentData != nullptr);
	}

	size = _yComponentData->getUncompressedLength();
	double * values = new double[size];
	_yComponentData->getValues(values,size);
	return values;
}

const double* EntityResult3DData::GetZComponentData(size_t & size)
{
	if (_zComponentData == nullptr)
	{
		if (_zComponentDataID == -1)
		{
			throw std::invalid_argument("Compressed vector zComponent ot::UID not existing.");
		}
		std::map<ot::UID, EntityBase *> entityMap;
		_zComponentData = dynamic_cast<EntityCompressedVector*>(readEntityFromEntityIDAndVersion(this, _zComponentDataID, _zComponentDataVersion, entityMap));
		assert(_zComponentData != nullptr);
	}

	size = _zComponentData->getUncompressedLength();
	double * values = new double[size];
	_zComponentData->getValues(values,size);
	return values;
}

const double * EntityResult3DData::GetXComponentDataAndReleaseEntity(size_t & size)
{
	const double * component = GetXComponentData(size);
	delete _xComponentData;
	_xComponentData = nullptr;
	return component;
}

const double * EntityResult3DData::GetYComponentDataAndReleaseEntity(size_t & size)
{
	const double * component = GetYComponentData(size);
	delete _yComponentData;
	_yComponentData = nullptr;
	return component;
}

const double * EntityResult3DData::GetZComponentDataAndReleaseEntity(size_t & size)
{
	const double * component = GetZComponentData(size);
	delete _zComponentData;
	_zComponentData = nullptr;
	return component;
}

EntityMeshCartesianData * EntityResult3DData::GetMeshData(void)
{
	if (_meshDataID == -1 || _meshDataVersion == -1)
	{
		throw std::invalid_argument("Meshdata ot::UID or Version ID not existing.");
	}
	if (_meshData == nullptr)
	{
		std::map<ot::UID, EntityBase *> entityMap;
		auto meshDataEntity = readEntityFromEntityIDAndVersion(this, _meshDataID, _meshDataVersion, entityMap);
		if (meshDataEntity == nullptr)
		{
			throw std::invalid_argument("Requested Meshdata does not exist.");
		}
		_meshData = dynamic_cast<EntityMeshCartesianData*>(meshDataEntity);
		assert(_meshData != nullptr);
	}
	
	return _meshData;
}

void EntityResult3DData::clearData(void)
{
	clearXComponentData();
	clearYComponentData();
	clearZComponentData();
}

void EntityResult3DData::clearXComponentData(void)
{
	if (_xComponentData != nullptr)
	{
		_xComponentData->clearData();
	}
}

void EntityResult3DData::clearYComponentData(void)
{
	if (_yComponentData != nullptr)
	{
		_yComponentData->clearData();
	}
}

void EntityResult3DData::clearZComponentData(void)
{
	if (_zComponentData != nullptr)
	{
		_zComponentData->clearData();
	}
}



void EntityResult3DData::addStorageData(bsoncxx::builder::basic::document & storage)
{
	EntityBase::addStorageData(storage);

	storage.append(
		bsoncxx::builder::basic::kvp("_xComponentDataID",      _xComponentDataID),
		bsoncxx::builder::basic::kvp("_xComponentDataVersion", _xComponentDataVersion),
		bsoncxx::builder::basic::kvp("_yComponentDataID",      _yComponentDataID),
		bsoncxx::builder::basic::kvp("_yComponentDataVersion", _yComponentDataVersion),
		bsoncxx::builder::basic::kvp("_zComponentDataID",      _zComponentDataID),
		bsoncxx::builder::basic::kvp("_zComponentDataVersion", _zComponentDataVersion),
		bsoncxx::builder::basic::kvp("meshID", _meshDataID),
		bsoncxx::builder::basic::kvp("meshVersion", _meshDataVersion),
		bsoncxx::builder::basic::kvp("resultType", (long long)getResultType())
	);

}

void EntityResult3DData::readSpecificDataFromDataBase(bsoncxx::document::view & doc_view, std::map<ot::UID, EntityBase*>& entityMap)
{
	EntityBase::readSpecificDataFromDataBase(doc_view, entityMap);

	_meshDataID            = doc_view["meshID"].get_int64().value;
	_meshDataVersion       = doc_view["meshVersion"].get_int64().value;
	_xComponentDataID      = doc_view["_xComponentDataID"].get_int64().value;
	_xComponentDataVersion = doc_view["_xComponentDataVersion"].get_int64().value;
	_yComponentDataID      = doc_view["_yComponentDataID"].get_int64().value;
	_yComponentDataVersion = doc_view["_yComponentDataVersion"].get_int64().value;
	_zComponentDataID      = doc_view["_zComponentDataID"].get_int64().value;
	_zComponentDataVersion = doc_view["_zComponentDataVersion"].get_int64().value;
	setResultType(static_cast<tResultType>((long long)doc_view["resultType"].get_int64()));
}
