//! @file GraphicsFactory.cpp
//! 
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/KeyMap.h"
#include "OTCore/Logger.h"
#include "OTGui/GraphicsItemCfg.h"
#include "OTWidgets/GraphicsItem.h"
#include "OTWidgets/GraphicsFactory.h"
#include "OTWidgets/GraphicsStackItem.h"
#include "OTWidgets/GraphicsHighlightItem.h"

ot::GraphicsItem* ot::GraphicsFactory::itemFromConfig(ot::GraphicsItemCfg* _configuration, bool _isRoot) {
	OTAssertNullptr(_configuration);

	if (!ot::GlobalKeyMap::instance().contains(_configuration->simpleFactoryObjectKey())) {
		OT_LOG_EA("No key found");
		return nullptr;
	}

	// Create item
	ot::GraphicsItem* itm = ot::SimpleFactory::instance().createType<ot::GraphicsItem>(
		ot::GlobalKeyMap::instance().get(
			_configuration->simpleFactoryObjectKey()));

	if (itm == nullptr) {
		OT_LOG_EA("Failed to create item from config");
		return nullptr;
	}

	// Setup item
	if (!itm->setupFromConfig(_configuration)) {
		OT_LOG_EA("Setup from configuration failed");
		delete itm;
		return nullptr;
	}

	// Create frame
	if (_isRoot) {
		ot::GraphicsStackItem* stck = dynamic_cast<ot::GraphicsStackItem*>(itm);
		if (stck) {
			stck->createHighlightItem();
			stck->addItem(stck->highlightItem(), false, true);
		}
		else {
			stck = new GraphicsStackItem;
			stck->setGraphicsItemFlags(itm->graphicsItemFlags());
			stck->setGraphicsItemName(itm->graphicsItemName());

			itm->setGraphicsItemName(itm->graphicsItemName() + "_oldRoot");

			stck->createHighlightItem();
			stck->addItem(itm, true, false);
			stck->addItem(stck->highlightItem(), false, true);

			itm = stck;
		}
	}

	return itm;
}
