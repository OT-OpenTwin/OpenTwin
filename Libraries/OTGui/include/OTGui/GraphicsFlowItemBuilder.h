//! @file GraphicsFlowItemBuilder.h
//! 
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/OTClassHelper.h"
#include "OTGui/OTGuiAPIExport.h"
#include "OTGui/GraphicsItemCfg.h"
#include "OTGui/GraphicsTriangleItemCfg.h"
#include "OTGui/Margins.h"

namespace ot {

	class GraphicsGridLayoutItemCfg;

	class Painter2D;

	class OT_GUI_API_EXPORT GraphicsFlowItemConnector {
	public:
		enum ConnectorFigure {
			Square,
			TriangleRight,
			TriangleLeft,
			TriangleUp,
			TriangleDown,
			KiteRight,
			KiteLeft,
			KiteUp,
			KiteDown,
			IceConeRight,
			IceConeLeft,
			IceConeUp,
			IceConeDown,
			Circle
		};

		GraphicsFlowItemConnector();
		GraphicsFlowItemConnector(const GraphicsFlowItemConnector& _other);
		virtual ~GraphicsFlowItemConnector();
		GraphicsFlowItemConnector& operator = (const GraphicsFlowItemConnector& _other);

		void setName(const std::string& _name) { m_name = _name; };
		const std::string& name(void) const { return m_name; };

		void setText(const std::string& _text) { m_text = _text; };
		const std::string& text(void) const { return m_text; };

		void setToolTip(const std::string& _toolTip) { m_toolTip = _toolTip; };
		const std::string& toolTip(void) const { return m_toolTip; };

		void setFont(const ot::Font& _font) { m_font = _font; };
		const ot::Font& font(void) const { return m_font; };

		void setTextColor(const ot::Color& _color) { m_textColor = _color; };
		const ot::Color& textColor(void) const { return m_textColor; };

		void setPrimaryColor(const ot::Color& _color) { m_primaryColor = _color; };
		const ot::Color& primaryColor(void) const { return m_primaryColor; };

		void setSecondaryColor(const ot::Color& _color) { m_secondaryColor = _color; };
		const ot::Color& secondaryColor(void) const { return m_secondaryColor; };

		void setFigure(GraphicsFlowItemConnector::ConnectorFigure _figure) { m_figure = _figure; };
		GraphicsFlowItemConnector::ConnectorFigure figure(void) const { return m_figure; };

		void addToGrid(int _row, GraphicsGridLayoutItemCfg* _gridLayout, bool _isLeft);

	private:
		ot::GraphicsItemCfg* createConnectorItem(void);
		ot::GraphicsItemCfg* createSquareItem(void);
		ot::GraphicsItemCfg* createCircleItem(void);
		ot::GraphicsItemCfg* createTriangleItem(GraphicsTriangleItemCfg::TriangleDirection _direction, GraphicsTriangleItemCfg::TriangleShape _shape);

