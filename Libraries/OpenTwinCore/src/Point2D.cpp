//! @file Point2D.cpp
//! @author Alexander Kuester (alexk95)
//! @date April 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OpenTwinCore/Point2D.h"
#include "OpenTwinCore/rJSONHelper.h"

void ot::Point2D::addToJsonObject(OT_rJSON_doc& _document, OT_rJSON_val& _object) const {
	ot::rJSON::add(_document, _object, "t", "i");
	ot::rJSON::add(_document, _object, "x", m_x);
	ot::rJSON::add(_document, _object, "y", m_y);
}

void ot::Point2D::setFromJsonObject(OT_rJSON_val& _object) {
	OT_rJSON_checkMember(_object, "t", String);
	OT_rJSON_checkMember(_object, "x", Double);
	OT_rJSON_checkMember(_object, "y", Double);
	otAssert(_object["t"] == "i", "Invalid Point2D type");
	m_x = _object["x"].GetInt();
	m_y = _object["y"].GetInt();
}

void ot::Point2DF::addToJsonObject(OT_rJSON_doc& _document, OT_rJSON_val& _object) const {
	ot::rJSON::add(_document, _object, "t", "f");
	ot::rJSON::add(_document, _object, "x", m_x);
	ot::rJSON::add(_document, _object, "y", m_y);
}

void ot::Point2DF::setFromJsonObject(OT_rJSON_val& _object) {
	OT_rJSON_checkMember(_object, "t", String);
	OT_rJSON_checkMember(_object, "x", Double);
	OT_rJSON_checkMember(_object, "y", Double);
	otAssert(_object["t"] == "f", "Invalid Point2D type");
	m_x = _object["x"].GetFloat();
	m_y = _object["y"].GetFloat();
}

void ot::Point2DD::addToJsonObject(OT_rJSON_doc& _document, OT_rJSON_val& _object) const {
	ot::rJSON::add(_document, _object, "t", "d");
	ot::rJSON::add(_document, _object, "x", m_x);
	ot::rJSON::add(_document, _object, "y", m_y);
}

void ot::Point2DD::setFromJsonObject(OT_rJSON_val& _object) {
	OT_rJSON_checkMember(_object, "t", String);
	OT_rJSON_checkMember(_object, "x", Double);
	OT_rJSON_checkMember(_object, "y", Double);
	otAssert(_object["t"] == "d", "Invalid Point2D type");
	m_x = _object["x"].GetDouble();
	m_y = _object["y"].GetDouble();
}