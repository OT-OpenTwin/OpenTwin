//! @file GraphicsHierarchicalProjectItemBuilder.cpp
//! 
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

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
	conGrid->setSizePolicy(Dynamic);
	root->addItemBottom(conGrid, false, true);

	// Connectors
	//conGrid->addChildItem(0, 0, createConnectorItem(ot::AlignTopLeft));
	conGrid->addChildItem(0, 2, createConnectorItem(ot::AlignTop));
	//conGrid->addChildItem(0, 4, createConnectorItem(ot::AlignTopRight));
	conGrid->addChildItem(2, 0, createConnectorItem(ot::AlignLeft));
	conGrid->addChildItem(2, 4, createConnectorItem(ot::AlignRight));
	//conGrid->addChildItem(4, 0, createConnectorItem(ot::AlignBottomLeft));
	conGrid->addChildItem(4, 2, createConnectorItem(ot::AlignBottom));
	//conGrid->addChildItem(4, 4, createConnectorItem(ot::AlignBottomRight));

	// Border
	GraphicsRectangularItemCfg* bor = new GraphicsRectangularItemCfg(new StyleRefPainter2D(ColorStyleValueEntry::GraphicsItemBackground));
	bor->setOutline(PenFCfg(1., new StyleRefPainter2D(ColorStyleValueEntry::GraphicsItemBorder)));
	bor->setCornerRadius(5);
	bor->setName(m_name + "_bor");
	bor->setSizePolicy(Dynamic);
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

	if (!m_type.empty()) {
		GraphicsTextItemCfg* typeItm = new GraphicsTextItemCfg("Project Type: " + m_type);
		typeItm->setName(m_name + "_ptype");
		typeItm->setAlignment(AlignLeft);
		
		cLay->addChildItem(typeItm);
	}
	
	// Create background image
	if (!m_backgroundImage.empty()) {
		/*GraphicsImageItemCfg* cImg = new GraphicsImageItemCfg;
		//cImg->setImagePath(m_backgroundImagePath);
		cImg->setName(m_name + "_cImg");
		cImg->setMargins(m_backgroundImageMargins);
		cImg->setSizePolicy(Dynamic);
		cImg->setAlignment(AlignCenter);
		cImg->setMaintainAspectRatio(true);
		cImg->setGraphicsItemFlags(GraphicsItemCfg::ItemForwardsTooltip);

		cLay->addChildItem(cImg, 1);*/
	}

	cLay->addStrech(1);

	// Add central layout to main layout
	mLay->addChildItem(cLay, 1);

	return root;
}

ot::GraphicsHierarchicalProjectItemBuilder::GraphicsHierarchicalProjectItemBuilder()
	: m_titleBackgroundPainter(nullptr), m_titleForegroundPainter(nullptr),
	m_backgroundImageMargins(5., 2., 2., 2.), m_connectorWidth(5.), m_connectorHeight(2.)
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
	painter->setSpread(ot::ReflectSpread);

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
	painter->setSpread(ot::ReflectSpread);

	painter->addStop(0.00, ot::Color(255, 255, 255));
	painter->addStop(0.75, ot::Color(255, 255, 255));
	painter->addStop(0.80, ot::Color(230, 230, 230));
	painter->addStop(0.85, ot::Color(25, 25, 25));
	painter->addStop(0.90, ot::Color(255, 255, 255));

	this->setTitleForegroundPainter(painter);
}

// ###########################################################################################################################################################################################################################################################################################################################

// Private: Helper

ot::GraphicsItemCfg* ot::GraphicsHierarchicalProjectItemBuilder::createConnectorItem(ot::Alignment _alignment) const {
	GraphicsEllipseItemCfg* con = new GraphicsEllipseItemCfg(0., 0., new StyleRefPainter2D(ColorStyleValueEntry::Transparent));
	con->setOutline(PenFCfg(1., new StyleRefPainter2D(ColorStyleValueEntry::Transparent)));
	con->setGraphicsItemFlags(GraphicsItemCfg::ItemIsConnectable | GraphicsItemCfg::ItemForwardsTooltip | GraphicsItemCfg::ItemHandlesState);
	con->setSizePolicy(ot::Preferred);

	constexpr double trigDist = GraphicsItemCfg::defaultAdditionalTriggerDistance();

	// Set name
	switch (_alignment) {
	case ot::AlignTopLeft:
		con->setName("TopLeft");
		con->setRadiusX(m_connectorWidth);
		con->setRadiusY(m_connectorWidth);
		con->setConnectionDirection(ot::ConnectUp);
		con->setAdditionalTriggerDistance(trigDist, trigDist, 0., trigDist);
		break;

	case ot::AlignTop:
		con->setName("Top");
		con->setRadiusX(m_connectorWidth);
		con->setRadiusY(m_connectorHeight);
		con->setConnectionDirection(ot::ConnectUp);
		con->setAdditionalTriggerDistance(trigDist, trigDist, 0., trigDist);
		break;

	case ot::AlignTopRight:
		con->setName("TopRight");
		con->setRadiusX(m_connectorWidth);
		con->setRadiusY(m_connectorWidth);
		con->setConnectionDirection(ot::ConnectUp);
		con->setAdditionalTriggerDistance(trigDist, trigDist, 0., trigDist);
		break;

	case ot::AlignLeft:
		con->setName("Left");
		con->setRadiusX(m_connectorHeight);
		con->setRadiusY(m_connectorWidth);
		con->setConnectionDirection(ot::ConnectLeft);
		con->setAdditionalTriggerDistance(trigDist, trigDist, trigDist, 0.);
		break;

	case ot::AlignRight:
		con->setName("Right");
		con->setRadiusX(m_connectorHeight);
		con->setRadiusY(m_connectorWidth);
		con->setConnectionDirection(ot::ConnectRight);
		con->setAdditionalTriggerDistance(0., trigDist, trigDist, trigDist);
		break;

	case ot::AlignBottomLeft:
		con->setName("BottomLeft");
		con->setRadiusX(m_connectorWidth);
		con->setRadiusY(m_connectorWidth);
		con->setConnectionDirection(ot::ConnectDown);
		con->setAdditionalTriggerDistance(trigDist, 0., trigDist, trigDist);
		break;

	case ot::AlignBottom:
		con->setName("Bottom");
		con->setRadiusX(m_connectorWidth);
		con->setRadiusY(m_connectorHeight);
		con->setConnectionDirection(ot::ConnectDown);
		con->setAdditionalTriggerDistance(trigDist, 0., trigDist, trigDist);
		break;

	case ot::AlignBottomRight:
		con->setName("BottomRight");
		con->setRadiusX(m_connectorWidth);
		con->setRadiusY(m_connectorWidth);
		con->setConnectionDirection(ot::ConnectDown);
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
	tBor->setSizePolicy(Dynamic);
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
		titLImg->setAlignment(AlignCenter);
		titLImg->setMargins(MarginsD(15., 0., 0., 0.));
		titLImg->setGraphicsItemFlags(GraphicsItemCfg::ItemForwardsTooltip);
		tLay->addChildItem(titLImg);

		MarginsD titMargins = tit->getMargins();
		titMargins.setLeft(15.);
		tit->setMargins(titMargins);
	}

	tLay->addChildItem(tit);
	tLay->addStrech(1);

	return tStack;
}