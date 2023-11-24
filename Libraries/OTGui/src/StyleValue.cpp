//! @file StyleValue.cpp
//! @author Alexander Kuester (alexk95)
//! @date November 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/rJSONHelper.h"
#include "OTGui/StyleValue.h"

#define OT_JSON_MEMBER_Key "Key"
#define OT_JSON_MEMBER_Qss "Qss"
#define OT_JSON_MEMBER_Color "Color"

ot::StyleValue::StyleValue() {}

ot::StyleValue::StyleValue(const StyleValue& _other) 
	: m_key(_other.m_key), m_qss(_other.m_qss), m_color(_other.m_color)
{}

ot::StyleValue::~StyleValue() {}

ot::StyleValue& ot::StyleValue::operator = (const StyleValue& _other) {
	m_key = _other.m_key;
	m_qss = _other.m_qss;
	m_color = _other.m_color;
	return *this;
}

void ot::StyleValue::addToJsonObject(OT_rJSON_doc& _document, OT_rJSON_val& _object) const {
	ot::rJSON::add(_document, _object, OT_JSON_MEMBER_Key, m_key);
	ot::rJSON::add(_document, _object, OT_JSON_MEMBER_Qss, m_qss);
	OT_rJSON_createValueObject(colorObj);
	m_color.addToJsonObject(_document, colorObj);
	ot::rJSON::add(_document, _object, OT_JSON_MEMBER_Color, colorObj);
}

void ot::StyleValue::setFromJsonObject(OT_rJSON_val& _object) {
	OT_rJSON_checkMember(_object, OT_JSON_MEMBER_Key, String);
	OT_rJSON_checkMember(_object, OT_JSON_MEMBER_Qss, String);
	OT_rJSON_checkMember(_object, OT_JSON_MEMBER_Color, Object);

	m_key = _object[OT_JSON_MEMBER_Key].GetString();
	m_qss = _object[OT_JSON_MEMBER_Qss].GetString();
	
	OT_rJSON_val colorObj = _object[OT_JSON_MEMBER_Color].GetObject();
	m_color.setFromJsonObject(colorObj);
}