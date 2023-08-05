//! @file BlockConnectorFactory.cpp
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTBlockEditor/BlockConnectorFactory.h"
#include "OTBlockEditor/BlockConnector.h"
#include "OTBlockEditorAPI/BlockConnectorConfiguration.h"
#include "OpenTwinCore/otAssert.h"

ot::BlockConnector* ot::BlockConnectorFactory::blockConnectorFromConfig(ot::BlockConnectorConfiguration* _config) {
	OTAssertNullptr(_config);

	ot::BlockConnector* newConnector = new ot::BlockConnector;

	newConnector->setTitle(QString::fromStdString(_config->title()));
	newConnector->setTitleAlignment(_config->titleAlignment());
	newConnector->setConnectorStyle(_config->style());
	newConnector->setFillColor(QColor(_config->fillColor().rInt(), _config->fillColor().gInt(), _config->fillColor().bInt(), _config->fillColor().aInt()));
	newConnector->setBorderColor(QColor(_config->borderColor().rInt(), _config->borderColor().gInt(), _config->borderColor().bInt(), _config->borderColor().aInt()));

	return newConnector;
}