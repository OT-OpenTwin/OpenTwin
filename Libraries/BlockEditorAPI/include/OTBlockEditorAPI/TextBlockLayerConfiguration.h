//! @file TextBlockCategoryConfiguration.h
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTBlockEditorAPI/BlockLayerConfiguration.h"
#include "OpenTwinCore/Color.h"

#define OT_TEXTBLOCKLAYERCONFIGURATION_TYPE "TextBlockLayer"

namespace ot {

	class BLOCKEDITORAPI_API_EXPORT TextBlockLayerConfiguration : public ot::BlockLayerConfiguration {
	public:
		TextBlockLayerConfiguration(const std::string& _text = std::string(), int _textSize = 12, const ot::Color& _textColor = ot::Color());
		virtual ~TextBlockLayerConfiguration();

		//! @brief Add the object contents to the provided JSON object
		//! @param _document The JSON document (used to get the allocator)
		//! @param _object The JSON object to add the contents to
		virtual void addToJsonObject(OT_rJSON_doc& _document, OT_rJSON_val& _object) const override;

		//! @brief Will set the object contents from the provided JSON object
		//! @param _object The JSON object containing the information
		//! @throw Will throw an exception if the provided object is not valid (members missing or invalid types)
		virtual void setFromJsonObject(OT_rJSON_val& _object) override;

		//! @brief Returns the layer configuration type
		virtual std::string layerType(void) const { return OT_TEXTBLOCKLAYERCONFIGURATION_TYPE; };

		void setText(const std::string& _text) { m_text = _text; };
		const std::string& text(void) const { return m_text; };

		void setTextSize(int _px) { m_textPixelSize = _px; };
		int textSize(void) const { return m_textPixelSize; };

		void setTextColor(const ot::Color& _color) { m_textColor = _color; };
		const ot::Color& textColor(void) const { return m_textColor; };

	private:
		std::string m_text;
		int         m_textPixelSize;
		ot::Color   m_textColor;

		TextBlockLayerConfiguration(TextBlockLayerConfiguration&) = delete;
		TextBlockLayerConfiguration& operator = (TextBlockLayerConfiguration&) = delete;
	};
}