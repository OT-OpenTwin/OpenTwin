#pragma once

// OpenTwin header
#include "OTBlockEditorAPI/BlockEditorAPIAPIExport.h"
#include "OTBlockEditorAPI/BlockConfiguration.h"

#define OT_CUSTOMBLOCKCONFIGURATION_TYPE "CustomBlock"

namespace ot {

	class BLOCKEDITORAPI_API_EXPORT CustomBlockConfiguration : public ot::BlockConfiguration {
	public:
		CustomBlockConfiguration();
		virtual ~CustomBlockConfiguration();

		//! @brief Add the object contents to the provided JSON object
		//! @param _document The JSON document (used to get the allocator)
		//! @param _object The JSON object to add the contents to
		virtual void addToJsonObject(OT_rJSON_doc& _document, OT_rJSON_val& _object) const override;

		//! @brief Will set the object contents from the provided JSON object
		//! @param _object The JSON object containing the information
		//! @throw Will throw an exception if the provided object is not valid (members missing or invalid types)
		virtual void setFromJsonObject(OT_rJSON_val& _object) override;

		//! @brief Returns the block type as a string
		virtual std::string type(void) const { return OT_CUSTOMBLOCKCONFIGURATION_TYPE; };

	private:


		CustomBlockConfiguration(CustomBlockConfiguration&) = delete;
		CustomBlockConfiguration& operator = (CustomBlockConfiguration&) = delete;
	};

}