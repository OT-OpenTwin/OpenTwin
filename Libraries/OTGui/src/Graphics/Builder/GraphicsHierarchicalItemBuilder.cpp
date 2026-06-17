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

std::string ot::GraphicsHierarchicalItemBuilder::backgroundShapeToString(BackgroundShape _shape)
{
	switch (_shape)
	{
	case BackgroundShape::None: return "None";
	case BackgroundShape::Rectangle: return "Rectangle";
	case BackgroundShape::Ellipse: return "Ellipse";
	default:
		OT_LOG_E("Unknown background shape: " + std::to_string(static_cast<int>(_shape)));
		return "Rectangle";
	}
}

ot::GraphicsHierarchicalItemBuilder::BackgroundShape ot::GraphicsHierarchicalItemBuilder::stringToBackgroundShape(const std::string& _shapeStr)
{
	if (_shapeStr == backgroundShapeToString(BackgroundShape::None))
	{
		return BackgroundShape::None;
	}
	else if (_shapeStr == backgroundShapeToString(BackgroundShape::Rectangle))
	{
		return BackgroundShape::Rectangle;
	}
	else if (_shapeStr == backgroundShapeToString(BackgroundShape::Ellipse))
	{
		return BackgroundShape::Ellipse;
	}
	else
	{
		OT_LOG_E("Unknown background shape string: " + _shapeStr);
		return BackgroundShape::Rectangle;
	}
}

std::list<std::string> ot::GraphicsHierarchicalItemBuilder::getBackgroundShapeSelectionValues()
{
	return std::list<std::string>({
		backgroundShapeToString(BackgroundShape::None),
		backgroundShapeToString(BackgroundShape::Rectangle),
		backgroundShapeToString(BackgroundShape::Ellipse)
		});
}

std::string ot::GraphicsHierarchicalItemBuilder::createExpanderItemName(const std::string& _entityName, Alignment _expanderAlignment)
{
	return _entityName + "_expander_" + ot::toString(_expanderAlignment);
}

ot::Alignment ot::GraphicsHierarchicalItemBuilder::expanderAlignmentFromItemName(const std::string& _itemName)
{
	size_t pos = _itemName.rfind("_expander_");
	if (pos == std::string::npos)
	{
		OT_LOG_E("Invalid expander item name: " + _itemName);
		return Alignment::Center;
	}
	std::string alignmentStr = _itemName.substr(pos + std::string("_expander_").length());
	return stringToAlignment(alignmentStr);
}

ot::GraphicsItemCfg* ot::GraphicsHierarchicalItemBuilder::createGraphicsItem() const
{
	OTAssert(!m_entityName.empty(), "No entity name provided");
	
	// --- Create items ------------------------------------------------

	// Root
	GraphicsStackItemCfg* root = new GraphicsStackItemCfg;
	root->setName(m_entityName);
	root->setToolTip(m_toolTip);
	root->setGraphicsItemFlags(
		GraphicsItemCfg::ItemIsMoveable
		| GraphicsItemCfg::ItemSnapsToGridTopLeft
		| GraphicsItemCfg::ItemForwardsState
		| GraphicsItemCfg::ItemIsSelectable
		| GraphicsItemCfg::ItemIsDoubleClickable
	);

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
	conGrid->addChildItem(0, 2, createConnectorItem(Alignment::Top));
	conGrid->addChildItem(2, 0, createConnectorItem(Alignment::Left));
	conGrid->addChildItem(2, 4, createConnectorItem(Alignment::Right));
	conGrid->addChildItem(4, 2, createConnectorItem(Alignment::Bottom));

	// Shape
	GraphicsItemCfg* shapeItm = createShapeItem();
	if (shapeItm)
	{
		cStack->addItemBottom(shapeItm, false, true);
	}

	// Main layout
	GraphicsGridLayoutItemCfg* mLay = new GraphicsGridLayoutItemCfg(3, 3);
	mLay->setName(m_entityName + "_mLay");
	mLay->setGraphicsItemFlags(GraphicsItemCfg::ItemForwardsTooltip | GraphicsItemCfg::ItemForwardsState);
	mLay->setColumnStretch(1, 1);
	mLay->setRowStretch(1, 1);
	cStack->addItemTop(mLay, true, false);

	// Expander buttons
	mLay->addChildItem(0, 1, createExpanderItem(Alignment::Top));
	mLay->addChildItem(1, 0, createExpanderItem(Alignment::Left));
	mLay->addChildItem(1, 2, createExpanderItem(Alignment::Right));
	mLay->addChildItem(2, 1, createExpanderItem(Alignment::Bottom));

	// Content layout
	GraphicsVBoxLayoutItemCfg* cLay = new GraphicsVBoxLayoutItemCfg;
	cLay->setName(m_entityName + "_cLay");
	cLay->setMinimumSize(m_minimumSize);
	cLay->setMaximumSize(m_maximumSize);
	cLay->setGraphicsItemFlags(GraphicsItemCfg::ItemForwardsTooltip | GraphicsItemCfg::ItemForwardsState);
	mLay->addChildItem(1, 1, cLay);

	// Content
	createText(cLay, m_topText, "_ttxt");
	createImage(cLay, m_centerImage, "_cimg");
	createText(cLay, m_bottomText, "_btxt");

	return root;
}

