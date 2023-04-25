// OpenTwin header
#include "OTBlockEditorAPI/BlockConfiguration.h"
#include "OpenTwinCore/rJSONHelper.h"

#define JSON_MEMBER_Name "Name"
#define JSON_MEMBER_Type "Type"

ot::BlockConfiguration::BlockConfiguration(const std::string& _name) : BlockConfigurationGraphicsObject(_name), m_parentCategory(nullptr) {

}

ot::BlockConfiguration::~BlockConfiguration() {

}

void ot::BlockConfiguration::addToJsonObject(OT_rJSON_doc& _document, OT_rJSON_val& _object) const {
	BlockConfigurationGraphicsObject::addToJsonObject(_document, _object);
	ot::rJSON::add(_document, _object, JSON_MEMBER_Type, type());
}

void ot::BlockConfiguration::setFromJsonObject(OT_rJSON_val& _object) {
	BlockConfigurationGraphicsObject::setFromJsonObject(_object);
}
