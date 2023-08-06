//! @file GraphicsItemCfg.h
//! 
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTGui/OTGuiAPIExport.h"
#include "OTGui/Border.h"
#include "OTGui/Margins.h"
#include "OTGui/Font.h"
#include "OpenTwinCore/SimpleFactory.h"
#include "OpenTwinCore/Serializable.h"

// std header
#include <string>

#pragma warning(disable:4251)

#define OT_SimpleFactoryJsonKeyValue_GraphicsTextItemCfg "OT_GraphicsTextItemCfg"
#define OT_SimpleFactoryJsonKeyValue_GraphicsImageItemCfg "OT_GraphicsImageItemCfg"
#define OT_SimpleFactoryJsonKeyValue_GraphicsRectangularItemCfg "OT_GraphicsRectangularItemCfg"

namespace ot {

	class Painter2D;

	class OT_GUI_API_EXPORT GraphicsItemCfg : ot::Serializable, public ot::SimpleFactoryObject {
	public:
		GraphicsItemCfg();
		virtual ~GraphicsItemCfg();

		//! @brief Add the object contents to the provided JSON object
		//! @param _document The JSON document (used to get the allocator)
		//! @param _object The JSON object to add the contents to
		virtual void addToJsonObject(OT_rJSON_doc& _document, OT_rJSON_val& _object) const override;

		//! @brief Will set the object contents from the provided JSON object
		//! @param _object The JSON object containing the information
		//! @throw Will throw an exception if the provided object is not valid (members missing or invalid types)
		virtual void setFromJsonObject(OT_rJSON_val& _object) override;

		void setName(const std::string& _name) { m_name = _name; };
		const std::string& name(void) const { return m_name; };

		void setTitle(const std::string& _title) { m_tile = _title; };
		const std::string& title(void) const { return m_tile; };

		void setBorder(const Border& _border) { m_border = _border; };
		const Border& border(void) const { return m_border; };

		void setMargin(const Margins& _margin) { m_margins = _margin; };
		const Margins& margin(void) const { return m_margins; };

	private:
		std::string m_name;
		std::string m_tile;
		Border m_border;
		Margins m_margins;

	};

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	class OT_GUI_API_EXPORT GraphicsTextItemCfg : public ot::GraphicsItemCfg {
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

		void setTextColor(const ot::Color& _color) { m_textColor = _color; };
		const ot::Color& textColor(void) const { return m_textColor; };

	private:
		std::string m_text;
		ot::Font    m_textFont;
		ot::Color   m_textColor;

		GraphicsTextItemCfg(GraphicsTextItemCfg&) = delete;
		GraphicsTextItemCfg& operator = (GraphicsTextItemCfg&) = delete;
	};

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	class OT_GUI_API_EXPORT GraphicsImageItemCfg : public ot::GraphicsItemCfg {
	public:
		GraphicsImageItemCfg(const std::string& _imageSubPath = std::string());
		virtual ~GraphicsImageItemCfg();

		//! @brief Add the object contents to the provided JSON object
		//! @param _document The JSON document (used to get the allocator)
		//! @param _object The JSON object to add the contents to
		virtual void addToJsonObject(OT_rJSON_doc& _document, OT_rJSON_val& _object) const override;

		//! @brief Will set the object contents from the provided JSON object
		//! @param _object The JSON object containing the information
		//! @throw Will throw an exception if the provided object is not valid (members missing or invalid types)
		virtual void setFromJsonObject(OT_rJSON_val& _object) override;

		//! @brief Returns the key that is used to create an instance of this class in the simple factory
		virtual std::string simpleFactoryObjectKey(void) const override { return std::string(OT_SimpleFactoryJsonKeyValue_GraphicsImageItemCfg); };

		void setImagePath(const std::string& _path) { m_imageSubPath = _path; };
		const std::string& imagePath(void) const { return m_imageSubPath; };

	private:
		std::string m_imageSubPath;

		GraphicsImageItemCfg(GraphicsImageItemCfg&) = delete;
		GraphicsImageItemCfg& operator = (GraphicsImageItemCfg&) = delete;
	};

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	class OT_GUI_API_EXPORT GraphicsRectangularItemCfg : public ot::GraphicsItemCfg {
	public:
		GraphicsRectangularItemCfg(ot::Painter2D* _backgroundPainter = (ot::Painter2D*)nullptr, const ot::Color& _borderColor = ot::Color(), int _borderWidth = 2, int _cornerRadius = 0);
		virtual ~GraphicsRectangularItemCfg();

		//! @brief Add the object contents to the provided JSON object
		//! @param _document The JSON document (used to get the allocator)
		//! @param _object The JSON object to add the contents to
		virtual void addToJsonObject(OT_rJSON_doc& _document, OT_rJSON_val& _object) const override;

		//! @brief Will set the object contents from the provided JSON object
		//! @param _object The JSON object containing the information
		//! @throw Will throw an exception if the provided object is not valid (members missing or invalid types)
		virtual void setFromJsonObject(OT_rJSON_val& _object) override;

		//! @brief Returns the key that is used to create an instance of this class in the simple factory
		virtual std::string simpleFactoryObjectKey(void) const override { return std::string(OT_SimpleFactoryJsonKeyValue_GraphicsRectangularItemCfg); };

		void setCornerRadius(int _px) { m_cornerRadius = _px; };
		int cornerRadius(void) const { return m_cornerRadius; };

		void setBorderColor(const ot::Color& _color) { m_borderColor = _color; };
		const ot::Color& borderColor(void) const { return m_borderColor; };

		void setBorderWidth(int _px) { m_borderWidth = _px; };
		int borderWidth(void) const { return m_borderWidth; };

		void setBackgroundPainer(ot::Painter2D* _painter);
		ot::Painter2D* backgroundPainter(void) { return m_backgroundPainter; };

	private:
		int m_cornerRadius;
		ot::Color m_borderColor;
		int m_borderWidth;
		ot::Painter2D* m_backgroundPainter;

		GraphicsRectangularItemCfg(GraphicsRectangularItemCfg&) = delete;
		GraphicsRectangularItemCfg& operator = (GraphicsRectangularItemCfg&) = delete;
	};
}