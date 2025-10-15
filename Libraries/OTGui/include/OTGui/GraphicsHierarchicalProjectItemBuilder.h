//! @file GraphicsHierarchicalProjectItemBuilder.h
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/Color.h"
#include "OTGui/Margins.h"
#include "OTGui/GraphicsItemCfg.h"

namespace ot {

	class Painter2D;

	class OT_GUI_API_EXPORTONLY GraphicsHierarchicalProjectItemBuilder {
		OT_DECL_NOCOPY(GraphicsHierarchicalProjectItemBuilder)
	public:
		enum BackgroundImageInsertMode {
			OnLayout,
			OnStack
		};

		//! @brief Creates a GraphicsItemCfg in the "OpenTwin hierarchical project item block" style that takes the current configuration into account.
		//! The callee takes ownership of the item.
		ot::GraphicsItemCfg* createGraphicsItem(void) const;

		GraphicsHierarchicalProjectItemBuilder();
		virtual ~GraphicsHierarchicalProjectItemBuilder();

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

		void setBackgroundImagePath(const std::string& _path) { m_backgroundImagePath = _path; };
		void setBackgroundImageMargins(const ot::MarginsD& _margins) { m_backgroundImageMargins = _margins; };
		void setLeftTitleCornerImagePath(const std::string& _path) { m_leftTitleImagePath = _path; };

	private:
		std::string m_name;
		std::string m_title;
		std::string m_toolTip;

		ot::Painter2D* m_titleBackgroundPainter;
		ot::Painter2D* m_titleForegroundPainter;

		std::string m_leftTitleImagePath;

		std::string m_backgroundImagePath;
		ot::MarginsD m_backgroundImageMargins;
	};

}