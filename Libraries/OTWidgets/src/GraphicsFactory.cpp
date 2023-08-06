//! @file GraphicsFactory.cpp
//! 
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTWidgets/GraphicsFactory.h"
#include "OTWidgets/GraphicsItem.h"
#include "OTGui/GraphicsItemCfg.h"

namespace ot {
	namespace intern {
		
	}
}

ot::GraphicsItem* ot::GraphicsFactory::itemFromConfig(ot::GraphicsItemCfg* _configuration) {
	OTAssertNullptr(_configuration);

	return nullptr;
}
