//! @file GraphicsFactory.cpp
//! 
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTWidgets/GraphicsFactory.h"
#include "OTWidgets/GraphicsItem.h"
#include "OTGui/GraphicsItemCfg.h"
#include "OTCore/KeyMap.h"

ot::GraphicsItem* ot::GraphicsFactory::itemFromConfig(ot::GraphicsItemCfg* _configuration) {
	OTAssertNullptr(_configuration);

	if (!ot::GlobalKeyMap::instance().contains(_configuration->simpleFactoryObjectKey())) {
		OT_LOG_EA("No key found");
		return nullptr;
	}

	ot::GraphicsItem* itm = ot::SimpleFactory::instance().createType<ot::GraphicsItem>(
		ot::GlobalKeyMap::instance().get(
			_configuration->simpleFactoryObjectKey()));
	if (itm == nullptr) {
		OT_LOG_EA("Failed to create item from config");
		return nullptr;
	}
	if (!itm->setupFromConfig(_configuration)) {
		OT_LOG_EA("Setup from configuration failed");
		delete itm;
		return nullptr;
	}
	return itm;
}
