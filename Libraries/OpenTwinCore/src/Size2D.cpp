//! @file Size2D.cpp
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OpenTwinCore/Size2D.h"
#include "OpenTwinCore/rJSON.h"
#include "OpenTwinCore/rJSONHelper.h"

#define OT_JSON_MEMBER_Type "t"
#define OT_JSON_MEMBER_Width "w"
#define OT_JSON_MEMBER_Height "h"

#define OT_JSON_VALUE_IntType "i"
#define OT_JSON_VALUE_DoubleType "d"
#define OT_JSON_VALUE_FloatType "f"

void ot::Size2D::addToJsonObject(OT_rJSON_doc& _document, OT_rJSON_val& _object) const {
	Size2DTemplate::addToJsonObject(_document, _object);

	ot::rJSON::add(_document, _object, OT_JSON_MEMBER_Type, OT_JSON_VALUE_IntType);
	ot::rJSON::add(_document, _object, OT_JSON_MEMBER_Width, m_w);
	ot::rJSON::add(_document, _object, OT_JSON_MEMBER_Height, m_h);
}

void ot::Size2D::setFromJsonObject(OT_rJSON_val& _object) {
	Size2DTemplate::setFromJsonObject(_object);

	OT_rJSON_checkMember(_object, OT_JSON_MEMBER_Type, String);
	OT_rJSON_checkMember(_object, OT_JSON_MEMBER_Width, Int);
	OT_rJSON_checkMember(_object, OT_JSON_MEMBER_Height, Int);
	otAssert(_object[OT_JSON_MEMBER_Type] == OT_JSON_VALUE_IntType, "Invalid Size2D type");
	m_w = _object[OT_JSON_MEMBER_Width].GetInt();
	m_h = _object[OT_JSON_MEMBER_Height].GetInt();
}

void ot::Size2DF::addToJsonObject(OT_rJSON_doc& _document, OT_rJSON_val& _object) const {
	Size2DTemplate::addToJsonObject(_document, _object);

	ot::rJSON::add(_document, _object, OT_JSON_MEMBER_Type, OT_JSON_VALUE_FloatType);
	ot::rJSON::add(_document, _object, OT_JSON_MEMBER_Width, m_w);
	ot::rJSON::add(_document, _object, OT_JSON_MEMBER_Height, m_h);
}

void ot::Size2DF::setFromJsonObject(OT_rJSON_val& _object) {
	Size2DTemplate::setFromJsonObject(_object);

	OT_rJSON_checkMember(_object, OT_JSON_MEMBER_Type, String);
	OT_rJSON_checkMember(_object, OT_JSON_MEMBER_Width, Double);
	OT_rJSON_checkMember(_object, OT_JSON_MEMBER_Height, Double);
	otAssert(_object[OT_JSON_MEMBER_Type] == OT_JSON_VALUE_FloatType, "Invalid Size2D type");
	m_w = _object[OT_JSON_MEMBER_Width].GetFloat();
	m_h = _object[OT_JSON_MEMBER_Height].GetFloat();
}

void ot::Size2DD::addToJsonObject(OT_rJSON_doc& _document, OT_rJSON_val& _object) const {
	Size2DTemplate::addToJsonObject(_document, _object);

	ot::rJSON::add(_document, _object, OT_JSON_MEMBER_Type, OT_JSON_VALUE_DoubleType);
	ot::rJSON::add(_document, _object, OT_JSON_MEMBER_Width, m_w);
	ot::rJSON::add(_document, _object, OT_JSON_MEMBER_Height, m_h);
}

void ot::Size2DD::setFromJsonObject(OT_rJSON_val& _object) {
	Size2DTemplate::setFromJsonObject(_object);

	OT_rJSON_checkMember(_object, OT_JSON_MEMBER_Type, String);
	OT_rJSON_checkMember(_object, OT_JSON_MEMBER_Width, Double);
	OT_rJSON_checkMember(_object, OT_JSON_MEMBER_Height, Double);
	otAssert(_object[OT_JSON_MEMBER_Type] == OT_JSON_VALUE_DoubleType, "Invalid Size2D type");
	m_w = _object[OT_JSON_MEMBER_Width].GetDouble();
	m_h = _object[OT_JSON_MEMBER_Height].GetDouble();
}