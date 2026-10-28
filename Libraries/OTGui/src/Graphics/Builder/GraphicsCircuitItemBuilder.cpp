// @otlicense
// File: GraphicsCircuitItemBuilder.cpp
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

// OpenTwin header
#include "OTGui/Graphics/Builder/GraphicsCircuitItemBuilder.h"
#include "OTGui/Graphics/GraphicsLineItemCfg.h"
#include "OTGui/Graphics/GraphicsRectangularItemCfg.h"
#include "OTGui/Graphics/GraphicsEllipseItemCfg.h"
#include "OTGui/Graphics/GraphicsTextItemCfg.h"
#include "OTGui/Graphics/GraphicsGroupItemCfg.h"
#include "OTGui/Graphics/GraphicsStackItemCfg.h"
#include "OTGui/Graphics/GraphicsGridLayoutItemCfg.h"
#include "OTGui/Graphics/GraphicsVBoxLayoutItemCfg.h"
#include "OTGui/Graphics/GraphicsHBoxLayoutItemCfg.h"
#include "OTGui/Painter/StyleRefPainter2D.h"
#include "OTGui/Painter/FillPainter2D.h"
#include "OTCore/Logging/Logger.h"

// std header
#include <algorithm>
#include <cmath>

ot::GraphicsCircuitConnector::GraphicsCircuitConnector() 
	: m_side(Left)
{
	m_font.setSize(8);
}

ot::GraphicsCircuitConnector::GraphicsCircuitConnector(const std::string& _name, const std::string& _text, ConnectorSide _side)
	: m_name(_name), m_text(_text), m_side(_side)
{
	m_font.setSize(8);
}

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################


ot::GraphicsCircuitItemBuilder::GraphicsCircuitItemBuilder()
	: m_cornerRadius(0.0), m_minimumSize(80.0, 60.0),
	m_backgroundPainter(nullptr), m_borderPainter(nullptr), m_titlePainter(nullptr)
{
}

ot::GraphicsCircuitItemBuilder::~GraphicsCircuitItemBuilder() 
{
	if (m_backgroundPainter) delete m_backgroundPainter;
	if (m_borderPainter) delete m_borderPainter;
	if (m_titlePainter) delete m_titlePainter;
}

void ot::GraphicsCircuitItemBuilder::addLeft(const std::string& _name, const std::string& _title) 
{
	m_left.emplace_back(_name, _title, GraphicsCircuitConnector::Left);
}

void ot::GraphicsCircuitItemBuilder::addRight(const std::string& _name, const std::string& _title) 
{
	m_right.emplace_back(_name, _title, GraphicsCircuitConnector::Right);
}

void ot::GraphicsCircuitItemBuilder::addTop(const std::string& _name, const std::string& _title)
{
	m_top.emplace_back(_name, _title, GraphicsCircuitConnector::Top);
}

void ot::GraphicsCircuitItemBuilder::addBottom(const std::string& _name, const std::string& _title)
{
	m_bottom.emplace_back(_name, _title, GraphicsCircuitConnector::Bottom);
}

void ot::GraphicsCircuitItemBuilder::addConnector(const GraphicsCircuitConnector& _connector) 
{
	switch (_connector.getSide()) 
	{
	case GraphicsCircuitConnector::Left:   m_left.push_back(_connector);   break;
	case GraphicsCircuitConnector::Right:  m_right.push_back(_connector);  break;
	case GraphicsCircuitConnector::Top:    m_top.push_back(_connector);    break;
	case GraphicsCircuitConnector::Bottom: m_bottom.push_back(_connector); break;
	}
}

void ot::GraphicsCircuitItemBuilder::setBackgroundColor(const ot::Color& _color) 
{
	this->setBackgroundPainter(new FillPainter2D(_color));
}

void ot::GraphicsCircuitItemBuilder::setBorderColor(const ot::Color& _color) 
{
	this->setBorderPainter(new FillPainter2D(_color));
}

void ot::GraphicsCircuitItemBuilder::setTitleColor(const ot::Color& _color) 
{
	this->setTitlePainter(new FillPainter2D(_color));
}

void ot::GraphicsCircuitItemBuilder::setBackgroundPainter(ot::Painter2D* _painter) 
{
	if (m_backgroundPainter == _painter) return;
	if (m_backgroundPainter) delete m_backgroundPainter;
	m_backgroundPainter = _painter;
}

void ot::GraphicsCircuitItemBuilder::setBorderPainter(ot::Painter2D* _painter) 
{
	if (m_borderPainter == _painter) return;
	if (m_borderPainter) delete m_borderPainter;
	m_borderPainter = _painter;
}

