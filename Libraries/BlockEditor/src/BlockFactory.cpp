//! @file BlockFactory.cpp
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################


// OpenTwin header
#include "OTBlockEditor/BlockFactory.h"
#include "OTBlockEditor/DefaultBlock.h"

#include "OTBlockEditorAPI/BlockConfiguration.h"
#include "OTBlockEditorAPI/BlockLayerConfiguration.h"
#include "OTBlockEditorAPI/FlowBlockConfiguration.h"

#include "OpenTwinCore/Logger.h"

ot::Block* ot::BlockFactory::blockFromConfig(ot::BlockConfiguration* _config) {
	otAssert(_config, "nullptr provided");

	if (_config->type() == OT_FLOWBLOCKCONFIGURATION_TYPE) {
		return blockFromConfig(dynamic_cast<ot::FlowBlockConfiguration*>(_config));
	}
	else if (_config->type() != OT_DEFAULTBLOCKCONFIGURATION_TYPE) {
		OT_LOG_EAS("Unknown block configuration type (type = \"" + _config->type() + "\")");
		return nullptr;
	}

	// Create default block
	ot::DefaultBlock* newBlock = new ot::DefaultBlock;

	// Add layers
	for (auto layer : _config->layers()) {

	}

	return newBlock;
}

ot::Block* ot::BlockFactory::blockFromConfig(ot::FlowBlockConfiguration* _config) {
	otAssert(_config, "nullptr provided");

	otAssert(0, "zzZ");
	return nullptr;
}