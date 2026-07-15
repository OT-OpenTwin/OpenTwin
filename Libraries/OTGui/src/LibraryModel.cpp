// @otlicense
// File: LibraryModel.cpp
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
#include "OTCore/Logging/Logger.h"
#include "OTGui/LibraryModel.h"

#define OT_JSON_MEMBER_Name "Name"
#define OT_JSON_MEMBER_ModelType "Modeltype"
#define OT_JSON_MEMBER_ElementType "Elementtype"
#define OT_JSON_MEMBER_MetaData "Metadata"
#define OT_JSON_MEMBER_Key "Key"
#define OT_JSON_MEMBER_Value "Value"
#define OT_JSON_MEMBER_Owner "Owner"

std::string ot::LibraryModel::modelOriginToString(ModelOrigin _origin) {
    switch (_origin) {
    case ModelOrigin::Custom:
        return "Custom";
    case ModelOrigin::BuiltIn:
        return "Built-in";
    default:
        OT_LOG_E("Invalid model origin value: " + std::to_string(static_cast<int>(_origin)));
        return "Invalid";
    }
}

ot::LibraryModel::ModelOrigin ot::LibraryModel::stringToModelOrigin(const std::string& _origin) {
    if (_origin == "Custom") {
        return ModelOrigin::Custom;
    }
    else if (_origin == "Built-in") {
        return ModelOrigin::BuiltIn;
    }
    else {
        OT_LOG_E("Invalid model origin string: " + _origin);
        throw std::invalid_argument("Invalid model origin string: " + _origin);
    }
}

ot::LibraryModel::LibraryModel(const std::string& _name, const std::string& _modelType, const std::string& _elementType) 
	: m_name(_name), m_modelType(_modelType), m_elementType(_elementType)
{}

// ###########################################################################################################################################################################################################################################################################################################################

// Setter / Getter

void ot::LibraryModel::addMetaData(const std::string& _key, const std::string& _value) {
	m_metaData.insert_or_assign(_key, _value);
}

std::string ot::LibraryModel::getMetaDataValue(const std::string& _key) const {
    auto it = m_metaData.find(_key);
    if (it != m_metaData.end()) {
        return it->second;
    }

    OT_LOG_E("No meta data parameter found with key: " + _key);
    return "#" + _key;
}

// ###########################################################################################################################################################################################################################################################################################################################

// Serialization

void ot::LibraryModel::addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const {
    _object.AddMember(OT_JSON_MEMBER_Name, JsonString(m_name, _allocator), _allocator);
    _object.AddMember(OT_JSON_MEMBER_ModelType, JsonString(m_modelType, _allocator), _allocator);
    _object.AddMember(OT_JSON_MEMBER_ElementType, JsonString(m_elementType, _allocator), _allocator);
	_object.AddMember(OT_JSON_MEMBER_Owner, JsonString(m_owner, _allocator), _allocator);
	_object.AddMember("CollectionName", ot::JsonString(m_collectionName, _allocator), _allocator);

    JsonArray stringMapArr;
    for (const auto& it : m_metaData) {
        JsonObject stringMapObj;
        stringMapObj.AddMember(OT_JSON_MEMBER_Key, JsonString(it.first, _allocator), _allocator);
        stringMapObj.AddMember(OT_JSON_MEMBER_Value, JsonString(it.second, _allocator), _allocator);
        stringMapArr.PushBack(stringMapObj, _allocator);
    }
    _object.AddMember(OT_JSON_MEMBER_MetaData, stringMapArr, _allocator);
}

void ot::LibraryModel::setFromJsonObject(const ot::ConstJsonObject& _object) {
    m_name = json::getString(_object, OT_JSON_MEMBER_Name);
    m_modelType = json::getString(_object, OT_JSON_MEMBER_ModelType);
    m_elementType = json::getString(_object, OT_JSON_MEMBER_ElementType);
    
    if(_object.HasMember(OT_JSON_MEMBER_Owner)) {
        m_owner = json::getString(_object, OT_JSON_MEMBER_Owner);
	}

    if (ot::json::exists(_object, "CollectionName")) {
        m_collectionName = ot::json::getString(_object, "CollectionName");
    }

    m_metaData.clear();
    for (const ConstJsonObject& metaDataObj : json::getObjectList(_object, OT_JSON_MEMBER_MetaData)) {
        std::string k = json::getString(metaDataObj, OT_JSON_MEMBER_Key);
        std::string v = json::getString(metaDataObj, OT_JSON_MEMBER_Value);
        m_metaData.insert_or_assign(k, v);
    }
}
