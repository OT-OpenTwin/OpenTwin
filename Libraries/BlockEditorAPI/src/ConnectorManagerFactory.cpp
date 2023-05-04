// OpenTwin header
#include "OTBlockEditorAPI/ConnectorManagerFactory.h"
#include "OTBlockEditorAPI/BorderLayoutConnectorManager.h"
#include "OpenTwinCore/otAssert.h"
#include "OpenTwinCore/rJSONHelper.h"

ot::ConnectorManager* ot::ConnectorManagerFactory::connectorManagerConfigurationFromType(const std::string& _type) {
	if (_type == OT_BORDERLAYOUTCONNECTORMANAGERCONFIGURATION_TYPE) return new ot::BorderLayoutConnectorManager;
	else {
		otAssert(0, "Unknown block type");
		throw std::exception("Invalid type");
	}
}

ot::ConnectorManager* ot::ConnectorManagerFactory::connectorManagerConfigurationFromJson(OT_rJSON_val& _object) {
	OT_rJSON_checkMember(_object, OT_JSON_MEMBER_ConnectorManagerType, String);
	ot::ConnectorManager* newConnectorManager = connectorManagerConfigurationFromType(_object[OT_JSON_MEMBER_ConnectorManagerType].GetString());
	try {
		newConnectorManager->setFromJsonObject(_object);
		return newConnectorManager;
	}
	catch (const std::exception& _e) {
		delete newConnectorManager;
		throw _e;
	}
	catch (...) {
		delete newConnectorManager;
		throw std::exception("Fatal: Unknown error");
	}
}
