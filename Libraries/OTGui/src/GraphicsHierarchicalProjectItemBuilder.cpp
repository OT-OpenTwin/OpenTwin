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
	OTAssertNullptr(m_titleBackgroundPainter);
	OTAssertNullptr(m_titleForegroundPainter);
	OTAssert(!m_name.empty(), "No name provided");

	// --- Create a copy of local data ------------------------------------------------

	// Create a copy of the painter
	ot::Painter2D* painterTitleBack = m_titleBackgroundPainter->createCopy();
	ot::Painter2D* painterTitleFront = m_titleForegroundPainter->createCopy();

	OTAssertNullptr(painterTitleBack);
	OTAssertNullptr(painterTitleFront);

	// --- Create items ------------------------------------------------

	// Root
	ot::GraphicsStackItemCfg* root = new ot::GraphicsStackItemCfg;
	root->setName(m_name);
	root->setTitle(m_title);
	root->setToolTip(m_toolTip);
	root->setGraphicsItemFlags(GraphicsItemCfg::ItemIsMoveable | GraphicsItemCfg::ItemSnapsToGridTopLeft | GraphicsItemCfg::ItemForwardsState | GraphicsItemCfg::ItemIsSelectable);

	// Border
	ot::GraphicsRectangularItemCfg* bor = new ot::GraphicsRectangularItemCfg(new StyleRefPainter2D(ColorStyleValueEntry::GraphicsItemBackground));
	bor->setOutline(ot::PenFCfg(1., new StyleRefPainter2D(ColorStyleValueEntry::GraphicsItemBorder)));
	bor->setCornerRadius(5);
	bor->setName(m_name + "_bor");
	bor->setSizePolicy(ot::Dynamic);
	bor->setGraphicsItemFlags(GraphicsItemCfg::ItemForwardsTooltip | GraphicsItemCfg::ItemHandlesState);
	root->addItemTop(bor, false, true);

	// Main layout
	ot::GraphicsVBoxLayoutItemCfg* mLay = new ot::GraphicsVBoxLayoutItemCfg;
	mLay->setName(m_name + "_mLay");
	mLay->setMinimumSize(ot::Size2DD(50., 80.));
	mLay->setGraphicsItemFlags(GraphicsItemCfg::ItemForwardsTooltip | GraphicsItemCfg::ItemForwardsState);
	root->addItemTop(mLay, true, false);

	// Title: Stack
	ot::GraphicsStackItemCfg* tStack = new ot::GraphicsStackItemCfg;
	tStack->setName(m_name + "_tStack");
	tStack->setGraphicsItemFlags(GraphicsItemCfg::ItemForwardsTooltip | GraphicsItemCfg::ItemForwardsState);
	mLay->addChildItem(tStack, 0);

	// Title: Border
	ot::GraphicsRectangularItemCfg* tBor = new ot::GraphicsRectangularItemCfg(painterTitleBack);
	tBor->setOutline(PenFCfg(1., new StyleRefPainter2D(ColorStyleValueEntry::GraphicsItemBorder)));
	tBor->setName(m_name + "_tBor");
	tBor->setCornerRadius(5);
	//tBor->setSize(ot::Size2DD(200., 30.));
	tBor->setSizePolicy(ot::Dynamic);
	tBor->setGraphicsItemFlags(GraphicsItemCfg::ItemForwardsTooltip | GraphicsItemCfg::ItemHandlesState);
	tStack->addItemTop(tBor, false, true);

	// Title: Layout
	ot::GraphicsHBoxLayoutItemCfg* tLay = new ot::GraphicsHBoxLayoutItemCfg;
	tLay->setName(m_name + "_tLay");
	tLay->setGraphicsItemFlags(GraphicsItemCfg::ItemForwardsTooltip);
	tStack->addItemTop(tLay, true, false);

	// Title: Title
	ot::GraphicsTextItemCfg* tit = new ot::GraphicsTextItemCfg;
	tit->setName(m_name + "_tit");
	tit->setText(m_title);
	tit->setTextPainter(painterTitleFront);
	tit->setMinimumSize(ot::Size2DD(10., 22.));
	tit->setGraphicsItemFlags(GraphicsItemCfg::ItemForwardsTooltip);

	// Title: Left Corner
	if (m_leftTitleImagePath.empty()) {
		tLay->addStrech(1);
	}
	else {
		ot::GraphicsImageItemCfg* titLImg = new ot::GraphicsImageItemCfg;
		titLImg->setName(m_name + "_titLImg");
		titLImg->setImagePath(m_leftTitleImagePath);
		titLImg->setFixedSize(16., 16.);
		titLImg->setAlignment(ot::AlignCenter);
		titLImg->setMargins(ot::MarginsD(15., 0., 0., 0.));
		titLImg->setGraphicsItemFlags(GraphicsItemCfg::ItemForwardsTooltip);
		tLay->addChildItem(titLImg);

		MarginsD titMargins = tit->getMargins();
		titMargins.setLeft(15.);
		tit->setMargins(titMargins);
	}

	tLay->addChildItem(tit);
	tLay->addStrech(1);

	// Central layout
	ot::GraphicsVBoxLayoutItemCfg* cLay = new ot::GraphicsVBoxLayoutItemCfg;
	cLay->setName(m_name + "_cLay");
	cLay->setGraphicsItemFlags(GraphicsItemCfg::ItemForwardsTooltip);

	if (!m_type.empty()) {
		ot::GraphicsTextItemCfg* typeItm = new GraphicsTextItemCfg("Project Type: " + m_type);
		typeItm->setName(m_name + "_ptype");
		typeItm->setAlignment(ot::AlignLeft);
		
		cLay->addChildItem(typeItm);
	}
	

	// Create background image
	if (!m_backgroundImage.empty()) {
		/*ot::GraphicsImageItemCfg* cImg = new ot::GraphicsImageItemCfg;
		//cImg->setImagePath(m_backgroundImagePath);
		cImg->setName(m_name + "_cImg");
		cImg->setMargins(m_backgroundImageMargins);
		cImg->setSizePolicy(ot::Dynamic);
		cImg->setAlignment(ot::AlignCenter);
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
	m_backgroundImageMargins(5., 2., 2., 2.)
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