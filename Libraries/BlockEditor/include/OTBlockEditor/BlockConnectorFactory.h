//! @file BlockConnectorFactory.h
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTBlockEditor/BlockEditorAPIExport.h"

namespace ot {

	class BlockConnector;
	class BlockConnectorConfiguration;

	namespace BlockConnectorFactory {

		BLOCK_EDITOR_API_EXPORT ot::BlockConnector* blockConnectorFromConfig(ot::BlockConnectorConfiguration* _config);

	}
}