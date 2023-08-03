//! @file ImageBlockLayerConfiguration.h
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTBlockEditorAPI/BlockLayerConfiguration.h"

#define OT_IMAGEBLOCKLAYERCONFIGURATION_TYPE "ImageBlockLayer"

namespace ot {

	class BLOCKEDITORAPI_API_EXPORT ImageBlockLayerConfiguration : public ot::BlockLayerConfiguration {
	public:
		ImageBlockLayerConfiguration(const std::string& _imageSubPath = std::string());
		virtual ~ImageBlockLayerConfiguration();

		//! @brief Add the object contents to the provided JSON object
		//! @param _document The JSON document (used to get the allocator)
		//! @param _object The JSON object to add the contents to
		virtual void addToJsonObject(OT_rJSON_doc& _document, OT_rJSON_val& _object) const override;

		//! @brief Will set the object contents from the provided JSON object
		//! @param _object The JSON object containing the information
		//! @throw Will throw an exception if the provided object is not valid (members missing or invalid types)
		virtual void setFromJsonObject(OT_rJSON_val& _object) override;

		//! @brief Returns the layer configuration type
		virtual std::string layerType(void) const { return OT_IMAGEBLOCKLAYERCONFIGURATION_TYPE; };

		void setImagePath(const std::string& _path) { m_imageSubPath = _path; };
		const std::string& imagePath(void) const { return m_imageSubPath; };

	private:
		std::string m_imageSubPath;

		ImageBlockLayerConfiguration(ImageBlockLayerConfiguration&) = delete;
		ImageBlockLayerConfiguration& operator = (ImageBlockLayerConfiguration&) = delete;
	};
}