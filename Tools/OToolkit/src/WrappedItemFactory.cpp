//! @file WrappedItemFactory.cpp
//! @author Alexander Kuester (alexk95)
//! @date June 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OToolkit header
#include "WrappedItemFactory.h"
#include "GraphicsItemDesignerItemBase.h"

WrappedItemFactory& WrappedItemFactory::instance(void) {
	static WrappedItemFactory g_instance;
	return g_instance;
}

GraphicsItemDesignerItemBase* WrappedItemFactory::createFromConfig(const ot::GraphicsItemCfg* _config) {
	OTAssertNullptr(_config);
	GraphicsItemDesignerItemBase* newItem = WrappedItemFactory::instance().createFromKey(_config->getFactoryKey());
	if (!newItem) {
		OT_LOG_E("Factory failed");
		return newItem;
	}

	newItem->setupDesignerItemFromConfig(_config);

	return newItem;
}