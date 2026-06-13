// @otlicense
// File: GraphicsHierarchicalItemBuilder.cpp
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
#include "OTCore/Logging/Logger.h"
#include "OTGui/Graphics/Builder/GraphicsHierarchicalItemBuilder.h"

#include "OTGui/Graphics/GraphicsTextItemCfg.h"
#include "OTGui/Graphics/GraphicsStackItemCfg.h"
#include "OTGui/Graphics/GraphicsImageItemCfg.h"
#include "OTGui/Graphics/GraphicsEllipseItemCfg.h"
#include "OTGui/Graphics/GraphicsTriangleItemCfg.h"
#include "OTGui/Graphics/GraphicsRectangularItemCfg.h"

#include "OTGui/Graphics/GraphicsLayoutItemCfg.h"
#include "OTGui/Graphics/GraphicsVBoxLayoutItemCfg.h"
#include "OTGui/Graphics/GraphicsHBoxLayoutItemCfg.h"
#include "OTGui/Graphics/GraphicsGridLayoutItemCfg.h"

#include "OTGui/Painter/Painter2D.h"
#include "OTGui/Painter/FillPainter2D.h"
#include "OTGui/Painter/Painter2DFactory.h"
#include "OTGui/Painter/StyleRefPainter2D.h"
#include "OTGui/Painter/LinearGradientPainter2D.h"
#include "OTGui/Painter/RadialGradientPainter2D.h"

ot::GraphicsItemCfg* ot::GraphicsHierarchicalItemBuilder::createGraphicsItem() const
{
	OTAssert(!m_entityName.empty(), "No entity name provided");
	
	// --- Create items ------------------------------------------------

	// Root
	GraphicsStackItemCfg* root = new GraphicsStackItemCfg;
	root->setName(m_entityName);
	root->setToolTip(m_toolTip);
	root->setGraphicsItemFlags(GraphicsItemCfg::ItemIsMoveable | GraphicsItemCfg::ItemSnapsToGridTopLeft | GraphicsItemCfg::ItemForwardsState | GraphicsItemCfg::ItemIsSelectable);

	// Central Stack
	GraphicsStackItemCfg* cStack = new GraphicsStackItemCfg;
	cStack->setName(m_entityName + "_cStack");
	cStack->setToolTip(m_toolTip);
	cStack->setMargins(m_connectorHeight);
	cStack->setGraphicsItemFlags(GraphicsItemCfg::ItemForwardsTooltip | GraphicsItemCfg::ItemForwardsState);
	root->addItemTop(cStack, true, false);

	// Connector Grid
	GraphicsGridLayoutItemCfg* conGrid = new GraphicsGridLayoutItemCfg(5, 5);
	conGrid->setName(m_entityName + "_conGrid");
	conGrid->setPosition(-m_connectorHeight, -m_connectorHeight);
	conGrid->setColumnStretch(1, 1);
	conGrid->setColumnStretch(3, 1);
	conGrid->setRowStretch(1, 1);
	conGrid->setRowStretch(3, 1);
	conGrid->setSizePolicy(SizePolicy::Dynamic);
	root->addItemBottom(conGrid, false, true);

	// Connectors
	//conGrid->addChildItem(0, 0, createConnectorItem(Alignment::TopLeft));
	conGrid->addChildItem(0, 2, createConnectorItem(Alignment::Top));
	//conGrid->addChildItem(0, 4, createConnectorItem(Alignment::TopRight));
	conGrid->addChildItem(2, 0, createConnectorItem(Alignment::Left));
	conGrid->addChildItem(2, 4, createConnectorItem(Alignment::Right));
	//conGrid->addChildItem(4, 0, createConnectorItem(Alignment::BottomLeft));
	conGrid->addChildItem(4, 2, createConnectorItem(Alignment::Bottom));
	//conGrid->addChildItem(4, 4, createConnectorItem(Alignment::BottomRight));

	// Shape
	GraphicsItemCfg* shapeItm = createShapeItem();
	if (shapeItm)
	{
		cStack->addItemBottom(shapeItm, false, true);
	}

	// Main layout
	GraphicsVBoxLayoutItemCfg* mLay = new GraphicsVBoxLayoutItemCfg;
	mLay->setName(m_entityName + "_mLay");
	mLay->setMinimumSize(Size2DD(50., 80.));
	mLay->setGraphicsItemFlags(GraphicsItemCfg::ItemForwardsTooltip | GraphicsItemCfg::ItemForwardsState);
	cStack->addItemTop(mLay, true, false);

	// Content
	createText(mLay, m_topText, "_ttxt");
	createImage(mLay, m_centerImage, "_cimg");
	createText(mLay, m_bottomText, "_btxt");

	return root;
}

