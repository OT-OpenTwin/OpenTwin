// @otlicense
// File: GraphicsHierarchicalItemBuilder.h
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
#include "OTGui/Style/Font.h"
#include "OTGui/Style/PenCfg.h"
#include "OTGui/Style/Margins.h"
#include "OTGui/Painter/Painter2DContainer.h"
#include "OTGui/Graphics/GraphicsItemCfg.h"

// std header
#include <vector>
#include <string>

namespace ot {

	class Painter2D;
	class GraphicsLayoutItemCfg;

	class OT_GUI_API_EXPORTONLY GraphicsHierarchicalItemBuilder {
		OT_DECL_NOCOPY(GraphicsHierarchicalItemBuilder)
		OT_DECL_NOMOVE(GraphicsHierarchicalItemBuilder)
	public:
		//! @brief Creates a GraphicsItemCfg in the "OpenTwin hierarchical project item block" style that takes the current configuration into account.
		//! The callee takes ownership of the item.
		ot::GraphicsItemCfg* createGraphicsItem() const;

		GraphicsHierarchicalItemBuilder();
		virtual ~GraphicsHierarchicalItemBuilder();

		//! @brief Set the entity name of the item.
		void setEntityName(const std::string& _name) { m_entityName = _name; };

		void setTopText(const std::string& _text) { m_topText.text = _text; };
		void setTopTextFont(const Font& _font) { m_topText.font = _font; };
		void setTopTextMargins(const MarginsD& _margins) { m_topText.margins = _margins; };
		void setTopTextPen(const PenFCfg& _pen) { m_topText.pen = _pen; };
		//! @brief Replace the current top text pen painter.
		//! @param _painter The pen takes ownership of the painter.
		void setTopTextPainter(Painter2D* _painter) { m_topText.pen.setPainter(_painter); };
		void setTopTextColor(const Color& _color) { m_topText.pen.setColor(_color); };
		void setTopTextAlignment(Alignment _alignment) { m_topText.alignment = _alignment; };

		//! @brief Sets the image path.
		//! The path is only used if no image data is set.
		//! @param _path The relative image path.
		void setCenterImagePath(const std::string& _path) { m_centerImage.path = _path; };

		//! @brief Set the image data.
		//! The data is used instead of the image path if available.
		//! @param _data The image data.
		//! @param _format The image file format.
		void setCenterImageData(const std::vector<char>& _data, ImageFileFormat _format) { m_centerImage.data = _data; m_centerImage.format = _format; };
		void setCenterImageMargins(const ot::MarginsD& _margins) { m_centerImage.margins = _margins; };
		void setCenterImageAlignment(Alignment _alignment) { m_centerImage.alignment = _alignment; };
		void setCenterImageMaintainAspectRatio(bool _maintain) { m_centerImage.maintainAspectRatio = _maintain; };
		void setCenterImageMinimumSize(const Size2DD& _size) { m_centerImage.minimumSize = _size; };
		void setCenterImageMaximumSize(const Size2DD& _size) { m_centerImage.maximumSize = _size; };
		void setCenterImageFixedSize(const Size2DD& _size) { m_centerImage.minimumSize = _size; m_centerImage.maximumSize = _size; };

		void setBottomText(const std::string& _text) { m_bottomText.text = _text; };
		void setBottomTextFont(const Font& _font) { m_bottomText.font = _font; };
		void setBottomTextMargins(const MarginsD& _margins) { m_bottomText.margins = _margins; };
		void setBottomTextPen(const PenFCfg& _pen) { m_bottomText.pen = _pen; };
		//! @brief Replace the current bottom text pen painter.
		//! @param _painter The item takes ownership of the painter.
		void setBottomTextPainter(Painter2D* _painter) { m_bottomText.pen.setPainter(_painter); };
		void setBottomTextColor(const Color& _color) { m_bottomText.pen.setColor(_color); };
		void setBottomTextAlignment(Alignment _alignment) { m_bottomText.alignment = _alignment; };

		//! @brief Set the item tool tip.
		void setToolTip(const std::string& _toolTip) { m_toolTip = _toolTip; };

		//! @brief Replace the current background painter.
		//! @param _painter The item takes ownership of the painter.
		void setBackgroundPainter(Painter2D* _painter) { m_backgroundPainter.setPainter(_painter); };
		void setBackgroundColor(const Color& _color);
		inline void setBackgroundColor(int _r, int _g, int _b, int _a = 255) { this->setBackgroundColor(Color(_r, _g, _b, _a)); };

		void setOutline(const PenFCfg& _outline) { m_outline = _outline; };

		//! @brief Replace the current outline painter.
		//! @param _painter The item takes ownership of the painter.
		void setOutlinePainter(Painter2D* _painter) { m_outline.setPainter(_painter); };
		void setOutlineColor(const Color& _color) { m_outline.setColor(_color); };
		inline void setOutlineColor(int _r, int _g, int _b, int _a = 255) { m_outline.setColor(Color(_r, _g, _b, _a)); };

		void setOutlineWidth(double _width) { m_outline.setWidth(_width); };

		void setConnectorWidth(double _width) { m_connectorWidth = _width; };
		void setConnectorHeight(double _height) { m_connectorHeight = _height; };
		void setConnectorSize(double _width, double _height) { m_connectorWidth = _width; m_connectorHeight = _height; };

		// ###########################################################################################################################################################################################################################################################################################################################

		// Private: Helper

	private:
		struct TextInfo {
			std::string text;
			Font font;
			MarginsD margins;
			PenFCfg pen;
			Alignment alignment;
		};

		struct ImageInfo
		{
			std::string path;
			std::vector<char> data;
			ImageFileFormat format;
			MarginsD margins;
			Alignment alignment;
			bool maintainAspectRatio;
			Size2DD minimumSize;
			Size2DD maximumSize;
		};

		void initializeTextInfo(TextInfo& _info);
		void initializeImageInfo(ImageInfo& _info);

		GraphicsItemCfg* createConnectorItem(ot::Alignment _alignment) const;
		GraphicsItemCfg* createShapeItem() const;

		void createText(GraphicsLayoutItemCfg* _layout, const TextInfo& _info, const std::string& _nameSuffix) const;
		void createImage(GraphicsLayoutItemCfg* _layout, const ImageInfo& _info, const std::string& _nameSuffix) const;

		std::string m_entityName;

		TextInfo m_topText;
		ImageInfo m_centerImage;
		TextInfo m_bottomText;

		std::string m_toolTip;

		Painter2DContainer m_backgroundPainter;
		PenFCfg m_outline;
		int m_cornerRadius;

		double m_connectorWidth;
		double m_connectorHeight;
	};

}