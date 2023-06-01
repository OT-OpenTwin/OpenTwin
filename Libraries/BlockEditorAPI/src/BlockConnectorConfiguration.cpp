// OpenTwin header
#include "OpenTwinCore/rJSONHelper.h"
#include "OTBlockEditorAPI/BlockConnectorConfiguration.h"

#define JSON_MEMBER_Name "Name"
#define JSON_MEMBER_Title "Title"
#define JSON_MEMBER_TitleOrientation "TitleOrientation"
#define JSON_MEMBER_Style "Style"
#define JSON_MEMBER_FillColor "FillColor"
#define JSON_MEMBER_BorderColor "BorderColor"

ot::BlockConnectorConfiguration::BlockConnectorConfiguration() : m_titleOrientation(ot::OrientBottom), m_style(ot::ConnectorNone) {

}

ot::BlockConnectorConfiguration::~BlockConnectorConfiguration() {

}

void ot::BlockConnectorConfiguration::addToJsonObject(OT_rJSON_doc& _document, OT_rJSON_val& _object) const {
	BlockConfigurationGraphicsObject::addToJsonObject(_document, _object);
	ot::rJSON::add(_document, _object, JSON_MEMBER_Title, m_title);
	ot::rJSON::add(_document, _object, JSON_MEMBER_TitleOrientation, toString(m_titleOrientation));
	ot::rJSON::add(_document, _object, JSON_MEMBER_Style, toString(m_style));

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
	OT_rJSON_checkMember(_object, JSON_MEMBER_TitleOrientation, String);
	OT_rJSON_checkMember(_object, JSON_MEMBER_Style, String);
	OT_rJSON_checkMember(_object, JSON_MEMBER_FillColor, Object);
	OT_rJSON_checkMember(_object, JSON_MEMBER_BorderColor, Object);

	m_title = _object[JSON_MEMBER_Title].GetString();
	m_titleOrientation = stringToOrientation(_object[JSON_MEMBER_TitleOrientation].GetString());
	m_style = blockConnectorStyleFromString(_object[JSON_MEMBER_Style].GetString());

	OT_rJSON_val fillObj = _object[JSON_MEMBER_FillColor].GetObject();
	m_fillColor.setFromJsonObject(fillObj);

	OT_rJSON_val borderObj = _object[JSON_MEMBER_BorderColor].GetObject();
	m_borderColor.setFromJsonObject(borderObj);
}

// ########################################################################################################################################################
