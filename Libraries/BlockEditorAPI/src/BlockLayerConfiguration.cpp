// OpenTwin header
#include "OTBlockEditorAPI/BlockLayerConfiguration.h"
#include "OTBlockEditorAPI/BlockConnectorManagerConfiguration.h"
#include "OTBlockEditorAPI/BlockConnectorManagerConfigurationFactory.h"
#include "OpenTwinCore/rJSONHelper.h"
#include "OpenTwinCore/otAssert.h"

#define OT_JSON_MEMBER_ConnectorManager "ConnectorManager"

ot::BlockLayerConfiguration::BlockLayerConfiguration() : m_connectorManager(nullptr) {}

ot::BlockLayerConfiguration::~BlockLayerConfiguration() {
	if (m_connectorManager) delete m_connectorManager;
}

void ot::BlockLayerConfiguration::addToJsonObject(OT_rJSON_doc& _document, OT_rJSON_val& _object) const {
	BlockConfigurationGraphicsObject::addToJsonObject(_document, _object);
	ot::rJSON::add(_document, _object, OT_JSON_MEMBER_BlockLayerConfigurationType, layerType());

	if (m_connectorManager) {
		OT_rJSON_createValueObject(connectorManagerObj);
		m_connectorManager->addToJsonObject(_document, connectorManagerObj);
		ot::rJSON::add(_document, _object, OT_JSON_MEMBER_ConnectorManager, connectorManagerObj);
	}
	else {
		OT_rJSON_createValueNull(connectorManagerObj);
		ot::rJSON::add(_document, _object, OT_JSON_MEMBER_ConnectorManager, connectorManagerObj);
	}
}

void ot::BlockLayerConfiguration::setFromJsonObject(OT_rJSON_val& _object) {
	BlockConfigurationGraphicsObject::setFromJsonObject(_object);
	OT_rJSON_checkMember(_object, OT_JSON_MEMBER_BlockLayerConfigurationType, String);
	OT_rJSON_checkMemberExists(_object, OT_JSON_MEMBER_ConnectorManager);

	std::string type = _object[OT_JSON_MEMBER_BlockLayerConfigurationType].GetString();
	if (type != layerType()) {
		otAssert(0, "Invalid Block Layer Configuration Type for import");
		OT_LOG_E("Invalid type \"" + type + "\", expected: \"" + layerType() + "\"");
		throw std::exception("Invalid block layer cnfiguration type");
	}

	if (_object[OT_JSON_MEMBER_ConnectorManager].IsObject()) {
		OT_rJSON_val connectorManagerObj = _object[OT_JSON_MEMBER_ConnectorManager].GetObject();
		setConnectorManager(ConnectorManagerConfigurationFactory::connectorManagerConfigurationFromJson(connectorManagerObj));
	}
	else if (_object[OT_JSON_MEMBER_ConnectorManager].IsNull()) {
		setConnectorManager(nullptr);
	}
}

void ot::BlockLayerConfiguration::setConnectorManager(ot::BlockConnectorManagerConfiguration* _manager) {
	if (m_connectorManager == _manager) return;
	if (m_connectorManager) delete m_connectorManager;
	m_connectorManager = _manager;
}
