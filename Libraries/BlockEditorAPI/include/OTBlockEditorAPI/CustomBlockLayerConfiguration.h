#pragma once

// OpenTwin header
#include "OpenTwinCore/Serializable.h"
#include "OTBlockEditorAPI/BlockEditorAPIAPIExport.h"

namespace ot {

	class BLOCKEDITORAPI_API_EXPORT CustomBlockLayerConfiguration : public ot::Serializable {
	public:
		CustomBlockLayerConfiguration();
		virtual ~CustomBlockLayerConfiguration();

		//! @brief Add the object contents to the provided JSON object
		//! @param _document The JSON document (used to get the allocator)
		//! @param _object The JSON object to add the contents to
		virtual void addToJsonObject(OT_rJSON_doc& _document, OT_rJSON_val& _object) const override;

		//! @brief Will set the object contents from the provided JSON object
		//! @param _object The JSON object containing the information
		//! @throw Will throw an exception if the provided object is not valid (members missing or invalid types)
		virtual void setFromJsonObject(OT_rJSON_val& _object) override;

	private:


		CustomBlockLayerConfiguration(CustomBlockLayerConfiguration&) = delete;
		CustomBlockLayerConfiguration& operator = (CustomBlockLayerConfiguration&) = delete;
	};

}