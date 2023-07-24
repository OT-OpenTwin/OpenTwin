//! @file BlockFactory.cpp
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################


// OpenTwin header
#include "OTBlockEditor/BlockFactory.h"
#include "OTBlockEditor/LayerFactory.h"
#include "OTBlockEditor/DefaultBlock.h"

#include "OTBlockEditorAPI/BlockConfiguration.h"
#include "OTBlockEditorAPI/BlockLayerConfiguration.h"
#include "OTBlockEditorAPI/FlowBlockConfiguration.h"

#include "OTBlockEditorAPI/BlockLayers.h"

#include "OpenTwinCore/Logger.h"

namespace ot {
	namespace intern {

		//template <class T>
		void applyGeneralBlockSetting(ot::Block* _block, ot::BlockConfiguration* _config) {
			if (_config->isUserMoveable()) _block->setFlags(_block->flags() | QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable);
			_block->setWidthLimit(_config->widthLimits());
			_block->setHeightLimit(_config->heightLimits());
			_block->setPos(0., 0.);

			// Store block configuration (required to create copies of the block)
			OT_rJSON_createDOC(doc);
			_config->addToJsonObject(doc, doc);
			_block->setConfiguration(QByteArray::fromStdString(ot::rJSON::toJSON(doc)));
		}
	}
}


ot::Block* ot::BlockFactory::blockFromConfig(ot::BlockConfiguration* _config) {
	OTAssertNullptr(_config);

	if (_config->type() == OT_FLOWBLOCKCONFIGURATION_TYPE) {
		return blockFromConfig(dynamic_cast<ot::FlowBlockConfiguration*>(_config));
	}
	else if (_config->type() != OT_DEFAULTBLOCKCONFIGURATION_TYPE) {
		OT_LOG_EAS("Unknown block configuration type (type = \"" + _config->type() + "\")");
		return nullptr;
	}

	// Create default block
	ot::DefaultBlock* newBlock = new ot::DefaultBlock;
	ot::intern::applyGeneralBlockSetting(newBlock, _config);

	// Add layers
	for (auto layer : _config->layers()) {
		ot::BlockLayer* newLayer = LayerFactory::blockLayerFromConfig(newBlock, layer);
		if (newLayer) {
			newBlock->addLayer(newLayer);
		}
		else {
			OT_LOG_W("Failed to create layer from configuration, skipping layer");
		}
	}

	return newBlock;
}

ot::Block* ot::BlockFactory::blockFromConfig(ot::FlowBlockConfiguration* _config) {
	OTAssertNullptr(_config);

	otAssert(0, "zzZ");


	//ot::intern::applyGeneralBlockSetting(newBlock, _config);

	return nullptr;
}