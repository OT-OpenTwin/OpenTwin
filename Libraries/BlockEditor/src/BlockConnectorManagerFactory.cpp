//! @file BlockConnectorManagerFactory.cpp
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTBlockEditor/BlockConnectorManagerFactory.h"
#include "OTBlockEditor/BlockConnectorManager.h"
#include "OTBlockEditor/BlockConnectorFactory.h"
#include "OTBlockEditor/BlockConnector.h"
#include "OTBlockEditorAPI/BlockConnectorManagerConfiguration.h"
#include "OTBlockEditorAPI/BorderLayoutBlockConnectorManagerConfiguration.h"
#include "OpenTwinCore/otAssert.h"

namespace ot {
	namespace BlockConnectorManagerFactory {
		namespace intern {
			BlockConnectorManager* connectorManagerFromConfig(ot::BlockLayer* _layer, ot::BorderLayoutBlockConnectorManagerConfiguration* _config);
		}
	}
}

ot::BlockConnectorManager* ot::BlockConnectorManagerFactory::connectorManagerFromConfig(ot::BlockLayer* _layer, ot::BlockConnectorManagerConfiguration* _config) {
	OTAssertNullptr(_layer);
	OTAssertNullptr(_config);
	
	ot::BlockConnectorManager* cm = nullptr;

	if (_config->connectorManagerType() == OT_BORDERLAYOUTCONNECTORMANAGERCONFIGURATION_TYPE) {
		cm = intern::connectorManagerFromConfig(_layer, dynamic_cast<ot::BorderLayoutBlockConnectorManagerConfiguration*>(_config));
	}

	if (cm == nullptr) return cm;



	return cm;
}

ot::BlockConnectorManager* ot::BlockConnectorManagerFactory::intern::connectorManagerFromConfig(ot::BlockLayer* _layer, ot::BorderLayoutBlockConnectorManagerConfiguration* _config) {
	OTAssertNullptr(_layer);
	OTAssertNullptr(_config);

	BorderLayoutBlockConnectorManager* newCm = new BorderLayoutBlockConnectorManager(_layer);

	BlockConnectorLocationFlags defaultFlags(ot::ConnectorPushBack);

	for (auto t : _config->topConnectors()) {
		newCm->addConnector(BlockConnectorFactory::blockConnectorFromConfig(t), ot::ConnectorTop | ot::ConnectorPushBack);
	}
	for (auto l : _config->leftConnectors()) {
		newCm->addConnector(BlockConnectorFactory::blockConnectorFromConfig(l), ot::ConnectorLeft | ot::ConnectorPushBack);
	}
	for (auto r : _config->rightConnectors()) {
		newCm->addConnector(BlockConnectorFactory::blockConnectorFromConfig(r), ot::ConnectorRight | ot::ConnectorPushBack);
	}
	for (auto b : _config->bottomConnectors()) {
		newCm->addConnector(BlockConnectorFactory::blockConnectorFromConfig(b), ot::ConnectorBottom | ot::ConnectorPushBack);
	}

	return newCm;
}