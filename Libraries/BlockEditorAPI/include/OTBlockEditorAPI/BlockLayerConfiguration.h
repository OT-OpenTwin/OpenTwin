//! @file BlockLayerConfiguration.h
//! @author Alexander Kuester (alexk95)
//! @date March 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTBlockEditorAPI/BlockConfigurationGraphicsObject.h"

namespace ot {

	class BLOCKEDITORAPI_API_EXPORT BlockLayerConfiguration : public ot::BlockConfigurationGraphicsObject {
	public:
		BlockLayerConfiguration();
		virtual ~BlockLayerConfiguration();

		//! @brief Add the object contents to the provided JSON object
		//! @param _document The JSON document (used to get the allocator)
		//! @param _object The JSON object to add the contents to
		virtual void addToJsonObject(OT_rJSON_doc& _document, OT_rJSON_val& _object) const override;

		//! @brief Will set the object contents from the provided JSON object
		//! @param _object The JSON object containing the information
		//! @throw Will throw an exception if the provided object is not valid (members missing or invalid types)
		virtual void setFromJsonObject(OT_rJSON_val& _object) override;

		//! @brief Returns the layer configuration type
		virtual std::string layerType(void) const = 0;

	private:

		BlockLayerConfiguration(BlockLayerConfiguration&) = delete;
		BlockLayerConfiguration& operator = (BlockLayerConfiguration&) = delete;
	};
}