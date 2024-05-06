//! @file GraphicsItemCfgFactory.cpp
//! 
//! @author Alexander Kuester (alexk95)
//! @date May 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTGui/GraphicsItemCfgFactory.h"

ot::GraphicsItemCfgFactory& ot::GraphicsItemCfgFactory::instance(void) {
	static GraphicsItemCfgFactory g_instance;
	return g_instance;
}
