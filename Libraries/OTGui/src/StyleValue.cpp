//! @file StyleValue.cpp
//! @author Alexander Kuester (alexk95)
//! @date November 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
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

void ot::StyleValue::addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const {
	_object.AddMember(OT_JSON_MEMBER_Key, JsonString(m_key, _allocator), _allocator);
	_object.AddMember(OT_JSON_MEMBER_Qss, JsonString(m_qss, _allocator), _allocator);

	JsonObject colorObj;
	m_color.addToJsonObject(colorObj, _allocator);
	_object.AddMember(OT_JSON_MEMBER_Color, colorObj, _allocator);
}

void ot::StyleValue::setFromJsonObject(const ConstJsonObject& _object) {
	m_key = json::getString(_object, OT_JSON_MEMBER_Key);
	m_qss = json::getString(_object, OT_JSON_MEMBER_Qss);
	
	m_color.setFromJsonObject(json::getObject(_object, OT_JSON_MEMBER_Color));
}