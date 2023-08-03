//! @file BlockConfigurationObject.cpp
//! @author Alexander Kuester (alexk95)
//! @date July 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OpenTwinCore/rJSONHelper.h"
#include "OTBlockEditorAPI/BlockConfigurationObject.h"

#define JSON_MEMBER_Name "Name"

void ot::BlockConfigurationObject::addToJsonObject(OT_rJSON_doc& _document, OT_rJSON_val& _object) const {
	ot::rJSON::add(_document, _object, JSON_MEMBER_Name, m_name);
}

void ot::BlockConfigurationObject::setFromJsonObject(OT_rJSON_val& _object) {
	OT_rJSON_checkMember(_object, JSON_MEMBER_Name, String);
	m_name = _object[JSON_MEMBER_Name].GetString();
}