//! @file Size2D.cpp
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OpenTwinCore/Size2D.h"
#include "OpenTwinCore/rJSONHelper.h"

void ot::Size2D::addToJsonObject(OT_rJSON_doc& _document, OT_rJSON_val& _object) const {
	ot::rJSON::add(_document, _object, "t", "i");
	ot::rJSON::add(_document, _object, "w", m_w);
	ot::rJSON::add(_document, _object, "h", m_h);
}

void ot::Size2D::setFromJsonObject(OT_rJSON_val& _object) {
	OT_rJSON_checkMember(_object, "t", String);
	OT_rJSON_checkMember(_object, "w", Double);
	OT_rJSON_checkMember(_object, "h", Double);
	otAssert(_object["t"] == "i", "Invalid Size2D type");
	m_w = _object["w"].GetInt();
	m_h = _object["h"].GetInt();
}

void ot::Size2DF::addToJsonObject(OT_rJSON_doc& _document, OT_rJSON_val& _object) const {
	ot::rJSON::add(_document, _object, "t", "f");
	ot::rJSON::add(_document, _object, "w", m_w);
	ot::rJSON::add(_document, _object, "h", m_h);
}

void ot::Size2DF::setFromJsonObject(OT_rJSON_val& _object) {
	OT_rJSON_checkMember(_object, "t", String);
	OT_rJSON_checkMember(_object, "w", Double);
	OT_rJSON_checkMember(_object, "h", Double);
	otAssert(_object["t"] == "f", "Invalid Size2D type");
	m_w = _object["w"].GetFloat();
	m_h = _object["h"].GetFloat();
}

void ot::Size2DD::addToJsonObject(OT_rJSON_doc& _document, OT_rJSON_val& _object) const {
	ot::rJSON::add(_document, _object, "t", "d");
	ot::rJSON::add(_document, _object, "w", m_w);
	ot::rJSON::add(_document, _object, "h", m_h);
}

void ot::Size2DD::setFromJsonObject(OT_rJSON_val& _object) {
	OT_rJSON_checkMember(_object, "t", String);
	OT_rJSON_checkMember(_object, "w", Double);
	OT_rJSON_checkMember(_object, "h", Double);
	otAssert(_object["t"] == "d", "Invalid Size2D type");
	m_w = _object["w"].GetDouble();
	m_h = _object["h"].GetDouble();
}