// @otlicense
// File: GraphicsHierarchicalProjectItemBuilder.h
// 
// License:
// Copyright 2025 by OpenTwin
//  
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
//  
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// @otlicense-end

#pragma once

// OpenTwin header
#include "OTCore/Color.h"
#include "OTGui/Margins.h"
#include "OTGui/GraphicsItemCfg.h"

// std header
#include <vector>
#include <string>

namespace ot {

	class Painter2D;

	class OT_GUI_API_EXPORTONLY GraphicsHierarchicalProjectItemBuilder {
		OT_DECL_NOCOPY(GraphicsHierarchicalProjectItemBuilder)
		OT_DECL_NOMOVE(GraphicsHierarchicalProjectItemBuilder)
	public:
		enum BackgroundImageInsertMode {
			OnLayout,
			OnStack
		};

		//! @brief Creates a GraphicsItemCfg in the "OpenTwin hierarchical project item block" style that takes the current configuration into account.
		//! The callee takes ownership of the item.
		ot::GraphicsItemCfg* createGraphicsItem() const;

		GraphicsHierarchicalProjectItemBuilder();
		virtual ~GraphicsHierarchicalProjectItemBuilder();

		//! @brief Sets the name for the root item.
		//! The item name will be used as a prefix for the created child items (layouts and stacks).
		void setName(const std::string& _name) { m_name = _name; };
		const std::string& getName() const { return m_name; };

		//! @brief Sets the title that will be displayed to the user.
		void setTitle(const std::string& _title) { m_title = _title; };
		const std::string& getTitle() const { return m_title; };

		//! @brief Sets the project type that will be displayed to the user.
		//! @param _type The type string.
		void setProjectType(const std::string& _type) { m_type = _type; };
		const std::string& getProjectType() const { return m_type; };

		void setProjectVersion(const std::string& _version) { m_projectVersion = _version; };
		const std::string& getProjectVersion() const { return m_projectVersion; };

		//! @brief Set the item tool tip.
		void setToolTip(const std::string& _toolTip) { m_toolTip = _toolTip; };

		//! @brief Set the item tool tip.
		const std::string& getToolTip() const { return m_toolTip; };

		//! @brief Replace the current title background painter.
		//! The item takes ownership.
		void setTitleBackgroundPainter(ot::Painter2D* _painter);

		//! @brief Sets the title background color.
		//! Creates a FillPainter2D and replace the current title background painter.
		void setTitleBackgroundColor(const ot::Color& _color);

		//! @brief Sets the title background painter.
		//! Creates a LinearGradientPainter2D and replace the current title background painter.
		void setTitleBackgroundGradientColor(const ot::Color& _color);

		//! @brief Sets the title background color.
		//! Will create a FillPainter2D and replace the current title background painter.
		inline void setTitleBackgroundColor(int _r, int _g, int _b, int _a = 255) { this->setTitleBackgroundColor(ot::Color(_r, _g, _b, _a)); };

		//! @brief Replace the current title foreground painter.
		//! The item takes ownership.
		void setTitleForegroundPainter(ot::Painter2D* _painter);

		//! @brief Sets the title foreground painter.
		//! Create a FillPainter2D and replace the current title foreground painter.
		void setTitleForegroundColor(const ot::Color& _color);

		//! @brief Sets the title foreground painter.
		//! Create a LinearGradientPainter2D and replace the current title foreground painter.
		//! @param _color The primary text color, other colors are calculated by 255-color.
		void setDefaultTitleForegroundGradient();

		void setLeftTitleCornerImagePath(const std::string& _path) { m_leftTitleImagePath = _path; };

		//! @brief Sets the preview image path.
		//! The path is only used if no image data is set.
		//! @param _path The relative image path.
		void setPreviewImagePath(const std::string& _path) { m_previewImagePath = _path; };

		//! @brief Set the preview image data.
		//! The data is used instead of the image path if available.
		//! @param _data The image data.
		//! @param _format The image file format.
		void setPreviewImageData(const std::vector<char>& _data, ImageFileFormat _format) { m_previewImage = _data; m_previewImageFormat = _format; };
		void setPreviewImageMargins(const ot::MarginsD& _margins) { m_previewImageMargins = _margins; };
		
		void setConnectorSize(double _width, double _height) { m_connectorWidth = _width; m_connectorHeight = _height; };

		// ###########################################################################################################################################################################################################################################################################################################################

		// Private: Helper

	private:
		double m_connectorWidth;
		double m_connectorHeight;

		ot::GraphicsItemCfg* createConnectorItem(ot::Alignment _alignment) const;
		ot::GraphicsItemCfg* createTitle() const;

		std::string m_name;
		std::string m_type;
		std::string m_title;
		std::string m_toolTip;

		std::string m_projectVersion;

		ot::Painter2D* m_titleBackgroundPainter;
		ot::Painter2D* m_titleForegroundPainter;

		std::string m_leftTitleImagePath;

		std::string m_previewImagePath;
		std::vector<char> m_previewImage;
		ImageFileFormat m_previewImageFormat;
		ot::MarginsD m_previewImageMargins;
	};

}