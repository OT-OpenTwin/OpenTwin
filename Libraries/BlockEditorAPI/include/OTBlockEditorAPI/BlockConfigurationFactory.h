//! @file BlockConfigurationFactory.h
//! @author Alexander Kuester (alexk95)
//! @date March 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OpenTwinCore/rJSON.h"
#include "OTBlockEditorAPI/BlockEditorAPIAPIExport.h"

namespace ot {
	
	class BlockConfiguration;

	namespace BlockConfigurationFactory {

		//! @brief Creates an empty block configuration for the given type
		//! @param _type A valid block configuration type
		BLOCKEDITORAPI_API_EXPORT BlockConfiguration* blockConfigurationFromType(const std::string& _type);

		//! @brief Creates a block configuration from the given JSON object
		//! @param _object The JSON object containing the block configuration
		BLOCKEDITORAPI_API_EXPORT BlockConfiguration* blockConfigurationFromJson(OT_rJSON_val& _object);
	}
}