		std::string m_name;
		std::string m_text;
		std::string m_toolTip;
		ConnectorFigure m_figure;
		ot::Color m_textColor;
		ot::Font m_font;
		ot::Color m_primaryColor;
		ot::Color m_secondaryColor;
	};

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	class OT_GUI_API_EXPORTONLY GraphicsFlowItemBuilder {
		OT_DECL_NOCOPY(GraphicsFlowItemBuilder)
	public:
		enum BackgroundImageInsertMode {
			OnLayout,
			OnStack
		};

		//! @brief Creates a GraphicsItemCfg in the "OpenTwin flow block" style that takes the current configuration into account.
		//! The callee takes ownership of the item.
		ot::GraphicsItemCfg* createGraphicsItem(void) const;

		GraphicsFlowItemBuilder();
		virtual ~GraphicsFlowItemBuilder();

		//! @brief Sets the name for the root item
		//! The item name will be used as a prefix for the created child items (layouts and stacks)
		void setName(const std::string& _name) { m_name = _name; };

		//! @brief Item name
		const std::string& name(void) const { return m_name; };

		//! @brief Sets the title that will be displayed to the user
		void setTitle(const std::string& _title) { m_title = _title; };

		//! @brief Item title
		const std::string& title(void) const { return m_title; };

		//! @brief Set the item tool tip
		void setToolTip(const std::string& _toolTip) { m_toolTip = _toolTip; };

		//! @brief Set the item tool tip
		const std::string& toolTip(void) const { return m_toolTip; };

		//! @brief Add a connector on the left side of the FlowItem
		//! The default connector style will be applied
		//! @param _name Connector name
		//! @param _title Connector title
		//! @param _figure Conector figure
		void addLeft(const std::string& _name, const std::string& _title, GraphicsFlowItemConnector::ConnectorFigure _figure);

		//! @brief Add a connector on the left side of the FlowItem
		//! The default connector style will be applied
		//! @param _name Connector name
		//! @param _title Connector title
		//! @param _figure Conector figure
		void addLeft(const std::string& _name, const std::string& _title, GraphicsFlowItemConnector::ConnectorFigure _figure, ot::Color::DefaultColor _color);

		//! @brief Add a connector on the left side of the FlowItem
		//! The default connector style will be applied
		//! @param _name Connector name
		//! @param _title Connector title
		//! @param _figure Conector figure
		void addLeft(const std::string& _name, const std::string& _title, GraphicsFlowItemConnector::ConnectorFigure _figure, const ot::Color& _color);

		//! @brief Add the provided connector as input
		//! @param _input Connector configuration
		void addLeft(const GraphicsFlowItemConnector& _left);

		//! @brief Add a connector on the right side of the FlowItem
		//! The default connector style will be applied
		//! @param _name Connector name
		//! @param _title Connector title
		//! @param _figure Connector figure
		void addRight(const std::string& _name, const std::string& _title, GraphicsFlowItemConnector::ConnectorFigure _figure);

		//! @brief Add a connector on the right side of the FlowItem
		//! The default connector style will be applied
		//! @param _name Connector name
		//! @param _title Connector title
		//! @param _figure Connector figure
		void addRight(const std::string& _name, const std::string& _title, GraphicsFlowItemConnector::ConnectorFigure _figure, ot::Color::DefaultColor _color);

		//! @brief Add a connector on the right side of the FlowItem
		//! The default connector style will be applied
		//! @param _name Connector name
		//! @param _title Connector title
		//! @param _figure Connector figure
		void addRight(const std::string& _name, const std::string& _title, GraphicsFlowItemConnector::ConnectorFigure _figure, const ot::Color& _color);

		//! @brief Add the provided connector as output
		//! @param _input Connector configuration
		void addRight(const GraphicsFlowItemConnector& _right);

		//! @brief Replace the current background painter
		//! The item takes ownership
		void setBackgroundPainter(ot::Painter2D* _painter);

		//! @brief Sets the background color
		//! Will create a FillPainter2D and replace the current background painter
		void setBackgroundColor(const ot::Color& _color);

		//! @brief Sets the background color
		//! Will create a FillPainter2D and replace the current background painter
		void setBackgroundColor(int _r, int _g, int _b, int _a = 255) { this->setBackgroundColor(ot::Color(_r, _g, _b, _a)); };

		//! @brief Replace the current title background painter
		//! The item takes ownership
		void setTitleBackgroundPainter(ot::Painter2D* _painter);

		//! @brief Sets the title background color
		//! Creates a FillPainter2D and replace the current title background painter
		void setTitleBackgroundColor(const ot::Color& _color);

		//! @brief Sets the title background painter
		//! Creates a LinearGradientPainter2D and replace the current title background painter
		void setTitleBackgroundGradientColor(const ot::Color& _color);

		//! @brief Sets the title background color
		//! Will create a FillPainter2D and replace the current title background painter
		inline void setTitleBackgroundColor(int _r, int _g, int _b, int _a = 255) { this->setTitleBackgroundColor(ot::Color(_r, _g, _b, _a)); };

		//! @brief Replace the current title foreground painter
		//! The item takes ownership
		void setTitleForegroundPainter(ot::Painter2D* _painter);

		//! @brief Sets the title foreground painter
		//! Create a FillPainter2D and replace the current title foreground painter
		void setTitleForegroundColor(const ot::Color& _color);

		//! @brief Sets the title foreground painter
		//! Create a LinearGradientPainter2D and replace the current title foreground painter
		//! @param _color The primary text color, other colors are calculated by 255-color
		void setDefaultTitleForegroundGradient(void);

		//! @brief Sets the default connector style
		//! The new default style will only affect items added after settings the style.
		//! The defualt style is applied to connetors added via addInput() or addOutput() (with exceptions, see function comments)
		void setDefaultConnectorStyle(const GraphicsFlowItemConnector& _config) { m_defaultConnectorStyle = _config; };
		const GraphicsFlowItemConnector& defaultConnectorStyle(void) const { return m_defaultConnectorStyle; };

		void setBackgroundImagePath(const std::string& _path) { m_backgroundImagePath = _path; };
		void setBackgroundImageAlignment(ot::Alignment _align) { m_backgroundImageAlignment = _align; };
		void setBackgroundImageMargins(const ot::MarginsD& _margins) { m_backgroundImageMargins = _margins; };
		void setBackgroundImageInsertMode(BackgroundImageInsertMode _mode) { m_backgroundImageInsertMode = _mode; };
		void setBackgroundImageMaintainAspectRatio(bool _active) { m_backgroundImageMaintainAspectRatio = _active; };

		void setLeftTitleCornerImagePath(const std::string& _path) { m_leftTitleImagePath = _path; };

		void setRightTitleCornerImagePath(const std::string& _path) { m_rightTitleImagePath = _path; };

	private:
		std::string m_name;
		std::string m_title;
		std::string m_toolTip;

		ot::Painter2D* m_backgroundPainter;
		ot::Painter2D* m_titleBackgroundPainter;
		ot::Painter2D* m_titleForegroundPainter;

		std::string m_leftTitleImagePath;
		std::string m_rightTitleImagePath;

		std::string m_backgroundImagePath;
		ot::Alignment m_backgroundImageAlignment;
		ot::MarginsD m_backgroundImageMargins;
		BackgroundImageInsertMode m_backgroundImageInsertMode;
		bool m_backgroundImageMaintainAspectRatio;

		GraphicsFlowItemConnector m_defaultConnectorStyle;

		std::list<GraphicsFlowItemConnector> m_left;
		std::list<GraphicsFlowItemConnector> m_right;
	};

}