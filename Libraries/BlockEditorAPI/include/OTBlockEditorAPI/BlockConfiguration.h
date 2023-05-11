//! @file BlockConfiguration.h
//! @author Alexander Kuester (alexk95)
//! @date March 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTBlockEditorAPI/BlockEditorAPIAPIExport.h"
#include "OTBlockEditorAPI/BlockConfigurationGraphicsObject.h"

// std header
#include <string>
#include <vector>

#pragma warning (disable:4251)

#define OT_DEFAULTBLOCKCONFIGURATION_TYPE "DefaultBlock"

namespace ot {

	class BlockCategoryConfiguration;
	class BlockLayerConfiguration;

	class BLOCKEDITORAPI_API_EXPORT BlockConfiguration : public ot::BlockConfigurationGraphicsObject {
	public:
		BlockConfiguration(const std::string& _name = std::string(), const std::string& _title = std::string());
		virtual ~BlockConfiguration();

		//! @brief Add the object contents to the provided JSON object
		//! @param _document The JSON document (used to get the allocator)
		//! @param _object The JSON object to add the contents to
		virtual void addToJsonObject(OT_rJSON_doc& _document, OT_rJSON_val& _object) const override;

		//! @brief Will set the object contents from the provided JSON object
		//! @param _object The JSON object containing the information
		//! @throw Will throw an exception if the provided object is not valid (members missing or invalid types)
		virtual void setFromJsonObject(OT_rJSON_val& _object) override;

		//! @brief Returns the block type as a string
		virtual std::string type(void) const { return OT_DEFAULTBLOCKCONFIGURATION_TYPE; };

		// ########################################################################################################################################################

		// Setter/Getter

		//! @brief Set the block title
		//! @param _title The title to set
		void setTitle(const std::string& _title) { m_title = _title; };

		//! @brif Return the block title
		const std::string& title(void) const { return m_title; };
		
		//! @brief Add the provided block layer
		//! @param _layer The layer to add
		void addLayer(BlockLayerConfiguration* _layer);

		//! @brief Add the provided block layers
		//! @param _layers The layers to add
		void addLayers(const std::vector<BlockLayerConfiguration*>& _layers);

		//! @brief Will replace the current layers with the provided ones
		void setLayers(const std::vector<BlockLayerConfiguration*>& _layers);

		//! @brief Returns the layers that were set for this block
		const std::vector<BlockLayerConfiguration*>& layers(void) const { return m_layers; };

	private:
		friend class BlockCategoryConfiguration;
		inline void setParentCategory(BlockCategoryConfiguration* _category) { m_parentCategory = _category; };
		inline BlockCategoryConfiguration* parentCategory(void) const { return m_parentCategory; };

		void clearLayers(void);

		BlockCategoryConfiguration*           m_parentCategory;
		std::vector<BlockLayerConfiguration*> m_layers;
		std::string                           m_title;

		BlockConfiguration(BlockConfiguration&) = delete;
		BlockConfiguration& operator = (BlockConfiguration&) = delete;
	};

}