ot::GraphicsHierarchicalItemBuilder::GraphicsHierarchicalItemBuilder()
	: m_cornerRadius(5), m_connectorWidth(7.), m_connectorHeight(5.), m_backgroundShape(BackgroundShape::Rectangle),
	  m_minimumSize(Size2DD(10., 10.)), m_maximumSize(Size2DD(std::numeric_limits<double>::max(), std::numeric_limits<double>::max())),
	m_topExpanderState(ExpanderState::Expanded), m_bottomExpanderState(ExpanderState::Expanded), m_leftExpanderState(ExpanderState::Expanded), m_rightExpanderState(ExpanderState::Expanded)
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

ot::GraphicsItemCfg* ot::GraphicsHierarchicalItemBuilder::createExpanderItem(ot::Alignment _alignment) const
{
	ot::GraphicsBoxLayoutItemCfg* root = nullptr;
	if (_alignment == Alignment::Top || _alignment == Alignment::Bottom)
	{
		root = new GraphicsHBoxLayoutItemCfg;
	}
	else if (_alignment == Alignment::Left || _alignment == Alignment::Right)
	{
		root = new GraphicsVBoxLayoutItemCfg;
	}
	else
	{
		OT_LOG_E("Invalid expander item alignment: " + std::to_string((int)_alignment));
		return nullptr;
	}

	root->addStrech(1);

	GraphicsTriangleItemCfg* buttonItm = new GraphicsTriangleItemCfg;
	buttonItm->setName(createExpanderItemName(m_entityName, _alignment));
	buttonItm->setSizePolicy(SizePolicy::Preferred);
	buttonItm->setFixedSize(10., 10.);
	root->addChildItem(buttonItm);
	
	if (isExpanderVisible(_alignment))
	{
		buttonItm->setGraphicsItemFlags(GraphicsItemCfg::ItemIsClickable | GraphicsItemCfg::ItemHandlesState);
		buttonItm->setBackgroundPainer(new StyleRefPainter2D(ColorStyleValueEntry::GraphicsItemForeground));
		buttonItm->setOutline(PenFCfg(1., new StyleRefPainter2D(ColorStyleValueEntry::GraphicsItemForeground)));
		buttonItm->setTriangleDirection(expanderDiectionFromAlignment(_alignment));
	}
	else
	{
		buttonItm->setBackgroundPainer(new StyleRefPainter2D(ColorStyleValueEntry::Transparent));
		buttonItm->setOutline(PenFCfg(0., new StyleRefPainter2D(ColorStyleValueEntry::Transparent)));
	}

	root->addStrech(1);
	return root;
}

ot::GraphicsItemCfg* ot::GraphicsHierarchicalItemBuilder::createShapeItem() const
{
	switch (m_backgroundShape)
	{
	case ot::GraphicsHierarchicalItemBuilder::BackgroundShape::None: return nullptr;
	case ot::GraphicsHierarchicalItemBuilder::BackgroundShape::Rectangle: return createRectangleShapeItem();
	case ot::GraphicsHierarchicalItemBuilder::BackgroundShape::Ellipse: return createEllipseShapeItem();
	default:
		OT_LOG_E("Unknown background shape: " + std::to_string(static_cast<int>(m_backgroundShape)));
		return nullptr;
	}
}

