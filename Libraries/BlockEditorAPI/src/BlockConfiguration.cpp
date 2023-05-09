// OpenTwin header
#include "OTBlockEditorAPI/BlockConfiguration.h"
#include "OpenTwinCore/rJSONHelper.h"

#define JSON_MEMBER_Title "Title"
#define JSON_MEMBER_Type "Type"

ot::BlockConfiguration::BlockConfiguration(const std::string& _name) : BlockConfigurationGraphicsObject(_name), m_parentCategory(nullptr) {

}

ot::BlockConfiguration::~BlockConfiguration() {

}

void ot::BlockConfiguration::addToJsonObject(OT_rJSON_doc& _document, OT_rJSON_val& _object) const {
	BlockConfigurationGraphicsObject::addToJsonObject(_document, _object);
	ot::rJSON::add(_document, _object, JSON_MEMBER_Type, type());
	ot::rJSON::add(_document, _object, JSON_MEMBER_Title, m_title);
}

void ot::BlockConfiguration::setFromJsonObject(OT_rJSON_val& _object) {
	BlockConfigurationGraphicsObject::setFromJsonObject(_object);

	OT_rJSON_checkMember(_object, JSON_MEMBER_Title, String);
	m_title = _object[JSON_MEMBER_Title].GetString();
}