void ot::GraphicsCircuitItemBuilder::setTitlePainter(ot::Painter2D* _painter) 
{
	if (m_titlePainter == _painter) return;
	if (m_titlePainter) delete m_titlePainter;
	m_titlePainter = _painter;
}

ot::GraphicsItemCfg* ot::GraphicsCircuitItemBuilder::createPinItem(const GraphicsCircuitConnector& _connector) const
{
	const double lineLength = 20.0;
	const double circleDiameter = 10.0;
	const double circleRadius = circleDiameter / 2.0;
	const double layoutSpacing = 4.5;

	// 1. Group for Circle + Line + Label (Fixed 20-unit slot geometry centered on grid)
	ot::GraphicsGroupItemCfg* pinGroup = new ot::GraphicsGroupItemCfg;
	pinGroup->setName(_connector.getName() + "_pinGroup");

	ot::GraphicsLineItemCfg* line = new ot::GraphicsLineItemCfg;
	line->setName(_connector.getName() + "_line");
	line->setWidth(1.5);
	line->setPainter(new FillPainter2D(ot::Color(255, 0, 0)));

	ot::GraphicsEllipseItemCfg* connector = new ot::GraphicsEllipseItemCfg(circleRadius, circleRadius);
	connector->setName(_connector.getName());
	connector->setToolTip(_connector.getToolTip());
	connector->setBackgroundPainer(new StyleRefPainter2D(ColorStyleValueEntry::Transparent));
	connector->setOutline(ot::PenFCfg(0.0, new StyleRefPainter2D(ColorStyleValueEntry::Transparent)));
	connector->setGraphicsItemFlags(
		GraphicsItemCfg::ItemIsConnectable |
		GraphicsItemCfg::ItemSnapsToGridTopLeft
	);
	connector->setAdditionalTriggerDistance(MarginsD(10., 10., 10., 10.));

	// 2. Text label
	ot::GraphicsTextItemCfg* label = new ot::GraphicsTextItemCfg;
	label->setName(_connector.getName() + "_title");
	label->setText(_connector.getText());
	label->setTextFont(_connector.getFont());
	label->setTextPainter(new StyleRefPainter2D(ColorStyleValueEntry::GraphicsItemForeground));
	label->setToolTip(_connector.getToolTip());

	switch (_connector.getSide())
	{
	case GraphicsCircuitConnector::Left:
	{
		connector->setPosition(0.0, 5.0);
		connector->setConnectionDirection(ConnectionDirection::Left);

		line->setPosition(circleRadius, 10.0);
		line->setFrom(0.0, 0.0);
		line->setTo(lineLength + layoutSpacing, 0.0);
		line->setMaximumSize(ot::Size2DD(lineLength, circleDiameter));

		label->setPosition(circleDiameter, 11.0);
		label->setAlignment(Alignment::Right);

		pinGroup->addItem(line);
		pinGroup->addItem(connector);
		pinGroup->addItem(label);
		pinGroup->setAlignment(Alignment::Right);
		pinGroup->setMinimumSize(ot::Size2DD(circleRadius + lineLength, 20.0));
		pinGroup->setMaximumSize(ot::Size2DD(circleRadius + lineLength, 20.0));

		return pinGroup;
	}
	case GraphicsCircuitConnector::Right:
	{
		connector->setPosition(lineLength - circleRadius, 5.0);
		connector->setConnectionDirection(ConnectionDirection::Right);

		line->setPosition(0.0, 10.0);
		line->setFrom(-layoutSpacing, 0.0);
		line->setTo(lineLength, 0.0);
		line->setMaximumSize(ot::Size2DD(lineLength, circleDiameter));

		label->setPosition(0.0, 11.0);
		label->setAlignment(Alignment::Left);

		pinGroup->addItem(line);
		pinGroup->addItem(connector);
		pinGroup->addItem(label);
		pinGroup->setAlignment(Alignment::Left);
		pinGroup->setMinimumSize(ot::Size2DD(circleRadius + lineLength, 20.0));
		pinGroup->setMaximumSize(ot::Size2DD(circleRadius + lineLength, 20.0));

		return pinGroup;
	}
	case GraphicsCircuitConnector::Top:
	{
		connector->setPosition(5.0, 0.0);
		connector->setConnectionDirection(ConnectionDirection::Up);

		line->setPosition(10.0, circleRadius);
		line->setFrom(0.0, 0.0);
		line->setTo(0.0, lineLength + layoutSpacing);
		line->setMaximumSize(ot::Size2DD(circleDiameter, lineLength));

		label->setPosition(12.0, 5.0);
		label->setAlignment(Alignment::Bottom);

		pinGroup->addItem(line);
		pinGroup->addItem(connector);
		pinGroup->addItem(label);
		pinGroup->setAlignment(Alignment::Bottom);
		pinGroup->setMinimumSize(ot::Size2DD(20.0, circleRadius + lineLength));
		pinGroup->setMaximumSize(ot::Size2DD(20.0, circleRadius + lineLength));

		return pinGroup;
	}
	case GraphicsCircuitConnector::Bottom:
	{
		connector->setPosition(5.0, lineLength - circleRadius);
		connector->setConnectionDirection(ConnectionDirection::Down);

		line->setPosition(10.0, 0.0);
		line->setFrom(0.0, -layoutSpacing);
		line->setTo(0.0, lineLength);
		line->setMaximumSize(ot::Size2DD(circleDiameter, lineLength));

		label->setPosition(12.0, 0.0);
		label->setAlignment(Alignment::Top);

		pinGroup->addItem(line);
		pinGroup->addItem(connector);
		pinGroup->addItem(label);
		pinGroup->setAlignment(Alignment::Top);
		pinGroup->setMinimumSize(ot::Size2DD(20.0, circleRadius + lineLength));
		pinGroup->setMaximumSize(ot::Size2DD(20.0, circleRadius + lineLength));

		return pinGroup;
	}
	}

	return nullptr;
}

