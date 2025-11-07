// @otlicense
// File: EntityResultTextData.cpp
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


#include "EntityResultTextData.h"
#include "DataBase.h"
#include "OldTreeIcon.h"

#include <bsoncxx/builder/basic/array.hpp>

static EntityFactoryRegistrar<EntityResultTextData> registrar("EntityResultTextData");

EntityResultTextData::EntityResultTextData(ot::UID ID, EntityBase *parent, EntityObserver *obs, ModelState *ms) :
	EntityBase(ID, parent, obs, ms)
{
	
}

EntityResultTextData::~EntityResultTextData()
{
}

bool EntityResultTextData::getEntityBox(double &xmin, double &xmax, double &ymin, double &ymax, double &zmin, double &zmax)
{
	return false;
}

void EntityResultTextData::storeToDataBase(void)
{
	EntityBase::storeToDataBase();
}

void EntityResultTextData::addStorageData(bsoncxx::builder::basic::document &storage)
{
	// We store the parent class information first 
	EntityBase::addStorageData(storage);

	// Now add the actual text data

	storage.append(
		bsoncxx::builder::basic::kvp("textData", textData)
	);
}

void EntityResultTextData::readSpecificDataFromDataBase(bsoncxx::document::view &doc_view, std::map<ot::UID, EntityBase *> &entityMap)
{
	// We read the parent class information first 
	EntityBase::readSpecificDataFromDataBase(doc_view, entityMap);

	// Now read the text data
	textData.clear();

	// Now load the data from the storage
	textData = doc_view["textData"].get_utf8().value.data();

	resetModified();
}

void EntityResultTextData::removeChild(EntityBase *child)
{
	EntityBase::removeChild(child);
}

void EntityResultTextData::setText(const std::string &text)
{
	textData = text;
	setModified();
}

std::string &EntityResultTextData::getText(void)
{
	return textData;
}
