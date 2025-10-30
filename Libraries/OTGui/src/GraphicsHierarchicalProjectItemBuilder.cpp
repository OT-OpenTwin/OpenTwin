// @otlicense
// File: GraphicsHierarchicalProjectItemBuilder.cpp
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
#include "OTCore/LogDispatcher.h"
#include "OTGui/GraphicsHierarchicalProjectItemBuilder.h"

#include "OTGui/GraphicsTextItemCfg.h"
#include "OTGui/GraphicsStackItemCfg.h"
#include "OTGui/GraphicsImageItemCfg.h"
#include "OTGui/GraphicsEllipseItemCfg.h"
#include "OTGui/GraphicsFlowItemBuilder.h"
#include "OTGui/GraphicsTriangleItemCfg.h"
#include "OTGui/GraphicsRectangularItemCfg.h"

#include "OTGui/GraphicsLayoutItemCfg.h"
#include "OTGui/GraphicsVBoxLayoutItemCfg.h"
#include "OTGui/GraphicsHBoxLayoutItemCfg.h"
#include "OTGui/GraphicsGridLayoutItemCfg.h"

#include "OTGui/Painter2D.h"
#include "OTGui/FillPainter2D.h"
#include "OTGui/Painter2DFactory.h"
#include "OTGui/StyleRefPainter2D.h"
#include "OTGui/LinearGradientPainter2D.h"
#include "OTGui/RadialGradientPainter2D.h"

ot::GraphicsItemCfg* ot::GraphicsHierarchicalProjectItemBuilder::createGraphicsItem() const {
	OTAssert(!m_name.empty(), "No name provided");

	// --- Create items ------------------------------------------------

	// Root
	GraphicsStackItemCfg* root = new GraphicsStackItemCfg;
	root->setName(m_name);
	root->setTitle(m_title);
	root->setToolTip(m_toolTip);
	root->setGraphicsItemFlags(GraphicsItemCfg::ItemIsMoveable | GraphicsItemCfg::ItemSnapsToGridTopLeft | GraphicsItemCfg::ItemForwardsState | GraphicsItemCfg::ItemIsSelectable);

	// Central Stack
	GraphicsStackItemCfg* cStack = new GraphicsStackItemCfg;
	cStack->setName(m_name + "_cStack");
	cStack->setTitle(m_title);
	cStack->setToolTip(m_toolTip);
	cStack->setMargins(m_connectorHeight);
	cStack->setGraphicsItemFlags(GraphicsItemCfg::ItemForwardsTooltip | GraphicsItemCfg::ItemForwardsState);
	root->addItemTop(cStack, true, false);

	// Connector Grid
	GraphicsGridLayoutItemCfg* conGrid = new GraphicsGridLayoutItemCfg(5, 5);
	conGrid->setName(m_name + "_conGrid");
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

	// Border
	GraphicsRectangularItemCfg* bor = new GraphicsRectangularItemCfg(new StyleRefPainter2D(ColorStyleValueEntry::GraphicsItemBackground));
	bor->setOutline(PenFCfg(1., new StyleRefPainter2D(ColorStyleValueEntry::GraphicsItemBorder)));
	bor->setCornerRadius(5);
	bor->setName(m_name + "_bor");
	bor->setSizePolicy(SizePolicy::Dynamic);
	bor->setGraphicsItemFlags(GraphicsItemCfg::ItemForwardsTooltip | GraphicsItemCfg::ItemHandlesState);
	cStack->addItemTop(bor, false, true);

	// Main layout
	GraphicsVBoxLayoutItemCfg* mLay = new GraphicsVBoxLayoutItemCfg;
	mLay->setName(m_name + "_mLay");
	mLay->setMinimumSize(Size2DD(50., 80.));
	mLay->setGraphicsItemFlags(GraphicsItemCfg::ItemForwardsTooltip | GraphicsItemCfg::ItemForwardsState);
	cStack->addItemTop(mLay, true, false);

	// Title
	mLay->addChildItem(createTitle(), 0);

	// Central layout
	GraphicsVBoxLayoutItemCfg* cLay = new GraphicsVBoxLayoutItemCfg;
	cLay->setName(m_name + "_cLay");
	cLay->setGraphicsItemFlags(GraphicsItemCfg::ItemForwardsTooltip);

	// Info style
	ot::MarginsD infoMargins(5., 0., 5., 0.);
	ot::Font infoFont;
	infoFont.setSize(10);
	ot::PenFCfg infoPen(1., new ot::StyleRefPainter2D(ColorStyleValueEntry::GraphicsItemForeground));

	// Project type
	if (!m_type.empty()) {
		GraphicsTextItemCfg* typeItm = new GraphicsTextItemCfg("Project Type: " + m_type);
		typeItm->setName(m_name + "_ptype");
		typeItm->setAlignment(Alignment::Left);
		typeItm->setMargins(infoMargins);
		typeItm->setTextFont(infoFont);
		typeItm->setTextStyle(infoPen);
		
		cLay->addChildItem(typeItm);
	}

	// Project version
	if (!m_projectVersion.empty()) {
		GraphicsTextItemCfg* verItm = new GraphicsTextItemCfg("Version: " + m_projectVersion);
		verItm->setName(m_name + "_pver");
		verItm->setAlignment(Alignment::Left);
		ot::Font font = infoFont;
		font.setSize(font.size() - 2);
		font.setItalic(true);
		verItm->setTextFont(font);
		verItm->setTextStyle(infoPen);
		verItm->setMargins(infoMargins);

		cLay->addChildItem(verItm);
	}
	
	// Create background image
	if (!m_previewImage.empty() || !m_previewImagePath.empty()) {
		GraphicsImageItemCfg* cImg = new GraphicsImageItemCfg;
		cImg->setImagePath(m_previewImagePath);
		cImg->setImageData(m_previewImage, m_previewImageFormat);
		cImg->setName(m_name + "_cPrevImg");
		cImg->setMargins(m_previewImageMargins);
		cImg->setSizePolicy(SizePolicy::Dynamic);
		cImg->setAlignment(Alignment::Center);
		cImg->setMaintainAspectRatio(true);
		cImg->setGraphicsItemFlags(GraphicsItemCfg::ItemForwardsTooltip);
		cImg->setMaximumSize(Size2DD(150., 150.));

		cLay->addChildItem(cImg, 1);
	}

	cLay->addStrech(1);

	// Add central layout to main layout
	mLay->addChildItem(cLay, 1);

	return root;
}

