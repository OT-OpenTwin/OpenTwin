//! @file GraphicsTextItemCfg.h
//! 
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/Color.h"
#include "OTGui/PenCfg.h"
#include "OTGui/GraphicsItemCfg.h"

namespace ot {

	class OT_GUI_API_EXPORT GraphicsTextItemCfg : public ot::GraphicsItemCfg {
		OT_DECL_NOMOVE(GraphicsTextItemCfg)
	public:
		static std::string className() { return "GraphicsTextItemCfg"; };

		GraphicsTextItemCfg(const std::string& _text = std::string(), const ot::Color& _textColor = ot::Color());
		GraphicsTextItemCfg(const GraphicsTextItemCfg& _other);
		virtual ~GraphicsTextItemCfg();

		GraphicsTextItemCfg& operator = (const GraphicsTextItemCfg&) = delete;

		//! @brief Creates a copy of this item.
		virtual GraphicsItemCfg* createCopy(void) const override { return new GraphicsTextItemCfg(*this); };

		//! @brief Add the object contents to the provided JSON object
		//! @param _document The JSON document (used to get the allocator)
		//! @param _object The JSON object to add the contents to
		virtual void addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const override;

		//! @brief Will set the object contents from the provided JSON object
		//! @param _object The JSON object containing the information
		//! @throw Will throw an exception if the provided object is not valid (members missing or invalid types)
		virtual void setFromJsonObject(const ConstJsonObject& _object) override;

		//! @brief Returns the key that is used to create an instance of this class in the simple factory
		virtual std::string getFactoryKey(void) const override { return GraphicsTextItemCfg::className(); };

		void setText(const std::string& _text) { m_text = _text; };
		const std::string& getText(void) const { return m_text; };

		void setTextFont(const Font& _font) { m_textFont = _font; };
		const Font& getTextFont(void) const { return m_textFont; };

		void setTextColor(const Color& _color) { m_textStyle.setColor(_color); };

		//! @brief Sets the text painter.
		//! The item takes ownership of the painter.
		void setTextPainter(Painter2D* _painter) { m_textStyle.setPainter(_painter); };
		const Painter2D* getTextPainter(void) const { return m_textStyle.getPainter(); };

		void setTextLineWidth(double _width) { m_textStyle.setWidth(_width); };

		void setTextStyle(const PenFCfg& _style) { m_textStyle = _style; };
		const PenFCfg& getTextStyle(void) const { return m_textStyle; };

		//! @brief Sets the text is reference mode.
		//! @see getTextIsReference
		void setTextIsReference(bool _isReference) { m_textIsReference = _isReference; };

		//! @brief If enabled the set text will be used as a key for the string map.
		//! @see GraphicsItemCfg::getStringMap
		bool getTextIsReference(void) const { return m_textIsReference; };

	private:
		std::string m_text;
		ot::Font    m_textFont;
		PenFCfg	m_textStyle;
		bool m_textIsReference;

		GraphicsTextItemCfg(GraphicsTextItemCfg&) = delete;
		GraphicsTextItemCfg& operator = (GraphicsTextItemCfg&) = delete;
	};

}
