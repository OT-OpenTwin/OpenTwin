// @otlicense
// File: LibraryElementSelectionCfg.cpp
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

#include "OTModelEntities/Lms/LibraryElementSelectionCfg.h"

ot::LibraryElementSelectionCfg::LibraryElementSelectionCfg() : m_requestingEntityID(0) {
}

ot::LibraryElementSelectionCfg::LibraryElementSelectionCfg(const LibraryElementSelectionCfg& _other) : m_collectionName(_other.m_collectionName),
    m_additionalInfoFilters(_other.m_additionalInfoFilters),
    m_metaDataInfoFilters(_other.m_metaDataInfoFilters),
    m_callBackService(_other.m_callBackService),
    m_callBackAction(_other.m_callBackAction),
    m_className(_other.m_className),
    m_requestingEntityID(_other.m_requestingEntityID),
    m_uiServiceUrl(_other.m_uiServiceUrl),
    m_additionalInfo(_other.m_additionalInfo),
    m_newEntityFolder(_other.m_newEntityFolder) {
}

ot::LibraryElementSelectionCfg::LibraryElementSelectionCfg(LibraryElementSelectionCfg&& _other) noexcept
    : m_collectionName(std::move(_other.m_collectionName)),
    m_additionalInfoFilters(std::move(_other.m_additionalInfoFilters)),
    m_metaDataInfoFilters(std::move(_other.m_metaDataInfoFilters)),
    m_callBackService(std::move(_other.m_callBackService)),
    m_callBackAction(std::move(_other.m_callBackAction)),
    m_className(_other.m_className),
    m_requestingEntityID(_other.m_requestingEntityID),
    m_uiServiceUrl(std::move(_other.m_uiServiceUrl)),
    m_additionalInfo(_other.m_additionalInfo),
    m_newEntityFolder(_other.m_newEntityFolder) {
}

ot::LibraryElementSelectionCfg& ot::LibraryElementSelectionCfg::operator=(const LibraryElementSelectionCfg& _other) {
    if (this != &_other) {
        m_collectionName = _other.m_collectionName;
        m_additionalInfoFilters = _other.m_additionalInfoFilters;
        m_metaDataInfoFilters = _other.m_metaDataInfoFilters;
        m_callBackService = _other.m_callBackService;
        m_callBackAction = _other.m_callBackAction;
        m_className = _other.m_className;
        m_requestingEntityID = _other.m_requestingEntityID;
        m_uiServiceUrl = _other.m_uiServiceUrl;
        m_additionalInfo = _other.m_additionalInfo;
		m_newEntityFolder = _other.m_newEntityFolder;
    }
    return *this;
}

ot::LibraryElementSelectionCfg& ot::LibraryElementSelectionCfg::operator=(LibraryElementSelectionCfg&& _other) noexcept {
    if (this != &_other) {
        m_collectionName = std::move(_other.m_collectionName);
        m_additionalInfoFilters = std::move(_other.m_additionalInfoFilters);
        m_metaDataInfoFilters = std::move(_other.m_metaDataInfoFilters);
        m_callBackService = std::move(_other.m_callBackService);
        m_callBackAction = std::move(_other.m_callBackAction);
        m_className = _other.m_className;
        m_requestingEntityID = _other.m_requestingEntityID;
        m_uiServiceUrl = std::move(_other.m_uiServiceUrl);
        m_additionalInfo = std::move(_other.m_additionalInfo);
		m_newEntityFolder = std::move(_other.m_newEntityFolder);
    }
    return *this;
}