ot::GraphicsBoxLayoutItemCfg* ot::GraphicsCircuitItemBuilder::createSideLayout(const std::vector<GraphicsCircuitConnector>& _connectors, GraphicsCircuitConnector::ConnectorSide _side) const
{
	if (_connectors.empty())
	{
		return nullptr;
	}

	if (_side == GraphicsCircuitConnector::Left || _side == GraphicsCircuitConnector::Right)
	{
		ot::GraphicsVBoxLayoutItemCfg* vBox = new ot::GraphicsVBoxLayoutItemCfg;
		Alignment pinAlign = (_side == GraphicsCircuitConnector::Left) ? Alignment::Right : Alignment::Left;
		vBox->setAlignment(pinAlign);
		vBox->addStrech(1);
		for (const auto& c : _connectors)
		{
			ot::GraphicsItemCfg* pin = createPinItem(c);
			if (pin) pin->setAlignment(pinAlign);
			vBox->addChildItem(pin);
		}
		vBox->addStrech(1);
		return vBox;
	}
	else
	{
		ot::GraphicsHBoxLayoutItemCfg* hBox = new ot::GraphicsHBoxLayoutItemCfg;
		Alignment pinAlign = (_side == GraphicsCircuitConnector::Top) ? Alignment::Bottom : Alignment::Top;
		hBox->setAlignment(pinAlign);
		hBox->addStrech(1);
		for (const auto& c : _connectors)
		{
			ot::GraphicsItemCfg* pin = createPinItem(c);
			if (pin) pin->setAlignment(pinAlign);
			hBox->addChildItem(pin);
		}
		hBox->addStrech(1);
		return hBox;
	}
}

