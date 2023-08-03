//! @file ConnectorManagerConfigurationFactory.h
//! @author Alexander Kuester (alexk95)
//! @date March 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OpenTwinCore/rJSON.h"
#include "OTBlockEditorAPI/BlockEditorAPIAPIExport.h"

namespace ot {

	class BlockConnectorManagerConfiguration;

	namespace ConnectorManagerConfigurationFactory {

		//! @brief Creates an empty block layer configuration for the given type
		//! @param _type A valid block layer configuration type
		BLOCKEDITORAPI_API_EXPORT BlockConnectorManagerConfiguration* connectorManagerConfigurationFromType(const std::string& _type);

		//! @brief Creates a block layer configuration from the given JSON object
		//! @param _object The JSON object containing the block layer configuration
		BLOCKEDITORAPI_API_EXPORT BlockConnectorManagerConfiguration* connectorManagerConfigurationFromJson(OT_rJSON_val& _object);
	}
}
