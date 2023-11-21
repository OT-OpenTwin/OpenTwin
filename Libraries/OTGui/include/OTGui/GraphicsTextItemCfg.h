//! @file GraphicsTextItemCfg.h
//! 
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTGui/GraphicsItemCfg.h"

#define OT_SimpleFactoryJsonKeyValue_GraphicsTextItemCfg "OT_GICText"

namespace ot {

	class Painter2D;

	class OT_GUI_API_EXPORTONLY GraphicsTextItemCfg : public ot::GraphicsItemCfg {
	public:
		GraphicsTextItemCfg(const std::string& _text = std::string(), const ot::Color& _textColor = ot::Color());
		virtual ~GraphicsTextItemCfg();

		//! @brief Add the object contents to the provided JSON object
		//! @param _document The JSON document (used to get the allocator)
		//! @param _object The JSON object to add the contents to
		virtual void addToJsonObject(OT_rJSON_doc& _document, OT_rJSON_val& _object) const override;

		//! @brief Will set the object contents from the provided JSON object
		//! @param _object The JSON object containing the information
		//! @throw Will throw an exception if the provided object is not valid (members missing or invalid types)
		virtual void setFromJsonObject(OT_rJSON_val& _object) override;

		//! @brief Returns the key that is used to create an instance of this class in the simple factory
		virtual std::string simpleFactoryObjectKey(void) const override { return std::string(OT_SimpleFactoryJsonKeyValue_GraphicsTextItemCfg); };

		void setText(const std::string& _text) { m_text = _text; };
		const std::string& text(void) const { return m_text; };

		void setTextFont(const ot::Font& _font) { m_textFont = _font; };
		const ot::Font& textFont(void) const { return m_textFont; };

		void setTextColor(const ot::Color& _color);
		void setTextPainter(ot::Painter2D* _painter);
		Painter2D* textPainter(void) const { return m_textPainter; };

	private:
		std::string m_text;
		ot::Font    m_textFont;
		Painter2D* m_textPainter;

		GraphicsTextItemCfg(GraphicsTextItemCfg&) = delete;
		GraphicsTextItemCfg& operator = (GraphicsTextItemCfg&) = delete;
	};

}
