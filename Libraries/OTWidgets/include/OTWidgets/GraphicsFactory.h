//! @file GraphicsFactory.h
//! 
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

namespace ot {

	class GraphicsItem;
	class GraphicsItemCfg;

	namespace GraphicsFactory {

		//! @brief Will create a graphics item from the provided configuration
		//! @param _config The graphics item configuration
		ot::GraphicsItem* itemFromConfig(ot::GraphicsItemCfg* _config);
	}

}
