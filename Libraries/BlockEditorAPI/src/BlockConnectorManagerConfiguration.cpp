// OpenTwin header
#include "OTBlockEditorAPI/BlockConnectorManagerConfiguration.h"
#include "OpenTwinCore/rJSONHelper.h"
#include "OpenTwinCore/otAssert.h"
#include "OpenTwinCore/Logger.h"

void ot::BlockConnectorManagerConfiguration::addToJsonObject(OT_rJSON_doc& _document, OT_rJSON_val& _object) const {
	ot::rJSON::add(_document, _object, OT_JSON_MEMBER_ConnectorManagerType, connectorManagerType());
}

void ot::BlockConnectorManagerConfiguration::setFromJsonObject(OT_rJSON_val& _object) {
	OT_rJSON_checkMember(_object, OT_JSON_MEMBER_ConnectorManagerType, String);
	std::string type = _object[OT_JSON_MEMBER_ConnectorManagerType].GetString();
	if (type != this->connectorManagerType()) {
		otAssert(0, "Invalid Connector Manager Configuration Type for import");
		OT_LOG_E("Invalid type \"" + type + "\", expected: \"" + connectorManagerType() + "\"");
		throw std::exception("Invalid block layer cnfiguration type");
	}
}
