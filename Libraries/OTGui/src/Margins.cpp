#include "OTGui/Margins.h"
#include "OTCore/rJSONHelper.h"
#include "OTCore/OTAssert.h"

void ot::Margins::addToJsonObject(OT_rJSON_doc& _document, OT_rJSON_val& _jsonObject) const {
	ot::rJSON::add(_document, _jsonObject, "Top", m_top);
	ot::rJSON::add(_document, _jsonObject, "Right", m_right);
	ot::rJSON::add(_document, _jsonObject, "Bottom", m_bottom);
	ot::rJSON::add(_document, _jsonObject, "Left", m_left);
	ot::rJSON::add(_document, _jsonObject, "Type", "int");
}

void ot::Margins::setFromJsonObject(OT_rJSON_val& _jsonObject) {
	OT_rJSON_checkMember(_jsonObject, "Top", Int);
	OT_rJSON_checkMember(_jsonObject, "Right", Int);
	OT_rJSON_checkMember(_jsonObject, "Bottom", Int);
	OT_rJSON_checkMember(_jsonObject, "Left", Int);
	OT_rJSON_checkMember(_jsonObject, "Type", String);
	if (_jsonObject["Type"].GetString() != std::string("int")) {
		OTAssert(0, "Invalid type");
		throw std::exception("Invalid Margins type provided");
	}
	m_top = _jsonObject["Top"].GetInt();
	m_right = _jsonObject["Right"].GetInt();
	m_bottom = _jsonObject["Bottom"].GetInt();
	m_left = _jsonObject["Left"].GetInt();
}

void ot::MarginsF::addToJsonObject(OT_rJSON_doc& _document, OT_rJSON_val& _jsonObject) const {
	ot::rJSON::add(_document, _jsonObject, "Top", m_top);
	ot::rJSON::add(_document, _jsonObject, "Right", m_right);
	ot::rJSON::add(_document, _jsonObject, "Bottom", m_bottom);
	ot::rJSON::add(_document, _jsonObject, "Left", m_left);
	ot::rJSON::add(_document, _jsonObject, "Type", "float");
}

void ot::MarginsF::setFromJsonObject(OT_rJSON_val& _jsonObject) {
	OT_rJSON_checkMember(_jsonObject, "Top", Float);
	OT_rJSON_checkMember(_jsonObject, "Right", Float);
	OT_rJSON_checkMember(_jsonObject, "Bottom", Float);
	OT_rJSON_checkMember(_jsonObject, "Left", Float);
	OT_rJSON_checkMember(_jsonObject, "Type", String);
	if (_jsonObject["Type"].GetString() != std::string("float")) {
		OTAssert(0, "Invalid type");
		throw std::exception("Invalid Margins type provided");
	}
	m_top = _jsonObject["Top"].GetFloat();
	m_right = _jsonObject["Right"].GetFloat();
	m_bottom = _jsonObject["Bottom"].GetFloat();
	m_left = _jsonObject["Left"].GetFloat();
}

void ot::MarginsD::addToJsonObject(OT_rJSON_doc& _document, OT_rJSON_val& _jsonObject) const {
	ot::rJSON::add(_document, _jsonObject, "Top", m_top);
	ot::rJSON::add(_document, _jsonObject, "Right", m_right);
	ot::rJSON::add(_document, _jsonObject, "Bottom", m_bottom);
	ot::rJSON::add(_document, _jsonObject, "Left", m_left);
	ot::rJSON::add(_document, _jsonObject, "Type", "double");
}

void ot::MarginsD::setFromJsonObject(OT_rJSON_val& _jsonObject) {
	OT_rJSON_checkMember(_jsonObject, "Top", Double);
	OT_rJSON_checkMember(_jsonObject, "Right", Double);
	OT_rJSON_checkMember(_jsonObject, "Bottom", Double);
	OT_rJSON_checkMember(_jsonObject, "Left", Double);
	OT_rJSON_checkMember(_jsonObject, "Type", String);
	if (_jsonObject["Type"].GetString() != std::string("double")) {
		OTAssert(0, "Invalid type");
		throw std::exception("Invalid Margins type provided");
	}
	m_top = _jsonObject["Top"].GetDouble();
	m_right = _jsonObject["Right"].GetDouble();
	m_bottom = _jsonObject["Bottom"].GetDouble();
	m_left = _jsonObject["Left"].GetDouble();
}