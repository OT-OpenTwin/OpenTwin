// @otlicense
// File: EntityBinaryData.cpp
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
#include "DocumentAPI.h"
#include "EntityBinaryData.h"

#include <bsoncxx/builder/basic/array.hpp>

static EntityFactoryRegistrar<EntityBinaryData> registrar(EntityBinaryData::className());

EntityBinaryData::EntityBinaryData(ot::UID ID, EntityBase *parent, EntityObserver *obs, ModelState *ms) :
	EntityBase(ID, parent, obs, ms)
{
	
}

EntityBinaryData::~EntityBinaryData()
{
	data.clear();
}

bool EntityBinaryData::getEntityBox(double &xmin, double &xmax, double &ymin, double &ymax, double &zmin, double &zmax)
{
	return false;
}


void EntityBinaryData::addStorageData(bsoncxx::builder::basic::document &storage)
{
	// We store the parent class information first 
	EntityBase::addStorageData(storage);

	// Now add the actual data array

	if (data.size() > DataStorageAPI::DocumentManager::MaxDocumentLength)
	{
		// The data is too large and needs to be stored in GridFS
		DataStorageAPI::DocumentAPI doc;

		bsoncxx::types::value result = doc.InsertBinaryDataUsingGridFs((uint8_t*)(data.data()), data.size(), DataBase::instance().getCollectionName());
		std::string fileId = result.get_oid().value.to_string();

		storage.append(
			bsoncxx::builder::basic::kvp("size", (long long)data.size()),
			bsoncxx::builder::basic::kvp("file", fileId)
		);
	}
	else
	{
		// The data is small enough and can be stored directly in the document
		bsoncxx::types::b_binary bin_data;
		bin_data.size = data.size();
		bin_data.bytes = (uint8_t*)(data.data());

		storage.append(
			bsoncxx::builder::basic::kvp("size", (long long)data.size()),
			bsoncxx::builder::basic::kvp("data", bin_data)
		);
	}
}

void EntityBinaryData::readSpecificDataFromDataBase(bsoncxx::document::view &doc_view, std::map<ot::UID, EntityBase *> &entityMap)
{
	// We read the parent class information first 
	EntityBase::readSpecificDataFromDataBase(doc_view, entityMap);

	// Now read the curve data

	data.clear();

	// Now load the data from the storage

	if (doc_view.find("file") != doc_view.end())
	{
		// The storage is separate in GridFS
		DataStorageAPI::DocumentAPI doc;
		uint8_t* buffer = nullptr;
		size_t length = 0;

		std::string file = doc_view["file"].get_utf8().value.data();
		bsoncxx::oid oid_obj{ file };
		bsoncxx::types::value id{ bsoncxx::types::b_oid{oid_obj} };

		doc.GetDocumentUsingGridFs(id, buffer, length, DataBase::instance().getCollectionName());

		data.insert(data.end(), buffer, buffer + length);
	}
	else
	{
		// The storage is directly in the entity
		auto bin_data = doc_view["data"].get_binary();

		data.reserve(bin_data.size);

		for (unsigned long index = 0; index < bin_data.size; index++)
		{
			data.push_back(bin_data.bytes[index]);
		}
	}

	resetModified();
}

void EntityBinaryData::removeChild(EntityBase *child)
{
	EntityBase::removeChild(child);
}

void EntityBinaryData::setData(const char *dat, size_t length)
{
	data.clear();
	data.reserve(length);

	for (unsigned long index = 0; index < length; index++)
	{
		data.push_back(dat[index]);
	}
	setModified();
}

void EntityBinaryData::setData(const std::vector<char>& _data) {
	data = _data;
	setModified();
}

void EntityBinaryData::setData(const std::vector<char>&& _data) {
	data = std::move(_data);
	setModified();
}

void EntityBinaryData::clearData() {
	data.clear();
}

const std::vector<char>& EntityBinaryData::getData() const
{
	return data;
}