ot::GraphicsHierarchicalProjectItemBuilder::GraphicsHierarchicalProjectItemBuilder()
	: m_titleBackgroundPainter(nullptr), m_titleForegroundPainter(nullptr), m_previewImageFormat(ot::ImageFileFormat::PNG),
	m_previewImageMargins(10., 10., 10., 10.), m_connectorWidth(7.), m_connectorHeight(5.)
{
	this->setTitleBackgroundColor(ot::Color(70, 70, 70));
	this->setDefaultTitleForegroundGradient();
}

ot::GraphicsHierarchicalProjectItemBuilder::~GraphicsHierarchicalProjectItemBuilder() {}

void ot::GraphicsHierarchicalProjectItemBuilder::setTitleBackgroundPainter(ot::Painter2D* _painter) {
	if (m_titleBackgroundPainter == _painter) return;
	if (m_titleBackgroundPainter) delete m_titleBackgroundPainter;
	m_titleBackgroundPainter = _painter;
}

void ot::GraphicsHierarchicalProjectItemBuilder::setTitleBackgroundGradientColor(const ot::Color& _color) {
	ot::RadialGradientPainter2D* painter = new ot::RadialGradientPainter2D;
	painter->setCenterPoint(ot::Point2DD(0., 2.));
	painter->setCenterRadius(2.5);
	painter->setSpread(GradientSpread::Reflect);

	painter->addStop(0.00, _color);
	painter->addStop(0.75, ot::Color(50, 50, 50));
	painter->addStop(0.80, ot::Color(30, 30, 30));
	painter->addStop(0.85, ot::Color(230, 230, 230));
	painter->addStop(0.90, ot::Color(50, 50, 50));

	this->setTitleBackgroundPainter(painter);
}

void ot::GraphicsHierarchicalProjectItemBuilder::setTitleBackgroundColor(const ot::Color& _color) {
	this->setTitleBackgroundPainter(new ot::FillPainter2D(_color));
}

void ot::GraphicsHierarchicalProjectItemBuilder::setTitleForegroundPainter(ot::Painter2D* _painter) {
	if (m_titleForegroundPainter == _painter) return;
	if (m_titleForegroundPainter) delete m_titleForegroundPainter;
	m_titleForegroundPainter = _painter;
}

void ot::GraphicsHierarchicalProjectItemBuilder::setTitleForegroundColor(const ot::Color& _color) {
	this->setTitleForegroundPainter(new ot::FillPainter2D(_color));
}