ot::GraphicsHierarchicalItemBuilder::GraphicsHierarchicalItemBuilder()
	: m_cornerRadius(5), m_connectorWidth(7.), m_connectorHeight(5.)
{
	this->setBackgroundPainter(new ot::StyleRefPainter2D(ColorStyleValueEntry::GraphicsItemBackground));
	this->setOutlinePainter(new ot::StyleRefPainter2D(ColorStyleValueEntry::GraphicsItemBorder));
	this->setOutlineWidth(1.);

	this->initializeTextInfo(m_topText);
	this->initializeTextInfo(m_bottomText);
	this->initializeImageInfo(m_centerImage);
}

ot::GraphicsHierarchicalItemBuilder::~GraphicsHierarchicalItemBuilder()
{}

void ot::GraphicsHierarchicalItemBuilder::setBackgroundColor(const ot::Color& _color)
{
	this->setBackgroundPainter(new ot::FillPainter2D(_color));
}

// ###########################################################################################################################################################################################################################################################################################################################

// Private: Helper

void ot::GraphicsHierarchicalItemBuilder::initializeTextInfo(TextInfo& _info)
{
	_info.text = "";
	_info.font = Font();
	_info.margins = MarginsD(5., 5., 5., 5.);
	_info.pen.setWidth(1.);
	_info.pen.setPainter(new StyleRefPainter2D(ColorStyleValueEntry::GraphicsItemForeground));
	_info.alignment = Alignment::Center;
}

void ot::GraphicsHierarchicalItemBuilder::initializeImageInfo(ImageInfo& _info)
{
	_info.path = "";
	_info.data.clear();
	_info.format = ImageFileFormat::PNG;
	_info.margins = MarginsD(10., 10., 10., 10.);
	_info.alignment = Alignment::Center;
	_info.maintainAspectRatio = true;
	_info.minimumSize = Size2DD(0., 0.);
	_info.maximumSize = Size2DD(150., 150.);
}

ot::GraphicsItemCfg* ot::GraphicsHierarchicalItemBuilder::createConnectorItem(Alignment _alignment) const
{
	GraphicsEllipseItemCfg* con = new GraphicsEllipseItemCfg(0., 0., new StyleRefPainter2D(ColorStyleValueEntry::Transparent));
	con->setOutline(PenFCfg(1., new StyleRefPainter2D(ColorStyleValueEntry::Transparent)));
	con->setGraphicsItemFlags(GraphicsItemCfg::ItemIsConnectable | GraphicsItemCfg::ItemForwardsTooltip | GraphicsItemCfg::ItemHandlesState);
	con->setSizePolicy(SizePolicy::Preferred);

	constexpr double trigDist = GraphicsItemCfg::defaultAdditionalTriggerDistance();

	// Set name
	switch (_alignment)
	{
	case Alignment::TopLeft:
		con->setName("TopLeft");
		con->setRadiusX(m_connectorWidth);
		con->setRadiusY(m_connectorWidth);
		con->setConnectionDirection(ConnectionDirection::Up);
		con->setAdditionalTriggerDistance(trigDist, trigDist, 0., trigDist);
		break;

	case Alignment::Top:
		con->setName("Top");
		con->setRadiusX(m_connectorWidth);
		con->setRadiusY(m_connectorHeight);
		con->setConnectionDirection(ConnectionDirection::Up);
		con->setAdditionalTriggerDistance(trigDist, trigDist, 0., trigDist);
		break;

	case Alignment::TopRight:
		con->setName("TopRight");
		con->setRadiusX(m_connectorWidth);
		con->setRadiusY(m_connectorWidth);
		con->setConnectionDirection(ConnectionDirection::Up);
		con->setAdditionalTriggerDistance(trigDist, trigDist, 0., trigDist);
		break;

	case Alignment::Left:
		con->setName("Left");
		con->setRadiusX(m_connectorHeight);
		con->setRadiusY(m_connectorWidth);
		con->setConnectionDirection(ConnectionDirection::Left);
		con->setAdditionalTriggerDistance(trigDist, trigDist, trigDist, 0.);
		break;

	case Alignment::Right:
		con->setName("Right");
		con->setRadiusX(m_connectorHeight);
		con->setRadiusY(m_connectorWidth);
		con->setConnectionDirection(ConnectionDirection::Right);
		con->setAdditionalTriggerDistance(0., trigDist, trigDist, trigDist);
		break;

	case Alignment::BottomLeft:
		con->setName("BottomLeft");
		con->setRadiusX(m_connectorWidth);
		con->setRadiusY(m_connectorWidth);
		con->setConnectionDirection(ConnectionDirection::Down);
		con->setAdditionalTriggerDistance(trigDist, 0., trigDist, trigDist);
		break;

	case Alignment::Bottom:
		con->setName("Bottom");
		con->setRadiusX(m_connectorWidth);
		con->setRadiusY(m_connectorHeight);
		con->setConnectionDirection(ConnectionDirection::Down);
		con->setAdditionalTriggerDistance(trigDist, 0., trigDist, trigDist);
		break;

	case Alignment::BottomRight:
		con->setName("BottomRight");
		con->setRadiusX(m_connectorWidth);
		con->setRadiusY(m_connectorWidth);
		con->setConnectionDirection(ConnectionDirection::Down);
		con->setAdditionalTriggerDistance(trigDist, 0., trigDist, trigDist);
		break;

	default:
		OT_LOG_E("Invalid connection direction (" + std::to_string((int)_alignment) + ")");
		break;
	}

	return con;
}