// Serialization
void ot::LibraryElementSelectionCfg::addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const {

    _object.AddMember("CollectionName", ot::JsonString(m_collectionName, _allocator), _allocator);

    ot::JsonArray filterArray;
    for (const auto& pair : m_additionalInfoFilters) {
        ot::JsonObject filterObj;
        filterObj.AddMember("FieldName", ot::JsonString(pair.first, _allocator), _allocator);
        filterObj.AddMember("Value", ot::JsonString(pair.second, _allocator), _allocator);
        filterArray.PushBack(filterObj, _allocator);
    }
    _object.AddMember("AdditionalFilters", filterArray, _allocator);


    ot::JsonArray metadataArray;
    for (const auto& pair : m_metaDataInfoFilters) {
        ot::JsonObject metadataObj;
        metadataObj.AddMember("FieldName", ot::JsonString(pair.first, _allocator), _allocator);
        metadataObj.AddMember("Value", ot::JsonString(pair.second, _allocator), _allocator);
        metadataArray.PushBack(metadataObj, _allocator);
    }
    _object.AddMember("MetadataInfo", metadataArray, _allocator);
    _object.AddMember("CallbackService", ot::JsonString(m_callBackService, _allocator), _allocator);
    _object.AddMember("CallbackAction", ot::JsonString(m_callBackAction, _allocator), _allocator);
    _object.AddMember("EntityType", ot::JsonString(m_className, _allocator), _allocator);
    _object.AddMember("RequestingEntityID", static_cast<int64_t>(m_requestingEntityID), _allocator);
    _object.AddMember("UiServiceURL", ot::JsonString(m_uiServiceUrl, _allocator), _allocator);
	_object.AddMember("AdditionalInfo", ot::JsonString(m_additionalInfo, _allocator), _allocator);
	_object.AddMember("NewEntityFolder", ot::JsonString(m_newEntityFolder, _allocator), _allocator);
}

void ot::LibraryElementSelectionCfg::setFromJsonObject(const ot::ConstJsonObject& _object) {
    // Collection name
    m_collectionName = ot::json::getString(_object, "CollectionName");

    // Additional filter pairs
    m_additionalInfoFilters.clear();
    if (_object.HasMember("AdditionalFilters")) {
        ot::ConstJsonArray filterArray = ot::json::getArray(_object, "AdditionalFilters");
        for (rapidjson::SizeType i = 0; i < filterArray.Size(); ++i) {
            ot::ConstJsonObject filterObj = ot::json::getObject(filterArray, i);
            std::string fieldName = ot::json::getString(filterObj, "FieldName");
            std::string value = ot::json::getString(filterObj, "Value");
            m_additionalInfoFilters.push_back(std::make_pair(fieldName, value));
        }
    }


    m_metaDataInfoFilters.clear();
    if (_object.HasMember("MetadataInfo")) {
        ot::ConstJsonArray metadataArray = ot::json::getArray(_object, "MetadataInfo");
        for (rapidjson::SizeType i = 0; i < metadataArray.Size(); ++i) {
            ot::ConstJsonObject metadataObj = ot::json::getObject(metadataArray, i);
            std::string fieldName = ot::json::getString(metadataObj, "FieldName");
            std::string value = ot::json::getString(metadataObj, "Value");
            m_metaDataInfoFilters.push_back(std::make_pair(fieldName, value));
        }
    }


    m_callBackService = ot::json::getString(_object, "CallbackService");
    m_callBackAction = ot::json::getString(_object, "CallbackAction");
    m_className = ot::json::getString(_object, "EntityType");
    m_requestingEntityID = ot::json::getUInt64(_object, "RequestingEntityID");
    m_uiServiceUrl = ot::json::getString(_object, "UiServiceURL");
	m_additionalInfo = ot::json::getString(_object, "AdditionalInfo");
	m_newEntityFolder = ot::json::getString(_object, "NewEntityFolder");
}

void ot::LibraryElementSelectionCfg::serializeCallbackInfoToAdditionalInfo() {
    
    ot::JsonDocument doc;
    doc.AddMember("CallbackService", ot::JsonString(m_callBackService, doc.GetAllocator()), doc.GetAllocator());
    doc.AddMember("EntityType", ot::JsonString(m_className, doc.GetAllocator()), doc.GetAllocator());
    doc.AddMember("NewEntityFolder", ot::JsonString(m_newEntityFolder, doc.GetAllocator()), doc.GetAllocator());

    m_additionalInfo = doc.toJson();
}

