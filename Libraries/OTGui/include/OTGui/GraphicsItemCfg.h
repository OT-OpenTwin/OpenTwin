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
#include "OpenTwinCore/Size2D.h"
#include "OpenTwinCore/SimpleFactory.h"
#include "OpenTwinCore/Serializable.h"

// std header
#include <string>

#define OT_SimpleFactoryJsonKeyValue_GraphicsTextItemCfg "OT_GICText"
#define OT_SimpleFactoryJsonKeyValue_GraphicsStackItemCfg "OT_GICStack"
#define OT_SimpleFactoryJsonKeyValue_GraphicsImageItemCfg "OT_GICImage"
#define OT_SimpleFactoryJsonKeyValue_GraphicsEllipseItemCfg "OT_GICElli"
#define OT_SimpleFactoryJsonKeyValue_GraphicsRectangularItemCfg "OT_GICRect"

#pragma warning(disable:4251)

namespace ot {

	class Painter2D;

	class OT_GUI_API_EXPORTONLY GraphicsItemCfg : public ot::Serializable, public ot::SimpleFactoryObject {
	public:
		enum GraphicsItemFlag {
			NoFlags           = 0x00, //! @brief No item flags
			ItemIsConnectable = 0x01  //! @brief Item can be used as source or destination of a conncetion
		};

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

		void setSize(const ot::Size2D& _size) { m_size = _size; };
		const ot::Size2D& size(void) const { return m_size; };

		void setMargins(const Margins& _margin) { m_margins = _margin; };
		const Margins& margins(void) const { return m_margins; };

		void setGraphicsItemFlags(GraphicsItemFlag _flags) { m_flags = _flags; };
		GraphicsItemFlag graphicsItemFlags(void) const { return m_flags; };

		void setAlignment(ot::Alignment _align) { m_alignment = _align; };
		ot::Alignment alignment(void) const { return m_alignment; };

	private:
		std::string m_name;
		std::string m_tile;
		Size2D m_size;
		Margins m_margins;
		GraphicsItemFlag m_flags;
		ot::Alignment m_alignment;
	};

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	class OT_GUI_API_EXPORTONLY GraphicsStackItemCfg : public ot::GraphicsItemCfg {
	public:
		struct GraphicsStackItemCfgEntry {
			ot::GraphicsItemCfg* item;
			bool isMaster;
		};

		GraphicsStackItemCfg();
		virtual ~GraphicsStackItemCfg();

		//! @brief Add the object contents to the provided JSON object
		//! @param _document The JSON document (used to get the allocator)
		//! @param _object The JSON object to add the contents to
		virtual void addToJsonObject(OT_rJSON_doc& _document, OT_rJSON_val& _object) const override;

		//! @brief Will set the object contents from the provided JSON object
		//! @param _object The JSON object containing the information
		//! @throw Will throw an exception if the provided object is not valid (members missing or invalid types)
		virtual void setFromJsonObject(OT_rJSON_val& _object) override;

		//! @brief Returns the key that is used to create an instance of this class in the simple factory
		virtual std::string simpleFactoryObjectKey(void) const override { return std::string(OT_SimpleFactoryJsonKeyValue_GraphicsStackItemCfg); };

		void addItemTop(ot::GraphicsItemCfg* _item, bool _isMaster);
		void addItemBottom(ot::GraphicsItemCfg* _item, bool _isMaster);

		//! @brief Returns a list with all items in the stack
		//! 0: bottom, n: top
		const std::list<GraphicsStackItemCfgEntry>& items(void) const { return m_items; };

	private:
		void memClear(void);

		std::list<GraphicsStackItemCfgEntry> m_items;

	};

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

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

	class OT_GUI_API_EXPORTONLY GraphicsImageItemCfg : public ot::GraphicsItemCfg {
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

	class OT_GUI_API_EXPORTONLY GraphicsRectangularItemCfg : public ot::GraphicsItemCfg {
	public:
		GraphicsRectangularItemCfg(ot::Painter2D* _backgroundPainter = (ot::Painter2D*)nullptr, int _cornerRadius = 0);
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
		
		void setBorder(const ot::Border& _border) { m_border = _border; };
		const ot::Border& border(void) const { return m_border; };

		void setBackgroundPainer(ot::Painter2D* _painter);
		ot::Painter2D* backgroundPainter(void) { return m_backgroundPainter; };

	private:
		int m_cornerRadius;
		ot::Border m_border;
		ot::Painter2D* m_backgroundPainter;

		GraphicsRectangularItemCfg(GraphicsRectangularItemCfg&) = delete;
		GraphicsRectangularItemCfg& operator = (GraphicsRectangularItemCfg&) = delete;
	};

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	class OT_GUI_API_EXPORTONLY GraphicsEllipseItemCfg : public ot::GraphicsItemCfg {
	public:
		GraphicsEllipseItemCfg(int _radiusX = 5, int _radiusY = 5, ot::Painter2D* _backgroundPainter = (ot::Painter2D*)nullptr);
		virtual ~GraphicsEllipseItemCfg();

		//! @brief Add the object contents to the provided JSON object
		//! @param _document The JSON document (used to get the allocator)
		//! @param _object The JSON object to add the contents to
		virtual void addToJsonObject(OT_rJSON_doc& _document, OT_rJSON_val& _object) const override;

		//! @brief Will set the object contents from the provided JSON object
		//! @param _object The JSON object containing the information
		//! @throw Will throw an exception if the provided object is not valid (members missing or invalid types)
		virtual void setFromJsonObject(OT_rJSON_val& _object) override;

		//! @brief Returns the key that is used to create an instance of this class in the simple factory
		virtual std::string simpleFactoryObjectKey(void) const override { return std::string(OT_SimpleFactoryJsonKeyValue_GraphicsEllipseItemCfg); };

		void setRadiusX(int _radius) { m_radiusX = _radius; };
		int radiusX(void) const { return m_radiusX; };
		void setRadiusY(int _radius) { m_radiusY = _radius; };
		int radiusY(void) const { return m_radiusY; };

		void setBorder(const ot::Border& _border) { m_border = _border; };
		const ot::Border& border(void) const { return m_border; };

		void setBackgroundPainer(ot::Painter2D* _painter);
		ot::Painter2D* backgroundPainter(void) { return m_backgroundPainter; };

	private:
		int m_radiusX;
		int m_radiusY;
		ot::Border m_border;
		ot::Painter2D* m_backgroundPainter;

		GraphicsEllipseItemCfg(GraphicsEllipseItemCfg&) = delete;
		GraphicsEllipseItemCfg& operator = (GraphicsEllipseItemCfg&) = delete;
	};

}

OT_ADD_FLAG_FUNCTIONS(ot::GraphicsItemCfg::GraphicsItemFlag);