// @otlicense
// File: GraphicsCircuitItemBuilder.h
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
#include "OTGui/OTGuiAPIExport.h"
#include "OTGui/Style/Font.h"
#include "OTGui/Style/PenCfg.h"
#include "OTGui/Style/Margins.h"
#include "OTGui/Graphics/GraphicsItemCfg.h"
#include "OTGui/Graphics/GraphicsGroupItemCfg.h"

// std header
#include <string>
#include <vector>

namespace ot {
	class GraphicsBoxLayoutItemCfg;

	//! @brief Connector element positioned on one of the four sides of a circuit block item.
	class OT_GUI_API_EXPORT GraphicsCircuitConnector
	{
	public:
		enum ConnectorSide 
		{
			Left,
			Right,
			Top,
			Bottom
		};

		GraphicsCircuitConnector();
		GraphicsCircuitConnector(const std::string& _name, const std::string& _text, ConnectorSide _side);

		void setName(const std::string& _name) { m_name = _name; };
		const std::string& getName() const { return m_name; };

		void setText(const std::string& _text) { m_text = _text; };
		const std::string& getText() const { return m_text; };

		void setToolTip(const std::string& _toolTip) { m_toolTip = _toolTip; };
		const std::string& getToolTip() const { return m_toolTip; };

		void setSide(ConnectorSide _side) { m_side = _side; };
		ConnectorSide getSide() const { return m_side; };

		void setFont(const ot::Font& _font) { m_font = _font; };
		const ot::Font& getFont() const { return m_font; };

	private:
		std::string m_name;
		std::string m_text;
		std::string m_toolTip;
		ConnectorSide m_side;
		ot::Font m_font;
	};


	//! @brief Builder for creating rectangular sub-circuit items with connectors on the four sides and a centered title, ideal for subcircuits
	class OT_GUI_API_EXPORTONLY GraphicsCircuitItemBuilder
	{
		OT_DECL_NOCOPY(GraphicsCircuitItemBuilder)
			OT_DECL_NOMOVE(GraphicsCircuitItemBuilder)

	public:
		GraphicsCircuitItemBuilder();
		virtual ~GraphicsCircuitItemBuilder();

		//! @brief Creates the GraphicsItemCfg hierarchy. The caller takes ownership of the created pointer.
		ot::GraphicsItemCfg* createGraphicsItem() const;

		void setName(const std::string& _name) { m_name = _name; };
		const std::string& getName() const { return m_name; };

		void setTitle(const std::string& _title) { m_title = _title; };
		const std::string& getTitle() const { return m_title; };

		void setToolTip(const std::string& _toolTip) { m_toolTip = _toolTip; };
		const std::string& getToolTip() const { return m_toolTip; };

		void setCornerRadius(double _radius) { m_cornerRadius = _radius; };
		double getCornerRadius() const { return m_cornerRadius; };

		void setMinimumSize(const ot::Size2DD& _size) { m_minimumSize = _size; };
		const ot::Size2DD& getMinimumSize() const { return m_minimumSize; };

		// Add connectors to the four sides
		void addLeft(const std::string& _name, const std::string& _title);
		void addRight(const std::string& _name, const std::string& _title);
		void addTop(const std::string& _name, const std::string& _title);
		void addBottom(const std::string& _name, const std::string& _title);

		void addConnector(const GraphicsCircuitConnector& _connector);
		void setBackgroundColor(const ot::Color& _color);
		void setBorderColor(const ot::Color& _color);
		void setTitleColor(const ot::Color& _color);
		void setBackgroundPainter(ot::Painter2D* _painter);
		void setBorderPainter(ot::Painter2D* _painter);
		void setTitlePainter(ot::Painter2D* _painter);

	private:
		ot::GraphicsBoxLayoutItemCfg* createSideLayout(const std::vector<GraphicsCircuitConnector>& _connectors, GraphicsCircuitConnector::ConnectorSide _side) const;
		ot::GraphicsItemCfg* createPinItem(const GraphicsCircuitConnector& _connector) const;

		std::string m_name;
		std::string m_title;
		std::string m_toolTip;
		double m_cornerRadius;
		ot::Size2DD m_minimumSize;
		ot::Painter2D* m_backgroundPainter;
		ot::Painter2D* m_borderPainter;
		ot::Painter2D* m_titlePainter;
		std::vector<GraphicsCircuitConnector> m_left;
		std::vector<GraphicsCircuitConnector> m_right;
		std::vector<GraphicsCircuitConnector> m_top;
		std::vector<GraphicsCircuitConnector> m_bottom;
	};
}