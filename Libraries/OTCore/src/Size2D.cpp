//! @file Size2D.cpp
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/Size2D.h"

#define OT_JSON_MEMBER_Width "w"
#define OT_JSON_MEMBER_Height "h"

void ot::Size2D::addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const {
	_object.AddMember(OT_JSON_MEMBER_Width, m_w, _allocator);
	_object.AddMember(OT_JSON_MEMBER_Height, m_h, _allocator);
}

void ot::Size2D::setFromJsonObject(const ConstJsonObject& _object) {
	m_w = json::getInt(_object, OT_JSON_MEMBER_Width);
	m_h = json::getInt(_object, OT_JSON_MEMBER_Height);
}

void ot::Size2DF::addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const {
	_object.AddMember(OT_JSON_MEMBER_Width, m_w, _allocator);
	_object.AddMember(OT_JSON_MEMBER_Height, m_h, _allocator);
}

void ot::Size2DF::setFromJsonObject(const ConstJsonObject& _object) {
	m_w = json::getFloat(_object, OT_JSON_MEMBER_Width);
	m_h = json::getFloat(_object, OT_JSON_MEMBER_Height);
}

void ot::Size2DD::addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const {
	_object.AddMember(OT_JSON_MEMBER_Width, m_w, _allocator);
	_object.AddMember(OT_JSON_MEMBER_Height, m_h, _allocator);
}

void ot::Size2DD::setFromJsonObject(const ConstJsonObject& _object) {
	m_w = json::getDouble(_object, OT_JSON_MEMBER_Width);
	m_h = json::getDouble(_object, OT_JSON_MEMBER_Height);
}