void ot::GraphicsHierarchicalProjectItemBuilder::setDefaultTitleForegroundGradient() {
	ot::RadialGradientPainter2D* painter = new ot::RadialGradientPainter2D;
	painter->setCenterPoint(ot::Point2DD(0., 2.));
	painter->setCenterRadius(2.5);
	painter->setSpread(GradientSpread::Reflect);

	painter->addStop(0.00, ot::Color(255, 255, 255));
	painter->addStop(0.75, ot::Color(255, 255, 255));
	painter->addStop(0.80, ot::Color(230, 230, 230));
	painter->addStop(0.85, ot::Color(25, 25, 25));
	painter->addStop(0.90, ot::Color(255, 255, 255));

	this->setTitleForegroundPainter(painter);
}

// ###########################################################################################################################################################################################################################################################################################################################

// Private: Helper

ot::GraphicsItemCfg* ot::GraphicsHierarchicalProjectItemBuilder::createConnectorItem(Alignment _alignment) const {
	GraphicsEllipseItemCfg* con = new GraphicsEllipseItemCfg(0., 0., new StyleRefPainter2D(ColorStyleValueEntry::Transparent));
	con->setOutline(PenFCfg(1., new StyleRefPainter2D(ColorStyleValueEntry::Transparent)));
	con->setGraphicsItemFlags(GraphicsItemCfg::ItemIsConnectable | GraphicsItemCfg::ItemForwardsTooltip | GraphicsItemCfg::ItemHandlesState);
	con->setSizePolicy(SizePolicy::Preferred);

	constexpr double trigDist = GraphicsItemCfg::defaultAdditionalTriggerDistance();

	// Set name
	switch (_alignment) {
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

ot::GraphicsItemCfg* ot::GraphicsHierarchicalProjectItemBuilder::createTitle() const {
	OTAssertNullptr(m_titleBackgroundPainter);
	OTAssertNullptr(m_titleForegroundPainter);

	// Create a copy of the painter
	Painter2D* painterTitleBack = m_titleBackgroundPainter->createCopy();
	Painter2D* painterTitleFront = m_titleForegroundPainter->createCopy();

	OTAssertNullptr(painterTitleBack);
	OTAssertNullptr(painterTitleFront);

	// Title: Stack
	GraphicsStackItemCfg* tStack = new GraphicsStackItemCfg;
	tStack->setName(m_name + "_tStack");
	tStack->setGraphicsItemFlags(GraphicsItemCfg::ItemForwardsTooltip | GraphicsItemCfg::ItemForwardsState);

	// Title: Border
	GraphicsRectangularItemCfg* tBor = new GraphicsRectangularItemCfg(painterTitleBack);
	tBor->setOutline(PenFCfg(1., new StyleRefPainter2D(ColorStyleValueEntry::GraphicsItemBorder)));
	tBor->setName(m_name + "_tBor");
	tBor->setCornerRadius(5);
	//tBor->setSize(Size2DD(200., 30.));
	tBor->setSizePolicy(SizePolicy::Dynamic);
	tBor->setGraphicsItemFlags(GraphicsItemCfg::ItemForwardsTooltip | GraphicsItemCfg::ItemHandlesState);
	tStack->addItemTop(tBor, false, true);

	// Title: Layout
	GraphicsHBoxLayoutItemCfg* tLay = new GraphicsHBoxLayoutItemCfg;
	tLay->setName(m_name + "_tLay");
	tLay->setGraphicsItemFlags(GraphicsItemCfg::ItemForwardsTooltip);
	tStack->addItemTop(tLay, true, false);

	// Title: Title
	GraphicsTextItemCfg* tit = new GraphicsTextItemCfg;
	tit->setName(m_name + "_tit");
	tit->setText(m_title);
	tit->setTextPainter(painterTitleFront);
	tit->setMinimumSize(Size2DD(10., 22.));
	tit->setGraphicsItemFlags(GraphicsItemCfg::ItemForwardsTooltip);

	// Title: Left Corner
	if (m_leftTitleImagePath.empty()) {
		tLay->addStrech(1);
	}
	else {
		GraphicsImageItemCfg* titLImg = new GraphicsImageItemCfg;
		titLImg->setName(m_name + "_titLImg");
		titLImg->setImagePath(m_leftTitleImagePath);
		titLImg->setFixedSize(16., 16.);
		titLImg->setAlignment(Alignment::Center);
		titLImg->setMargins(MarginsD(5., 0., 0., 0.));
		titLImg->setGraphicsItemFlags(GraphicsItemCfg::ItemForwardsTooltip);
		tLay->addChildItem(titLImg);

		MarginsD titMargins = tit->getMargins();
		titMargins.setLeft(5.);
		tit->setMargins(titMargins);
	}

	tLay->addChildItem(tit);
	tLay->addStrech(1);

	return tStack;
}