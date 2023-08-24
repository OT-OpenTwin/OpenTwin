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
#define OT_SimpleFactoryJsonKeyValue_GraphicsFlowItemCfg "OT_GICFlow"
#define OT_SimpleFactoryJsonKeyValue_GraphicsStackItemCfg "OT_GICStack"
#define OT_SimpleFactoryJsonKeyValue_GraphicsImageItemCfg "OT_GICImage"
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

	private:
		std::string m_name;
		std::string m_tile;
		Size2D m_size;
		Margins m_margins;
		GraphicsItemFlag m_flags;
	};

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	class OT_GUI_API_EXPORTONLY GraphicsStackItemCfg : public ot::GraphicsItemCfg {
	public:
		GraphicsStackItemCfg();
		GraphicsStackItemCfg(ot::GraphicsItemCfg* _bottomItem, ot::GraphicsItemCfg* _topItem);
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

		//! @brief Set the bottom item
		//! The stack item takes ownership of the item
		//! @param _bottom Child item
		void setBottomItem(ot::GraphicsItemCfg* _bottom);
		ot::GraphicsItemCfg* bottomItem(void) { return m_bottom; };

		//! @brief Set the top item
		//! The stack item takes ownership of the item
		//! @param _top Child item
		void setTopItem(ot::GraphicsItemCfg* _top);
		ot::GraphicsItemCfg* topItem(void) { return m_top; };

	private:
		ot::GraphicsItemCfg* m_bottom;
		ot::GraphicsItemCfg* m_top;

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

	class OT_GUI_API_EXPORTONLY GraphicsFlowItemCfg : public ot::GraphicsItemCfg {
	public:
		enum FlowConnectorType {
			Square
		};

		GraphicsFlowItemCfg();
		virtual ~GraphicsFlowItemCfg();

		//! @brief Add the object contents to the provided JSON object
		//! @param _document The JSON document (used to get the allocator)
		//! @param _object The JSON object to add the contents to
		virtual void addToJsonObject(OT_rJSON_doc& _document, OT_rJSON_val& _object) const override;

		//! @brief Will set the object contents from the provided JSON object
		//! @param _object The JSON object containing the information
		//! @throw Will throw an exception if the provided object is not valid (members missing or invalid types)
		virtual void setFromJsonObject(OT_rJSON_val& _object) override;

		//! @brief Returns the key that is used to create an instance of this class in the simple factory
		virtual std::string simpleFactoryObjectKey(void) const override { return std::string(OT_SimpleFactoryJsonKeyValue_GraphicsFlowItemCfg); };

		//! @brief Add a connector on the left side of the FlowItem
		//! @param _name Connector name
		//! @param _title Connector title
		//! @param _type Connector type
		void addInput(const std::string& _name, const std::string& _title, FlowConnectorType _type);

		//! @brief Add a connector on the right side of the FlowItem
		//! @param _name Connector name
		//! @param _title Connector title
		//! @param _type Connector type
		void addOutput(const std::string& _name, const std::string& _title, FlowConnectorType _type);

		//! @brief Replace the current background painter
		//! The item takes ownership
		void setBackgroundPainter(ot::Painter2D* _painter);

		//! @brief Sets the background color
		//! Will create a FillPainter2D and replace the current background painter
		void setBackgroundColor(const ot::Color& _color);

		//! @brief Sets the background color
		//! Will create a FillPainter2D and replace the current background painter
		void setBackgroundColor(int _r, int _g, int _b, int _a = 255) { this->setBackgroundColor(ot::Color(_r, _g, _b, _a)); };

		//! @brief Replace the current background painter
		//! The item takes ownership
		void setTitleBackgroundPainter(ot::Painter2D* _painter);

		//! @brief Sets the title background color
		//! Will create a FillPainter2D and replace the current title background painter
		void setTitleBackgroundColor(const ot::Color& _color);

		//! @brief Sets the title background color
		//! Will create a FillPainter2D and replace the current title background painter
		inline void setTitleBackgroundColor(int _r, int _g, int _b, int _a = 255) { this->setTitleBackgroundColor(ot::Color(_r, _g, _b, _a)); };

		void setConnectorTextColor(const ot::Color& _color) { m_connectorTextColor = _color; };
		inline void setConnectorTextColor(int _r, int _g, int _b, int _a = 255) { m_connectorTextColor = ot::Color(_r, _g, _b, _a); };

		void setConnectorTextFont(const ot::Font& _font) { m_connectorTextFont = _font; };

		void setBackgroundImagePath(const std::string& _path) { m_backgroundImagePath = _path; };

	private:
		struct FlowConnectorEntry {
			std::string name;
			std::string title;
			FlowConnectorType type;
		};

		ot::GraphicsItemCfg* createConnectorEntry(const FlowConnectorEntry& _entry, ot::GraphicsItemCfg* _connectorItem, bool _isInput) const;
		ot::GraphicsItemCfg* createSquareConnector(const FlowConnectorEntry& _entry, bool _isInput) const;
		ot::GraphicsItemCfg* createConnector(const FlowConnectorEntry& _entry, bool _isInput) const;

		ot::Painter2D* m_backgroundPainter;
		ot::Painter2D* m_titleBackgroundPainter;

		ot::Color m_connectorTextColor;
		ot::Font m_connectorTextFont;

		std::string m_backgroundImagePath;

		std::list<FlowConnectorEntry> m_inputs;
		std::list<FlowConnectorEntry> m_outputs;

		GraphicsFlowItemCfg(GraphicsFlowItemCfg&) = delete;
		GraphicsFlowItemCfg& operator = (GraphicsFlowItemCfg&) = delete;
	};

}

OT_ADD_FLAG_FUNCTIONS(ot::GraphicsItemCfg::GraphicsItemFlag);