ot::GraphicsItemCfg* ot::GraphicsHierarchicalItemBuilder::createRectangleShapeItem() const
{
	GraphicsRectangularItemCfg* shape = new GraphicsRectangularItemCfg(m_backgroundPainter->createCopy());
	shape->setOutline(m_outline);
	shape->setCornerRadius(m_cornerRadius);
	shape->setName(m_entityName + "_shape");
	shape->setSizePolicy(SizePolicy::Dynamic);
	shape->setGraphicsItemFlags(GraphicsItemCfg::ItemForwardsTooltip | GraphicsItemCfg::ItemHandlesState);

	return shape;
}

ot::GraphicsItemCfg* ot::GraphicsHierarchicalItemBuilder::createEllipseShapeItem() const
{
	GraphicsEllipseItemCfg* shape = new GraphicsEllipseItemCfg(5., 5., m_backgroundPainter->createCopy());
	shape->setOutline(m_outline);
	shape->setName(m_entityName + "_shape");
	shape->setSizePolicy(SizePolicy::Dynamic);
	shape->setGraphicsItemFlags(GraphicsItemCfg::ItemForwardsTooltip | GraphicsItemCfg::ItemHandlesState);

	return shape;
}

void ot::GraphicsHierarchicalItemBuilder::createText(GraphicsVBoxLayoutItemCfg* _layout, const TextInfo& _info, const std::string& _nameSuffix) const
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

void ot::GraphicsHierarchicalItemBuilder::createImage(GraphicsVBoxLayoutItemCfg* _layout, const ImageInfo& _info, const std::string& _nameSuffix) const
{
	if (_info.data.empty() && _info.path.empty())
	{
		return;
	}

	GraphicsImageItemCfg* itm = new GraphicsImageItemCfg;
	itm->setName(m_entityName + _nameSuffix);
	itm->setImagePath(_info.path);
	itm->setImageData(_info.data, _info.format);
	itm->setMargins(_info.margins);
	itm->setSizePolicy(SizePolicy::Dynamic);
	itm->setAlignment(_info.alignment);
	itm->setMaintainAspectRatio(_info.maintainAspectRatio);
	itm->setGraphicsItemFlags(GraphicsItemCfg::ItemForwardsTooltip);

	_layout->addChildItem(itm);
}

bool ot::GraphicsHierarchicalItemBuilder::isExpanderVisible(Alignment _alignment) const
{
	switch (_alignment)
	{
	case Alignment::Top: return m_topExpanderState != ExpanderState::None;
	case Alignment::Bottom: return m_bottomExpanderState != ExpanderState::None;
	case Alignment::Left: return m_leftExpanderState != ExpanderState::None;
	case Alignment::Right: return m_rightExpanderState != ExpanderState::None;
	default:
		OT_LOG_E("Invalid expander alignment: " + std::to_string((int)_alignment));
		return false;
	}
}

ot::GraphicsTriangleItemCfg::TriangleDirection ot::GraphicsHierarchicalItemBuilder::expanderDiectionFromAlignment(Alignment _alignment) const
{
	switch (_alignment)
	{
	case Alignment::Top: return (m_topExpanderState == ExpanderState::Expanded ? GraphicsTriangleItemCfg::GraphicsTriangleItemCfg::TriangleDirection::Down : GraphicsTriangleItemCfg::TriangleDirection::Up);
	case Alignment::Bottom: return (m_bottomExpanderState == ExpanderState::Expanded ? GraphicsTriangleItemCfg::TriangleDirection::Up : GraphicsTriangleItemCfg::TriangleDirection::Down);
	case Alignment::Left: return (m_leftExpanderState == ExpanderState::Expanded ? GraphicsTriangleItemCfg::TriangleDirection::Right : GraphicsTriangleItemCfg::TriangleDirection::Left);
	case Alignment::Right: return (m_rightExpanderState == ExpanderState::Expanded ? GraphicsTriangleItemCfg::TriangleDirection::Left : GraphicsTriangleItemCfg::TriangleDirection::Right);
	default:
		OT_LOG_E("Invalid expander alignment: " + std::to_string((int)_alignment));
		return GraphicsTriangleItemCfg::TriangleDirection::Down;
	}
}
