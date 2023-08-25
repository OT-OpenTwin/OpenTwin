//! @file GraphicsFlowConnectorCfg.h
//! 
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTGui/OTGuiAPIExport.h"
#include "OTGui/GraphicsItemCfg.h"

namespace ot {

	class GraphicsGridLayoutItemCfg;

	class OT_GUI_API_EXPORTONLY GraphicsFlowConnectorCfg {
	public:
		enum ConnectorFigure {
			Square,
			//TriangleRight,
			//TriangleLeft,
			Circle
		};

		GraphicsFlowConnectorCfg();
		GraphicsFlowConnectorCfg(const GraphicsFlowConnectorCfg& _other);
		virtual ~GraphicsFlowConnectorCfg();
		GraphicsFlowConnectorCfg& operator = (const GraphicsFlowConnectorCfg& _other);

		void setName(const std::string& _name) { m_name = _name; };
		const std::string& name(void) const { return m_name; };

		void setText(const std::string& _text) { m_text = _text; };
		const std::string& text(void) const { return m_text; };

		void setFont(const ot::Font& _font) { m_font = _font; };
		const ot::Font& font(void) const { return m_font; };

		void setTextColor(const ot::Color& _color) { m_textColor = _color; };
		const ot::Color& textColor(void) const { return m_textColor; };

		void setPrimaryColor(const ot::Color& _color) { m_primaryColor = _color; };
		const ot::Color& primaryColor(void) const { return m_primaryColor; };

		void setSecondaryColor(const ot::Color& _color) { m_secondaryColor = _color; };
		const ot::Color& secondaryColor(void) const { return m_secondaryColor; };

		void setFigure(GraphicsFlowConnectorCfg::ConnectorFigure _figure) { m_figure = _figure; };
		GraphicsFlowConnectorCfg::ConnectorFigure figure(void) const { return m_figure; };

		//! @brief Creates a graphics item corresponding to the 
		void addToGrid(int _row, int _connectorColumn, int _titleColumn, GraphicsGridLayoutItemCfg* _gridLayout);

	private:
		ot::GraphicsItemCfg* createConnectorItem(void);
		ot::GraphicsItemCfg* createSquareItem(void);
		ot::GraphicsItemCfg* createCircleItem(void);

		std::string m_name;
		std::string m_text;
		ConnectorFigure m_figure;
		ot::Color m_textColor;
		ot::Font m_font;
		ot::Color m_primaryColor;
		ot::Color m_secondaryColor;
	};

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	class OT_GUI_API_EXPORTONLY GraphicsFlowItemCfg {
	public:
		//! @brief Creates a GraphicsItem configuration in the OpenTwin flow block style
		//! The callee takes ownership of the item
		ot::GraphicsItemCfg* createGraphicsItem(const std::string& _name, const std::string& _title) const;

		GraphicsFlowItemCfg();
		virtual ~GraphicsFlowItemCfg();

		//! @brief Add a connector on the left side of the FlowItem
		//! The default connector style will be applied
		//! @param _name Connector name
		//! @param _title Connector title
		//! @param _figure Conector figure
		void addInput(const std::string& _name, const std::string& _title, GraphicsFlowConnectorCfg::ConnectorFigure _figure);

		//! @brief Add a connector on the left side of the FlowItem
		//! The default connector style will be applied
		//! @param _name Connector name
		//! @param _title Connector title
		//! @param _figure Conector figure
		void addInput(const std::string& _name, const std::string& _title, GraphicsFlowConnectorCfg::ConnectorFigure _figure, ot::Color::DefaultColor _color);

		//! @brief Add a connector on the left side of the FlowItem
		//! The default connector style will be applied
		//! @param _name Connector name
		//! @param _title Connector title
		//! @param _figure Conector figure
		void addInput(const std::string& _name, const std::string& _title, GraphicsFlowConnectorCfg::ConnectorFigure _figure, const ot::Color& _color);

		//! @brief Add the provided connector as input
		//! @param _input Connector configuration
		void addInput(const GraphicsFlowConnectorCfg& _input);

		//! @brief Add a connector on the right side of the FlowItem
		//! The default connector style will be applied
		//! @param _name Connector name
		//! @param _title Connector title
		//! @param _figure Connector figure
		void addOutput(const std::string& _name, const std::string& _title, GraphicsFlowConnectorCfg::ConnectorFigure _figure);

		//! @brief Add a connector on the right side of the FlowItem
		//! The default connector style will be applied
		//! @param _name Connector name
		//! @param _title Connector title
		//! @param _figure Connector figure
		void addOutput(const std::string& _name, const std::string& _title, GraphicsFlowConnectorCfg::ConnectorFigure _figure, ot::Color::DefaultColor _color);

		//! @brief Add a connector on the right side of the FlowItem
		//! The default connector style will be applied
		//! @param _name Connector name
		//! @param _title Connector title
		//! @param _figure Connector figure
		void addOutput(const std::string& _name, const std::string& _title, GraphicsFlowConnectorCfg::ConnectorFigure _figure, const ot::Color& _color);

		//! @brief Add the provided connector as output
		//! @param _input Connector configuration
		void addOutput(const GraphicsFlowConnectorCfg& _output);

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

		//! @brief Sets the default connector style
		//! The new default style will only affect items added after settings the style.
		//! The defualt style is applied to connetors added via addInput() or addOutput() (with exceptions, see function comments)
		void setDefaultConnectorStyle(const GraphicsFlowConnectorCfg& _config) { m_defaultConnectorStyle = _config; };
		const GraphicsFlowConnectorCfg& defaultConnectorStyle(void) const { return m_defaultConnectorStyle; };

		void setBackgroundImagePath(const std::string& _path) { m_backgroundImagePath = _path; };

	private:
		ot::Painter2D* m_backgroundPainter;
		ot::Painter2D* m_titleBackgroundPainter;

		std::string m_backgroundImagePath;

		GraphicsFlowConnectorCfg m_defaultConnectorStyle;

		std::list<GraphicsFlowConnectorCfg> m_inputs;
		std::list<GraphicsFlowConnectorCfg> m_outputs;

		GraphicsFlowItemCfg(GraphicsFlowItemCfg&) = delete;
		GraphicsFlowItemCfg& operator = (GraphicsFlowItemCfg&) = delete;
	};

}