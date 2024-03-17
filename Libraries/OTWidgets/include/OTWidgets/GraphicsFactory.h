//! @file GraphicsFactory.h
//! 
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTWidgets/OTWidgetsAPIExport.h"

namespace ot {

	class GraphicsItem;
	class GraphicsItemCfg;

	namespace GraphicsFactory {

		//! @brief Will create a graphics item from the provided configuration
		//! @param _config The graphics item configuration
		//! @param _isRoot If true, the item will be added to a stack with a highlight item on top. The highlight item will be set to the root item
		OT_WIDGETS_API_EXPORT ot::GraphicsItem* itemFromConfig(ot::GraphicsItemCfg* _config, bool _isRoot = false);
	}

}