ot::GraphicsItemCfg* ot::GraphicsHierarchicalItemBuilder::createShapeItem() const
{
	GraphicsRectangularItemCfg* shape = new GraphicsRectangularItemCfg(m_backgroundPainter->createCopy());
	shape->setOutline(m_outline);
	shape->setCornerRadius(m_cornerRadius);
	shape->setName(m_entityName + "_shape");
	shape->setSizePolicy(SizePolicy::Dynamic);
	shape->setGraphicsItemFlags(GraphicsItemCfg::ItemForwardsTooltip | GraphicsItemCfg::ItemHandlesState);

	return shape;
}

void ot::GraphicsHierarchicalItemBuilder::createText(GraphicsLayoutItemCfg* _layout, const TextInfo& _info, const std::string& _nameSuffix) const
{
	if (_info.text.empty())
	{
		return;
	}

	GraphicsTextItemCfg* itm = new GraphicsTextItemCfg(_info.text);
	itm->setName(m_entityName + _nameSuffix);
	itm->setTextFont(_info.font);
	itm->setMargins(_info.margins);
	itm->setTextStyle(_info.pen);
	itm->setAlignment(_info.alignment);

	_layout->addChildItem(itm);
}

void ot::GraphicsHierarchicalItemBuilder::createImage(GraphicsLayoutItemCfg* _layout, const ImageInfo& _info, const std::string& _nameSuffix) const
{
	if (_info.data.empty() && _info.path.empty())
	{
		return;
	}

	OTAssert(_info.minimumSize.getWidth() <= _info.maximumSize.getWidth(), "Minimum width must be smaller or equal to maximum width");
	OTAssert(_info.minimumSize.getHeight() <= _info.maximumSize.getHeight(), "Minimum height must be smaller or equal to maximum height");

	GraphicsImageItemCfg* itm = new GraphicsImageItemCfg;
	itm->setImagePath(_info.path);
	itm->setImageData(_info.data, _info.format);
	itm->setName(m_entityName + _nameSuffix);
	itm->setMargins(_info.margins);
	itm->setSizePolicy(SizePolicy::Dynamic);
	itm->setAlignment(_info.alignment);
	itm->setMaintainAspectRatio(_info.maintainAspectRatio);
	itm->setGraphicsItemFlags(GraphicsItemCfg::ItemForwardsTooltip);
	itm->setMinimumSize(_info.minimumSize);
	itm->setMaximumSize(_info.maximumSize);

	_layout->addChildItem(itm);
}
