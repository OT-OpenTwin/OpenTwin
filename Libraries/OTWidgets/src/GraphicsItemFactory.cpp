//! @file GraphicsItemFactory.cpp
//! 
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTGui/GraphicsItemCfg.h"
#include "OTGui/GraphicsItemCfgFactory.h"
#include "OTWidgets/GraphicsItem.h"
#include "OTWidgets/GraphicsItemFactory.h"
#include "OTWidgets/GraphicsStackItem.h"
#include "OTWidgets/GraphicsHighlightItem.h"

ot::GraphicsItemFactory& ot::GraphicsItemFactory::instance(void) {
	static GraphicsItemFactory g_instance;
	return g_instance;
}

ot::GraphicsItem* ot::GraphicsItemFactory::create(const ConstJsonObject& _configObject, bool _isRoot) {
	// Create configuration
	GraphicsItemCfg* cfg = GraphicsItemCfgFactory::instance().create(_configObject);
	if (!cfg) return nullptr;

	// Create item
	return this->itemFromConfig(cfg, _isRoot);
}

ot::GraphicsItem* ot::GraphicsItemFactory::itemFromConfig(ot::GraphicsItemCfg* _configuration, bool _isRoot) {
	OTAssertNullptr(_configuration);
	// Create item
	GraphicsItem* itm = this->createFromKey(_configuration->getFactoryKey());
	if (!itm) return nullptr;
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
			stck->setGraphicsItemFlags(itm->getGraphicsItemFlags());
			stck->setGraphicsItemName(itm->getGraphicsItemName() + "_oldRoot");
			stck->setGraphicsItemUid(itm->getGraphicsItemUid());

			stck->createHighlightItem();
			stck->addItem(itm, true, false);
			stck->addItem(stck->highlightItem(), false, true);

			itm = stck;
		}
	}

	return itm;
}
