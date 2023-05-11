//! @file BlockFactory.h
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTBlockEditor/BlockEditorAPIExport.h"

namespace ot {

	class Block;
	class BlockConfiguration;
	class FlowBlockConfiguration;

	namespace BlockFactory {

		BLOCK_EDITOR_API_EXPORT ot::Block* blockFromConfig(ot::BlockConfiguration* _config);
		BLOCK_EDITOR_API_EXPORT ot::Block* blockFromConfig(ot::FlowBlockConfiguration* _config);

	}

}