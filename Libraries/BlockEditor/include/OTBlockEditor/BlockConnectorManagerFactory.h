//! @file BlockConnectorManagerFactory.h
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTBlockEditor/BlockEditorAPIExport.h"

namespace ot {

	class BlockLayer;
	class BlockConnectorManager;
	class BlockConnectorManagerConfiguration;

	namespace BlockConnectorManagerFactory {

		BLOCK_EDITOR_API_EXPORT BlockConnectorManager* connectorManagerFromConfig(ot::BlockLayer * _layer, ot::BlockConnectorManagerConfiguration * _config);

	}

}