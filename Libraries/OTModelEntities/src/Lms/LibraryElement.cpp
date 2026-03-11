// @otlicense
// File: LibraryModelImportCfg.cpp
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

#include "OTModelEntities/Lms/LibraryElement.h"

std::string ot::LibraryElement::getMetaDataValue(const std::string& _key) const {
    auto it = m_metaData.find(_key);
    if (it != m_metaData.end()) {
        return it->second;
    }
    return "";
}

std::string ot::LibraryElement::getAdditionalInfoValue(const std::string& _key) const {
    auto it = m_additionalInfos.find(_key);
    if (it != m_additionalInfos.end()) {
        return it->second;
    }
    return "";
}


void ot::LibraryElement::addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const {
    // Basic information
    _object.AddMember("Name", ot::JsonString(m_name, _allocator), _allocator);
    _object.AddMember("Version", m_version, _allocator);

    // Origin information
    _object.AddMember("FileName", ot::JsonString(m_fileName, _allocator), _allocator);
    _object.AddMember("Hash", ot::JsonString(m_hash, _allocator), _allocator);

    // Model data
    _object.AddMember("Data", ot::JsonString(m_data, _allocator), _allocator);

    _object.AddMember("RequestingEntityID", m_requestingEntityID, _allocator);
    _object.AddMember("CollectionName", ot::JsonString(m_collectionName, _allocator), _allocator);

    _object.AddMember("EntityType", ot::JsonString(LibraryElementSelectionCfg::entityTypeToString(m_newEntityType), _allocator), _allocator);
    _object.AddMember("NewEntityFolder", ot::JsonString(m_newEntityFolder, _allocator), _allocator);
    _object.AddMember("CallbackService", ot::JsonString(m_callBackService, _allocator), _allocator);

    // Serialize metadata
    ot::JsonObject metaDataObj;
    for (const auto& pair : m_metaData) {
        metaDataObj.AddMember(
            ot::JsonString(pair.first, _allocator),
            ot::JsonString(pair.second, _allocator),
            _allocator
        );
    }
    _object.AddMember("MetaData", metaDataObj, _allocator);

    // Serialize additional infos
    ot::JsonObject additionalInfosObj;
    for (const auto& pair : m_additionalInfos) {
        additionalInfosObj.AddMember(
            ot::JsonString(pair.first, _allocator),
            ot::JsonString(pair.second, _allocator),
            _allocator
        );
    }
    _object.AddMember("AdditionalInfos", additionalInfosObj, _allocator);
}

void ot::LibraryElement::setFromJsonObject(const ot::ConstJsonObject& _object) {
    // Basic information
    m_name = ot::json::getString(_object, "Name");
    m_version = ot::json::getInt64(_object, "Version");

    // Origin information
    m_fileName = ot::json::getString(_object, "FileName");
    m_hash = ot::json::getString(_object, "Hash");

    // Model data
    m_data = ot::json::getString(_object, "Data");

    // Import context
    m_requestingEntityID = ot::json::getUInt64(_object, "RequestingEntityID");
    m_collectionName = ot::json::getString(_object, "CollectionName");

    m_callBackService = ot::json::getString(_object, "CallbackService");
	m_newEntityType = LibraryElementSelectionCfg::stringToEntityType(ot::json::getString(_object, "EntityType"));
	m_newEntityFolder = ot::json::getString(_object, "NewEntityFolder");

    // Deserialize metadata
    m_metaData.clear();
    if (_object.HasMember("MetaData")) {
        ot::ConstJsonObject metaDataObj = ot::json::getObject(_object, "MetaData");
        for (auto it = metaDataObj.MemberBegin(); it != metaDataObj.MemberEnd(); ++it) {
            std::string key = it->name.GetString();
            std::string value = it->value.IsString() ? it->value.GetString() : "";
            m_metaData[key] = value;
        }
    }

    // Deserialize additional infos
    m_additionalInfos.clear();
    if (_object.HasMember("AdditionalInfos")) {
        ot::ConstJsonObject additionalInfosObj = ot::json::getObject(_object, "AdditionalInfos");
        for (auto it = additionalInfosObj.MemberBegin(); it != additionalInfosObj.MemberEnd(); ++it) {
            std::string key = it->name.GetString();
            std::string value = it->value.IsString() ? it->value.GetString() : "";
            m_additionalInfos[key] = value;
        }
    }
}

bool ot::LibraryElement::deserializeCallbackInfoFromAdditionalInfo(const std::string& _additionalInfo) {
 

    if (_additionalInfo.empty()) {
        return false;
    }

    ot::JsonDocument doc;
    if (!doc.fromJson(_additionalInfo)) {
        return false;
    }

    if (!doc.HasMember("CallbackService") || !doc.HasMember("EntityType") || !doc.HasMember("NewEntityFolder")) {
        return false;
    }

    m_callBackService = ot::json::getString(doc, "CallbackService");
    m_newEntityType = LibraryElementSelectionCfg::stringToEntityType(ot::json::getString(doc, "EntityType"));
    m_newEntityFolder = ot::json::getString(doc, "NewEntityFolder");

    return true;
   
}