#include "OTGui/LibraryModel.h"

#define OT_JSON_MEMBER_Name "Name"
#define OT_JSON_MEMBER_FileName "Filename"
#define OT_JSON_MEMBER_ModelType "Modeltype"
#define OT_JSON_MEMBER_ElementType "Elementtype"
#define OT_JSON_MEMBER_MetaData "Metadata"
#define OT_JSON_MEMBER_Key "Key"
#define OT_JSON_MEMBER_Value "Value"



void ot::LibraryModel::addData(const std::string& _key, const std::string& _value) {
	m_metaData.insert_or_assign(_key, _value);
}

std::string ot::LibraryModel::getParameter(const std::string& _key) const {
    auto it = m_metaData.find(_key);
    if (it != m_metaData.end()) {
        return it->second;
    }

    OT_LOG_E("No meta data parameter found with key: " + _key);
    return "#" + _key;
}

const std::unordered_map<std::string, std::string>& ot::LibraryModel::getMetaData() const {
    return m_metaData;
}

void ot::LibraryModel::addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const {
    _object.AddMember(OT_JSON_MEMBER_Name, JsonString(m_name, _allocator), _allocator);
    _object.AddMember(OT_JSON_MEMBER_FileName, JsonString(m_fileName, _allocator), _allocator);
    _object.AddMember(OT_JSON_MEMBER_ModelType, JsonString(m_modelType, _allocator), _allocator);
    _object.AddMember(OT_JSON_MEMBER_ElementType, JsonString(m_elementType, _allocator), _allocator);
  
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
    m_fileName = json::getString(_object, OT_JSON_MEMBER_FileName);
    m_modelType = json::getString(_object, OT_JSON_MEMBER_ModelType);
    m_elementType = json::getString(_object, OT_JSON_MEMBER_ElementType);
    

    m_metaData.clear();
    ConstJsonObjectList metaData = json::getObjectList(_object, OT_JSON_MEMBER_MetaData);
    for (const ConstJsonObject& metaDataObj : metaData) {
        std::string k = json::getString(metaDataObj, OT_JSON_MEMBER_Key);
        std::string v = json::getString(metaDataObj, OT_JSON_MEMBER_Value);
        m_metaData.insert_or_assign(k, v);
    }

}
