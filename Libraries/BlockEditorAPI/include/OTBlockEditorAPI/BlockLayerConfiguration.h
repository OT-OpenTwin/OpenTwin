//! @file BlockLayerConfiguration.h
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTBlockEditorAPI/BlockConfigurationGraphicsObject.h"

// std header
#include "vector"

#define OT_JSON_MEMBER_BlockLayerConfigurationType "BlockLayerConfigurationType"

namespace ot {

	class BlockConnectorManagerConfiguration;

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

		//! @brief Will set the currently used connector manager
		void setConnectorManager(ot::BlockConnectorManagerConfiguration* _manager);

		//! @brief Returns the currently used conenctor manager
		ot::BlockConnectorManagerConfiguration* getConnectorManager(void) { return m_connectorManager; };

	private:
		BlockConnectorManagerConfiguration* m_connectorManager;

		BlockLayerConfiguration(BlockLayerConfiguration&) = delete;
		BlockLayerConfiguration& operator = (BlockLayerConfiguration&) = delete;
	};
}