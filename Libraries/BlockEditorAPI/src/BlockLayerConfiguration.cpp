// OpenTwin header
#include "OTBlockEditorAPI/BlockLayerConfiguration.h"
#include "OpenTwinCore/rJSONHelper.h"
#include "OpenTwinCore/otAssert.h"

#define OT_JSON_MEMBER_Type "Type"

ot::BlockLayerConfiguration::BlockLayerConfiguration() {}

ot::BlockLayerConfiguration::~BlockLayerConfiguration() {}

void ot::BlockLayerConfiguration::addToJsonObject(OT_rJSON_doc& _document, OT_rJSON_val& _object) const {
	BlockConfigurationGraphicsObject::addToJsonObject(_document, _object);
	ot::rJSON::add(_document, _object, OT_JSON_MEMBER_Type, layerType());
}

void ot::BlockLayerConfiguration::setFromJsonObject(OT_rJSON_val& _object) {
	BlockConfigurationGraphicsObject::setFromJsonObject(_object);
	OT_rJSON_checkMember(_object, OT_JSON_MEMBER_Type, String);
	std::string type = _object[OT_JSON_MEMBER_Type].GetString();
	if (type != layerType()) {
		otAssert(0, "Invalid Block Layer Configuration Type for import");
		OT_LOG_E("Invalid type \"" + type + "\", expected: \"" + layerType() + "\"");
		throw std::exception("Invalid block layer cnfiguration type");
	}
}