ot::GraphicsItemCfg* ot::GraphicsCircuitItemBuilder::createGraphicsItem() const 
{
	OTAssert(!m_name.empty(), "No name provided for GraphicsCircuitItemBuilder");

	// 1. Root Stack Item (handles selection, move, click, and forwards state)
	ot::GraphicsStackItemCfg* root = new ot::GraphicsStackItemCfg;
	root->setName(m_name);
	root->setTitle(m_title);
	root->setToolTip(m_toolTip);
	root->setGraphicsItemFlags(
		ot::GraphicsItemCfg::ItemIsMoveable |
		ot::GraphicsItemCfg::ItemIsSelectable |
		ot::GraphicsItemCfg::ItemSnapsToGridTopLeft |
		ot::GraphicsItemCfg::ItemUserTransformEnabled |
		ot::GraphicsItemCfg::ItemParticipatesInStateHandling |
		ot::GraphicsItemCfg::ItemForwardsState |
		ot::GraphicsItemCfg::ItemIsDoubleClickable
	);

	// 2. 3x3 Grid Layout (master in root)
	ot::GraphicsGridLayoutItemCfg* grid = new ot::GraphicsGridLayoutItemCfg(3, 3);
	grid->setName(m_name + "_grid");
	grid->setGraphicsItemFlags(GraphicsItemCfg::ItemForwardsState);
	grid->setRowStretch(1, 1);
	grid->setColumnStretch(1, 1);

	// 3. Central Body in Cell (1, 1): Stack with Border (slave) and VBox (master)
	ot::GraphicsStackItemCfg* bodyStack = new ot::GraphicsStackItemCfg;
	bodyStack->setName(m_name + "_bodyStack");
	bodyStack->setMinimumSize(m_minimumSize);
	bodyStack->setGraphicsItemFlags(GraphicsItemCfg::ItemForwardsState);
	bodyStack->setAlignment(Alignment::Center);

	ot::Painter2D* bgPainter = m_backgroundPainter ? m_backgroundPainter->createCopy() : new StyleRefPainter2D(ColorStyleValueEntry::GraphicsItemBackground);
	ot::Painter2D* borderPainter = m_borderPainter ? m_borderPainter->createCopy() : new StyleRefPainter2D(ColorStyleValueEntry::GraphicsItemBorder);

	ot::GraphicsRectangularItemCfg* body = new ot::GraphicsRectangularItemCfg(bgPainter);
	body->setName(m_name + "_body");
	body->setOutline(ot::PenFCfg(1.5, borderPainter));
	body->setCornerRadius(static_cast<int>(m_cornerRadius));
	body->setSizePolicy(SizePolicy::Dynamic);
	body->setGraphicsItemFlags(GraphicsItemCfg::ItemUsesStateStyling);
	bodyStack->addItemTop(body, false, true); // isMaster = false, isSlave = true

	ot::GraphicsVBoxLayoutItemCfg* bodyVBox = new ot::GraphicsVBoxLayoutItemCfg;
	bodyVBox->setName(m_name + "_bodyVBox");
	bodyVBox->setMinimumSize(m_minimumSize);

	ot::GraphicsHBoxLayoutItemCfg* titleHBox = new ot::GraphicsHBoxLayoutItemCfg;
	titleHBox->setName(m_name + "_titleHBox");

	ot::GraphicsTextItemCfg* titleItem = new ot::GraphicsTextItemCfg;
	titleItem->setName(m_name + "_centerTitle");
	titleItem->setText(m_title);
	titleItem->setAlignment(Alignment::Center);
	titleItem->setMargins(ot::MarginsD(10.0, 6.0, 10.0, 0.0));
	ot::Font titleFont;
	titleFont.setSize(10);
	titleFont.setBold(true);
	titleItem->setTextFont(titleFont);
	ot::Painter2D* titlePainter = m_titlePainter ? m_titlePainter->createCopy() : new StyleRefPainter2D(ColorStyleValueEntry::GraphicsItemForeground);
	titleItem->setTextPainter(titlePainter);

	titleHBox->addStrech(1);
	titleHBox->addChildItem(titleItem);
	titleHBox->addStrech(1);

	bodyVBox->addChildItem(titleHBox);
	bodyVBox->addStrech(1); // Stretch to fill flexible body height

	bodyStack->addItemTop(bodyVBox, true, false); // isMaster = true, isSlave = false

	grid->addChildItem(1, 1, bodyStack);

	// 4. Side layouts for Pins (Cells: Top (0,1), Bottom (2,1), Left (1,0), Right (1,2))
	ot::GraphicsBoxLayoutItemCfg* topLay = createSideLayout(m_top, GraphicsCircuitConnector::Top);
	if (topLay)
	{
		topLay->setName(m_name + "_topLay");
		topLay->setAlignment(Alignment::Bottom);
		grid->addChildItem(0, 1, topLay);
	}

	ot::GraphicsBoxLayoutItemCfg* bottomLay = createSideLayout(m_bottom, GraphicsCircuitConnector::Bottom);
	if (bottomLay)
	{
		bottomLay->setName(m_name + "_bottomLay");
		bottomLay->setAlignment(Alignment::Top);
		grid->addChildItem(2, 1, bottomLay);
	}

	ot::GraphicsBoxLayoutItemCfg* leftLay = createSideLayout(m_left, GraphicsCircuitConnector::Left);
	if (leftLay)
	{
		leftLay->setName(m_name + "_leftLay");
		leftLay->setAlignment(Alignment::Right);
		grid->addChildItem(1, 0, leftLay);
	}

	ot::GraphicsBoxLayoutItemCfg* rightLay = createSideLayout(m_right, GraphicsCircuitConnector::Right);
	if (rightLay)
	{
		rightLay->setName(m_name + "_rightLay");
		rightLay->setAlignment(Alignment::Left);
		grid->addChildItem(1, 2, rightLay);
	}

	root->addItemTop(grid, true, false);

	return root;
}