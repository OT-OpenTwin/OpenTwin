// @otlicense
// File: EntityFileRawData.cpp
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

// OpenTwin header
#include "EntityFileRawData.h"

#include "OTCommunication/ActionTypes.h"
#include "DataBase.h"
#include "OTCore/String.h"
#include "OTCore/LogDispatcher.h"
#include "OTCore/RuntimeTests.h"
#include "OTCore/EncodingGuesser.h"
#include "OTCore/EncodingConverter_UTF16ToUTF8.h"
#include "OTCore/EncodingConverter_ISO88591ToUTF8.h"
#include "OTGui/VisualisationTypes.h"

#include "PropertyHelper.h"

static EntityFactoryRegistrar<EntityFileRawData> registrar(EntityFileRawData::className());

EntityFileRawData::EntityFileRawData(ot::UID _ID, EntityBase* _parent, EntityObserver* _obs, ModelState* _ms)
	: EntityFile(_ID, _parent, _obs, _ms), m_format(ot::ImageFileFormat::PNG) {
}

bool EntityFileRawData::updateFromProperties() {
	return EntityFile::updateFromProperties();
}

void EntityFileRawData::setSpecializedProperties() {
	EntityFile::setSpecializedProperties();
}

void EntityFileRawData::addStorageData(bsoncxx::builder::basic::document& _storage) {
	EntityFile::addStorageData(_storage);
}

void EntityFileRawData::readSpecificDataFromDataBase(const bsoncxx::document::view& _doc_view, std::map<ot::UID, EntityBase*>& _entityMap) {
	EntityFile::readSpecificDataFromDataBase(_doc_view, _entityMap);
}
