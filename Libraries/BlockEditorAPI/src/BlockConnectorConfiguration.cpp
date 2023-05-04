// OpenTwin header
#include "OpenTwinCore/rJSONHelper.h"
#include "OTBlockEditorAPI/BlockConnectorConfiguration.h"

#define JSON_MEMBER_Name "Name"
#define JSON_MEMBER_Title "Title"
#define JSON_MEMBER_TitlePosition "TitlePosition"
#define JSON_MEMBER_Tags "Tags"
#define JSON_MEMBER_Style "Style"
#define JSON_MEMBER_ImageSubPath "ImageSubPath"
#define JSON_MEMBER_FillColor "FillColor"
#define JSON_MEMBER_BorderColor "BorderColor"

ot::BlockConnectorConfiguration::BlockConnectorConfiguration() : m_titlePosition(ot::ComponentBottom), m_style(ot::ConnectorNone) {

}

ot::BlockConnectorConfiguration::~BlockConnectorConfiguration() {

}

void ot::BlockConnectorConfiguration::addToJsonObject(OT_rJSON_doc& _document, OT_rJSON_val& _object) const {
	BlockConfigurationGraphicsObject::addToJsonObject(_document, _object);
	ot::rJSON::add(_document, _object, JSON_MEMBER_Title, m_title);
	ot::rJSON::add(_document, _object, JSON_MEMBER_TitlePosition, toString(m_titlePosition));

	OT_rJSON_createValueArray(tagArr);
	for (auto t : m_tags) {
		tagArr.PushBack(rapidjson::Value(t.c_str(), _document.GetAllocator()), _document.GetAllocator());
	}
	ot::rJSON::add(_document, _object, JSON_MEMBER_Tags, tagArr);
	ot::rJSON::add(_document, _object, JSON_MEMBER_Style, toString(m_style));
	ot::rJSON::add(_document, _object, JSON_MEMBER_ImageSubPath, m_imageSubPath);

	OT_rJSON_createValueObject(fillObj);
	m_fillColor.addToJsonObject(_document, fillObj);
	ot::rJSON::add(_document, _object, JSON_MEMBER_FillColor, fillObj);

	OT_rJSON_createValueObject(borderObj);
	m_fillColor.addToJsonObject(_document, borderObj);
	ot::rJSON::add(_document, _object, JSON_MEMBER_BorderColor, borderObj);
}

void ot::BlockConnectorConfiguration::setFromJsonObject(OT_rJSON_val& _object) {
	BlockConfigurationGraphicsObject::setFromJsonObject(_object);
	OT_rJSON_checkMember(_object, JSON_MEMBER_Name, String);
	OT_rJSON_checkMember(_object, JSON_MEMBER_Title, String);
	OT_rJSON_checkMember(_object, JSON_MEMBER_TitlePosition, String);
	OT_rJSON_checkMember(_object, JSON_MEMBER_Tags, Array);
	OT_rJSON_checkMember(_object, JSON_MEMBER_Style, String);
	OT_rJSON_checkMember(_object, JSON_MEMBER_ImageSubPath, String);
	OT_rJSON_checkMember(_object, JSON_MEMBER_FillColor, Object);
	OT_rJSON_checkMember(_object, JSON_MEMBER_BorderColor, Object);

	m_tags.clear();
	
	m_title = _object[JSON_MEMBER_Title].GetString();
	m_titlePosition = blockComponentPositionFromString(_object[JSON_MEMBER_TitlePosition].GetString());
	m_style = blockConnectorStyleFromString(_object[JSON_MEMBER_Style].GetString());
	m_imageSubPath = _object[JSON_MEMBER_ImageSubPath].GetString();

	OT_rJSON_val tagArr = _object[JSON_MEMBER_Tags].GetArray();
	for (rapidjson::SizeType i = 0; i < tagArr.Size(); i++) {
		OT_rJSON_checkArrayEntryType(tagArr, i, String);
		m_tags.push_back(tagArr[i].GetString());
	}

	OT_rJSON_val fillObj = _object[JSON_MEMBER_FillColor].GetObject();
	m_fillColor.setFromJsonObject(fillObj);

	OT_rJSON_val borderObj = _object[JSON_MEMBER_BorderColor].GetObject();
	m_borderColor.setFromJsonObject(borderObj);
}

// ########################################################################################################################################################

// Setter/Getter

void ot::BlockConnectorConfiguration::addTag(const std::string& _tag) {
	m_tags.push_back(_tag);
}

void ot::BlockConnectorConfiguration::addTags(const std::list<std::string>& _tags) {
	for (auto t : _tags) {
		m_tags.push_back(t);
	}
}

void ot::BlockConnectorConfiguration::setTags(const std::list<std::string>& _tags) {
	m_tags = _tags;
}