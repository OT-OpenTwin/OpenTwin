#include "OTGui/LengthLimitation.h"
#include "OpenTwinCore/rJSONHelper.h"
#include "OpenTwinCore/otAssert.h"

void ot::LengthLimitation::addToJsonObject(OT_rJSON_doc& _document, OT_rJSON_val& _jsonObject) const {
	ot::rJSON::add(_document, _jsonObject, "Min", m_min);
	ot::rJSON::add(_document, _jsonObject, "MinIsSet", m_minSet);
	ot::rJSON::add(_document, _jsonObject, "Max", m_max);
	ot::rJSON::add(_document, _jsonObject, "MaxIsSet", m_maxSet);
	ot::rJSON::add(_document, _jsonObject, "Type", "int");
}

void ot::LengthLimitation::setFromJsonObject(OT_rJSON_val& _jsonObject) {
	OT_rJSON_checkMember(_jsonObject, "Min", Int);
	OT_rJSON_checkMember(_jsonObject, "MinIsSet", Bool);
	OT_rJSON_checkMember(_jsonObject, "Max", Int);
	OT_rJSON_checkMember(_jsonObject, "MaxIsSet", Bool);
	OT_rJSON_checkMember(_jsonObject, "Type", String);
	if (_jsonObject["Type"].GetString() != std::string("int")) {
		otAssert(0, "Invalid type");
		throw std::exception("Invalid LengthLimitation type provided");
	}
	m_min = _jsonObject["Min"].GetInt();
	m_minSet = _jsonObject["MinIsSet"].GetBool();
	m_max = _jsonObject["Max"].GetInt();
	m_maxSet = _jsonObject["MaxIsSet"].GetBool();
}

// #########################################################################################################################################################################################################

void ot::LengthLimitationF::addToJsonObject(OT_rJSON_doc& _document, OT_rJSON_val& _jsonObject) const {
	ot::rJSON::add(_document, _jsonObject, "Min", m_min);
	ot::rJSON::add(_document, _jsonObject, "MinIsSet", m_minSet);
	ot::rJSON::add(_document, _jsonObject, "Max", m_max);
	ot::rJSON::add(_document, _jsonObject, "MaxIsSet", m_maxSet);
	ot::rJSON::add(_document, _jsonObject, "Type", "float");
}

void ot::LengthLimitationF::setFromJsonObject(OT_rJSON_val& _jsonObject) {
	OT_rJSON_checkMember(_jsonObject, "Min", Float);
	OT_rJSON_checkMember(_jsonObject, "MinIsSet", Bool);
	OT_rJSON_checkMember(_jsonObject, "Max", Float);
	OT_rJSON_checkMember(_jsonObject, "MaxIsSet", Bool);
	OT_rJSON_checkMember(_jsonObject, "Type", String);
	if (_jsonObject["Type"].GetString() != std::string("float")) {
		otAssert(0, "Invalid type");
		throw std::exception("Invalid LengthLimitation type provided");
	}
	m_min = _jsonObject["Min"].GetFloat();
	m_minSet = _jsonObject["MinIsSet"].GetBool();
	m_max = _jsonObject["Max"].GetFloat();
	m_maxSet = _jsonObject["MaxIsSet"].GetBool();
}

// #########################################################################################################################################################################################################

void ot::LengthLimitationD::addToJsonObject(OT_rJSON_doc& _document, OT_rJSON_val& _jsonObject) const {
	ot::rJSON::add(_document, _jsonObject, "Min", m_min);
	ot::rJSON::add(_document, _jsonObject, "MinIsSet", m_minSet);
	ot::rJSON::add(_document, _jsonObject, "Max", m_max);
	ot::rJSON::add(_document, _jsonObject, "MaxIsSet", m_maxSet);
	ot::rJSON::add(_document, _jsonObject, "Type", "double");
}

void ot::LengthLimitationD::setFromJsonObject(OT_rJSON_val& _jsonObject) {
	OT_rJSON_checkMember(_jsonObject, "Min", Double);
	OT_rJSON_checkMember(_jsonObject, "MinIsSet", Bool);
	OT_rJSON_checkMember(_jsonObject, "Max", Double);
	OT_rJSON_checkMember(_jsonObject, "MaxIsSet", Bool);
	OT_rJSON_checkMember(_jsonObject, "Type", String);
	if (_jsonObject["Type"].GetString() != std::string("double")) {
		otAssert(0, "Invalid type");
		throw std::exception("Invalid LengthLimitation type provided");
	}
	m_min = _jsonObject["Min"].GetDouble();
	m_minSet = _jsonObject["MinIsSet"].GetBool();
	m_max = _jsonObject["Max"].GetDouble();
	m_maxSet = _jsonObject["MaxIsSet"].GetBool();
}
