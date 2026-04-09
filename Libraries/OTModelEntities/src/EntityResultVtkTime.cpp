// @otlicense
// File: EntityResultVtkTime.cpp
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
#include "OTModelEntities/EntityResultVtkTime.h"
#include "OTModelEntities/DataBase.h"

static EntityFactoryRegistrar<EntityResultVtkTime> registrar("EntityResultVtkTime");

EntityResultVtkTime::EntityResultVtkTime(ot::UID ID, EntityBase* parent, EntityObserver* obs, ModelState* ms)
	:EntityBase(ID, parent, obs, ms)
{}

EntityResultVtkTime::~EntityResultVtkTime()
{
}

bool EntityResultVtkTime::getEntityBox(double & xmin, double & xmax, double & ymin, double & ymax, double & zmin, double & zmax)
{
	return false;
}

void EntityResultVtkTime::setTimeData(const std::string& quantityName, const std::list<std::pair<ot::UID, ot::UID>>& dataEntityList, const std::list<double>& dataEntityTimeList)
{
	_quantityName       = quantityName;
	_dataEntityList     = dataEntityList;
	_dataEntityTimeList = dataEntityTimeList;
}

//void EntityResultVtkTime::getComplexData(std::string &quantityName, eQuantityType& quantityType, std::vector<char> &dataAbs, std::vector<char>& dataArg)
//{
//	quantityName.clear();
//
//	if (_vtkDataAbs == nullptr && _vtkDataIDAbs != -1 && _vtkDataVersionAbs != -1)
//	{
//		std::map<ot::UID, EntityBase*> entityMap;
//		_vtkDataAbs = dynamic_cast<EntityBinaryData*>(readEntityFromEntityIDAndVersion(this, _vtkDataIDAbs, _vtkDataVersionAbs, entityMap));
//		assert(_vtkDataAbs != nullptr);
//	}
//
//	if (_vtkDataArg == nullptr && _vtkDataIDArg != -1 && _vtkDataVersionArg != -1)
//	{
//		std::map<ot::UID, EntityBase*> entityMap;
//		_vtkDataArg = dynamic_cast<EntityBinaryData*>(readEntityFromEntityIDAndVersion(this, _vtkDataIDArg, _vtkDataVersionArg, entityMap));
//		assert(_vtkDataArg != nullptr);
//	}
//
//	if (_vtkDataAbs == nullptr || _vtkDataArg == nullptr)
//	{
//		return; // The data is missing for this entity.
//	}
//
//	quantityName = _quantityName;
//	quantityType = _quantityType;
//	dataAbs      = _vtkDataAbs->getData();
//	dataArg      = _vtkDataArg->getData();
//}

void EntityResultVtkTime::addStorageData(bsoncxx::builder::basic::document & storage)
{
	EntityBase::addStorageData(storage);

	auto dataEntityID = bsoncxx::builder::basic::array();
	auto dataEntityVersion = bsoncxx::builder::basic::array();

	for (auto dataEntity : _dataEntityList)
	{
		dataEntityID.append((long long)dataEntity.first);
		dataEntityVersion.append((long long)dataEntity.second);
	}

	auto dataEntityTime = bsoncxx::builder::basic::array();

	for (auto time : _dataEntityTimeList)
	{
		dataEntityTime.append(time);
	}

	storage.append(
		bsoncxx::builder::basic::kvp("quantityName",		_quantityName),
		bsoncxx::builder::basic::kvp("vtkDataID",			dataEntityID),
		bsoncxx::builder::basic::kvp("vtkDataVersion",		dataEntityVersion),
		bsoncxx::builder::basic::kvp("vtkDataTime",			dataEntityTime),
		bsoncxx::builder::basic::kvp("resultType", (long long)getResultType()),
		bsoncxx::builder::basic::kvp("scaleFactor",			_scaleFactor)
	);
}

void EntityResultVtkTime::readSpecificDataFromDataBase(const bsoncxx::document::view & doc_view, std::map<ot::UID, EntityBase*>& entityMap)
{
	EntityBase::readSpecificDataFromDataBase(doc_view, entityMap);

	_dataEntityList.clear();
	_dataEntityTimeList.clear();

	_quantityName		= doc_view["quantityName"].get_string();
	_scaleFactor		= doc_view["scaleFactor"].get_double();

	setResultType(static_cast<tResultType>((long long)doc_view["resultType"].get_int64()));

	auto dataEntityID      = doc_view["vtkDataID"].get_array().value;
	auto dataEntityVersion = doc_view["vtkDataVersion"].get_array().value;
	auto dataEntityTime    = doc_view["vtkDataTime"].get_array().value;

	size_t numberDataEntities = std::distance(dataEntityID.begin(), dataEntityID.end());
	assert(numberDataEntities == std::distance(dataEntityVersion.begin(), dataEntityVersion.end()));
	assert(numberDataEntities == std::distance(dataEntityTime.begin(), dataEntityTime.end()));

	auto cID	  = dataEntityID.begin();
	auto cVersion = dataEntityVersion.begin();
	auto cTime    = dataEntityTime.begin();

	for (unsigned long index = 0; index < numberDataEntities; index++)
	{
		_dataEntityList.push_back(std::pair<ot::UID, ot::UID>(cID->get_int64(), cVersion->get_int64()));
		_dataEntityTimeList.push_back(cTime->get_double());

		cID++;
		cVersion++;
		cTime++;
	}
}
