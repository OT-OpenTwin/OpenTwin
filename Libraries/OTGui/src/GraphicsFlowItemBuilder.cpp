//! @file GraphicsFlowItemBuilder.cpp
//! 
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/Logger.h"

#include "OTGui/GraphicsFlowItemBuilder.h"
#include "OTGui/GraphicsEllipseItemCfg.h"
#include "OTGui/GraphicsImageItemCfg.h"
#include "OTGui/GraphicsRectangularItemCfg.h"
#include "OTGui/GraphicsStackItemCfg.h"
#include "OTGui/GraphicsTextItemCfg.h"
#include "OTGui/GraphicsTriangleItemCfg.h"

#include "OTGui/GraphicsLayoutItemCfg.h"
#include "OTGui/GraphicsVBoxLayoutItemCfg.h"
#include "OTGui/GraphicsHBoxLayoutItemCfg.h"
#include "OTGui/GraphicsGridLayoutItemCfg.h"

#include "OTGui/Painter2D.h"
#include "OTGui/FillPainter2D.h"
#include "OTGui/Painter2DFactory.h"
#include "OTGui/LinearGradientPainter2D.h"
#include "OTGui/RadialGradientPainter2D.h"

ot::GraphicsFlowItemConnector::GraphicsFlowItemConnector() : m_figure(ot::GraphicsFlowItemConnector::Square) {}

ot::GraphicsFlowItemConnector::GraphicsFlowItemConnector(const GraphicsFlowItemConnector& _other) {
	*this = _other;
}

ot::GraphicsFlowItemConnector::~GraphicsFlowItemConnector() {}

ot::GraphicsFlowItemConnector& ot::GraphicsFlowItemConnector::operator = (const GraphicsFlowItemConnector& _other) {
	if (this != &_other) {
		m_name = _other.m_name;
		m_text = _other.m_text;
		m_figure = _other.m_figure;
		m_textColor = _other.m_textColor;
		m_font = _other.m_font;
		m_primaryColor = _other.m_primaryColor;
		m_secondaryColor = _other.m_secondaryColor;
	}
	return *this;
}

void ot::GraphicsFlowItemConnector::addToGrid(int _row, GraphicsGridLayoutItemCfg* _gridLayout, bool _isLeft) {
	// Connector item
	ot::GraphicsItemCfg* itm = this->createConnectorItem();
	itm->setGraphicsItemFlags(ot::GraphicsItemCfg::ItemIsConnectable);
	itm->setName(m_name);
	itm->setMargins(ot::MarginsD(0., 0., 0., 0));
	itm->setToolTip(m_toolTip);

	// Title item
	ot::GraphicsTextItemCfg* itmTxt = new ot::GraphicsTextItemCfg;
	itmTxt->setName(m_name + "_ctit");
	itmTxt->setText(m_text);
	itmTxt->setTextColor(m_textColor);
	itmTxt->setTextFont(m_font);
	itmTxt->setToolTip(m_toolTip);
	//itmTxt->setMargins(ot::MarginsD(2., 2., 2., 2.));

	// Place into layout
	if (_isLeft) {
		itm->setAlignment(ot::AlignLeft);
		itm->setConnectionDirection(ot::ConnectLeft);
		itmTxt->setAlignment(ot::AlignLeft);

		_gridLayout->addChildItem(_row, 0, itm);
		_gridLayout->addChildItem(_row, 1, itmTxt);
		
	}
	else {
		itm->setAlignment(ot::AlignRight);
		itm->setConnectionDirection(ot::ConnectRight);
		itmTxt->setAlignment(ot::AlignRight);

		_gridLayout->addChildItem(_row, 1, itm);
		_gridLayout->addChildItem(_row, 0, itmTxt);
	}
	
}

ot::GraphicsItemCfg* ot::GraphicsFlowItemConnector::createConnectorItem(void) {
	switch (m_figure)
	{
	case ot::GraphicsFlowItemConnector::Square: return this->createSquareItem();
	case ot::GraphicsFlowItemConnector::Circle: return this->createCircleItem();
	case ot::GraphicsFlowItemConnector::TriangleLeft: return this->createTriangleItem(GraphicsTriangleItemCfg::Left, GraphicsTriangleItemCfg::Triangle);
	case ot::GraphicsFlowItemConnector::TriangleRight: return this->createTriangleItem(GraphicsTriangleItemCfg::Right, GraphicsTriangleItemCfg::Triangle);
	case ot::GraphicsFlowItemConnector::TriangleUp: return this->createTriangleItem(GraphicsTriangleItemCfg::Up, GraphicsTriangleItemCfg::Triangle);
	case ot::GraphicsFlowItemConnector::TriangleDown: return this->createTriangleItem(GraphicsTriangleItemCfg::Down, GraphicsTriangleItemCfg::Triangle);
	case ot::GraphicsFlowItemConnector::KiteLeft: return this->createTriangleItem(GraphicsTriangleItemCfg::Left, GraphicsTriangleItemCfg::Kite);
	case ot::GraphicsFlowItemConnector::KiteRight: return this->createTriangleItem(GraphicsTriangleItemCfg::Right, GraphicsTriangleItemCfg::Kite);
	case ot::GraphicsFlowItemConnector::KiteUp: return this->createTriangleItem(GraphicsTriangleItemCfg::Up, GraphicsTriangleItemCfg::Kite);
	case ot::GraphicsFlowItemConnector::KiteDown: return this->createTriangleItem(GraphicsTriangleItemCfg::Down, GraphicsTriangleItemCfg::Kite);
	case ot::GraphicsFlowItemConnector::IceConeLeft: return this->createTriangleItem(GraphicsTriangleItemCfg::Left, GraphicsTriangleItemCfg::IceCone);
	case ot::GraphicsFlowItemConnector::IceConeRight: return this->createTriangleItem(GraphicsTriangleItemCfg::Right, GraphicsTriangleItemCfg::IceCone);
	case ot::GraphicsFlowItemConnector::IceConeUp: return this->createTriangleItem(GraphicsTriangleItemCfg::Up, GraphicsTriangleItemCfg::IceCone);
	case ot::GraphicsFlowItemConnector::IceConeDown: return this->createTriangleItem(GraphicsTriangleItemCfg::Down, GraphicsTriangleItemCfg::IceCone);
	default:
		OT_LOG_EA("Unknown connector type");
		throw std::exception("Unknown connector type");
	}
}

ot::GraphicsItemCfg* ot::GraphicsFlowItemConnector::createSquareItem(void) {
	ot::GraphicsRectangularItemCfg* itm = new ot::GraphicsRectangularItemCfg(new ot::FillPainter2D(m_primaryColor));
	itm->setSize(ot::Size2DD(10., 10.));
	itm->setMaximumSize(ot::Size2DD(10., 10.));
	itm->setMinimumSize(ot::Size2DD(10., 10.));
	itm->setOutline(ot::OutlineF(1., m_secondaryColor));

	return itm;
}

ot::GraphicsItemCfg* ot::GraphicsFlowItemConnector::createCircleItem(void) {
	ot::GraphicsEllipseItemCfg* itm = new ot::GraphicsEllipseItemCfg(5, 5, new ot::FillPainter2D(m_primaryColor));
	itm->setMaximumSize(ot::Size2DD(10., 10.));
	itm->setMinimumSize(ot::Size2DD(10., 10.));
	itm->setOutline(ot::OutlineF(1., m_secondaryColor));

	return itm;
}

ot::GraphicsItemCfg* ot::GraphicsFlowItemConnector::createTriangleItem(GraphicsTriangleItemCfg::TriangleDirection _direction, GraphicsTriangleItemCfg::TriangleShape _shape) {
	ot::GraphicsTriangleItemCfg* itm = new ot::GraphicsTriangleItemCfg(_direction, _shape);
	itm->setBackgroundPainer(new ot::FillPainter2D(m_primaryColor));
	itm->setMaximumSize(ot::Size2DD(10., 10.));
	itm->setMinimumSize(ot::Size2DD(10., 10.));
	itm->setOutline(ot::OutlineF(1., m_secondaryColor));

	return itm;
}

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

ot::GraphicsItemCfg* ot::GraphicsFlowItemBuilder::createGraphicsItem() const {
	OTAssertNullptr(m_backgroundPainter);
	OTAssertNullptr(m_titleBackgroundPainter);
	OTAssertNullptr(m_titleForegroundPainter);
	OTAssert(!m_name.empty(), "No name provided");

	// --- Create a copy of local data ------------------------------------------------

	// Create a copy of the painter
	ot::Painter2D* painterBack = nullptr;
	ot::Painter2D* painterTitleBack = nullptr;
	ot::Painter2D* painterTitleFront = nullptr;

	try {
		JsonDocument backDoc;
		m_backgroundPainter->addToJsonObject(backDoc, backDoc.GetAllocator());
		painterBack = Painter2DFactory::instance().create(backDoc.GetConstObject());

		JsonDocument titleBackDoc;
		m_titleBackgroundPainter->addToJsonObject(titleBackDoc, titleBackDoc.GetAllocator());
		painterTitleBack = Painter2DFactory::instance().create(titleBackDoc.GetConstObject());
		
		JsonDocument titleFrontDoc;
		m_titleForegroundPainter->addToJsonObject(titleFrontDoc, titleFrontDoc.GetAllocator());
		painterTitleFront = Painter2DFactory::instance().create(titleFrontDoc.GetConstObject());
	}
	catch (const std::exception& _e) {
		OT_LOG_EAS(_e.what());
		if (painterBack) delete painterBack;
		if (painterTitleBack) delete painterTitleBack;
		return nullptr;
	}
	catch (...) {
		OT_LOG_EA("[FATAL] Unknown error");
		if (painterBack) delete painterBack;
		if (painterTitleBack) delete painterTitleBack;
		if (painterTitleFront) delete painterTitleFront;
		return nullptr;
	}

	OTAssertNullptr(painterBack);
	OTAssertNullptr(painterTitleBack);
	OTAssertNullptr(painterTitleFront);

	// --- Create items ------------------------------------------------

	// Root
	ot::GraphicsStackItemCfg* root = new ot::GraphicsStackItemCfg;
	root->setName(m_name);
	root->setTitle(m_title);
	root->setToolTip(m_toolTip);
	root->setGraphicsItemFlags(ot::GraphicsItemCfg::ItemIsMoveable);

	// Border
	ot::GraphicsRectangularItemCfg* bor = new ot::GraphicsRectangularItemCfg(painterBack);
	bor->setOutline(ot::OutlineF(1., ot::Color(0, 0, 0)));
	bor->setCornerRadius(5);
	bor->setName(m_name + "_bor");
	bor->setSizePolicy(ot::Dynamic);
	bor->setGraphicsItemFlags(GraphicsItemCfg::ItemForwardsTooltip);
	root->addItemTop(bor, false, true);

	// Layout
	ot::GraphicsVBoxLayoutItemCfg* mLay = new ot::GraphicsVBoxLayoutItemCfg;
	mLay->setName(m_name + "_mLay");
	mLay->setMinimumSize(ot::Size2DD(50., 80.));
	mLay->setGraphicsItemFlags(GraphicsItemCfg::ItemForwardsTooltip);
	root->addItemTop(mLay, true, false);

	// Title: Stack
	ot::GraphicsStackItemCfg* tStack = new ot::GraphicsStackItemCfg;
	tStack->setName(m_name + "_tStack");
	tStack->setGraphicsItemFlags(GraphicsItemCfg::ItemForwardsTooltip);
	mLay->addChildItem(tStack, 0);

	// Title: Border
	ot::GraphicsRectangularItemCfg* tBor = new ot::GraphicsRectangularItemCfg(painterTitleBack);
	tBor->setOutline(OutlineF(1., ot::Color(0, 0, 0)));
	tBor->setName(m_name + "_tBor");
	tBor->setCornerRadius(5);
	//tBor->setSize(ot::Size2DD(200., 30.));
	tBor->setSizePolicy(ot::Dynamic);
	tBor->setGraphicsItemFlags(GraphicsItemCfg::ItemForwardsTooltip);
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
	tit->setMargins(ot::MarginsD(2., 2., 2., 2.));
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
		titLImg->setMaximumSize(ot::Size2DD(22., 22.));
		titLImg->setAlignment(ot::AlignCenter);
		titLImg->setMargins(ot::MarginsD(15., 2., 0., 2.));
		titLImg->setGraphicsItemFlags(GraphicsItemCfg::ItemForwardsTooltip);
		tLay->addChildItem(titLImg);
	}
	
	tLay->addChildItem(tit);
	
	// Title: Right Corner
	if (m_rightTitleImagePath.empty()) {
		tLay->addStrech(1);
	}
	else {
		ot::GraphicsImageItemCfg* titRImg = new ot::GraphicsImageItemCfg;
		titRImg->setName(m_name + "_titRImg");
		titRImg->setImagePath(m_rightTitleImagePath);
		titRImg->setMaximumSize(ot::Size2DD(22., 22.));
		titRImg->setAlignment(ot::AlignCenter);
		titRImg->setMargins(ot::MarginsD(0., 2., 15., 2.));
		titRImg->setGraphicsItemFlags(GraphicsItemCfg::ItemForwardsTooltip);
		tLay->addChildItem(titRImg);
	}

	// Central layout
	ot::GraphicsHBoxLayoutItemCfg* cLay = new ot::GraphicsHBoxLayoutItemCfg;
	cLay->setName(m_name + "_cLay");
	cLay->setGraphicsItemFlags(GraphicsItemCfg::ItemForwardsTooltip);
	
	// Left connector layout
	ot::GraphicsGridLayoutItemCfg* lcLay = new ot::GraphicsGridLayoutItemCfg((int)m_left.size() + 1, 2);
	lcLay->setName(m_name + "_lcLay");
	lcLay->setRowStretch((int)m_left.size(), 1);
	lcLay->setColumnStretch(1, 1);
	lcLay->setGraphicsItemFlags(GraphicsItemCfg::ItemForwardsTooltip);

	int ix = 0;
	for (auto c : m_left) {
		c.addToGrid(ix, lcLay, true);
		ix++;
	}

	// Right connector layout
	ot::GraphicsGridLayoutItemCfg* rcLay = new ot::GraphicsGridLayoutItemCfg((int)m_right.size() + 1, 2);
	rcLay->setName(m_name + "_rcLay");
	rcLay->setRowStretch((int)m_right.size(), 1);
	rcLay->setColumnStretch(0, 1);
	rcLay->setGraphicsItemFlags(GraphicsItemCfg::ItemForwardsTooltip);

	ix = 0;
	for (auto c : m_right) {
		c.addToGrid(ix, rcLay, false);
		ix++;
	}

	// Create background image
	ot::GraphicsImageItemCfg* cImg = nullptr;
	if (!m_backgroundImagePath.empty()) {
		cImg = new ot::GraphicsImageItemCfg;
		cImg->setImagePath(m_backgroundImagePath);
		cImg->setName(m_name + "_cImg");
		cImg->setMargins(m_backgroundImageMargins);
		cImg->setSizePolicy(ot::Dynamic);
		cImg->setAlignment(m_backgroundImageAlignment);
		cImg->setMaintainAspectRatio(m_backgroundImageMaintainAspectRatio);
		cImg->setGraphicsItemFlags(GraphicsItemCfg::ItemForwardsTooltip);
	}

	// Create central stack (if background image is set and the image insert mode is "onStack")
	ot::GraphicsStackItemCfg* cStack = nullptr;
	if (cImg && m_backgroundImageInsertMode == OnStack) {
		cStack = new ot::GraphicsStackItemCfg;
		cStack->setName(m_name + "_cStack");
		cStack->setGraphicsItemFlags(GraphicsItemCfg::ItemForwardsTooltip);

		cStack->addItemBottom(cImg, false, true);
		cStack->addItemTop(cLay, true, false);
	}

	// Setup central layout (left connector layout)
	cLay->addChildItem(lcLay);

	// Setup central layout (central image (if on layout mode is set))
	if (cImg && m_backgroundImageInsertMode == OnLayout) {
		cLay->addChildItem(cImg, 1);
	}
	else {
		// Setup central layout (stretch)
		cLay->addStrech(1);
	}

	// Setup central layout (right connector layout)
	cLay->addChildItem(rcLay);

	// Add central layout or stack to main layout
	if (cStack) {
		mLay->addChildItem(cStack, 1);
	}
	else {
		mLay->addChildItem(cLay, 1);
	}
	
	return root;
}

ot::GraphicsFlowItemBuilder::GraphicsFlowItemBuilder()
	: m_backgroundPainter(nullptr), m_titleBackgroundPainter(nullptr), m_titleForegroundPainter(nullptr),
	m_backgroundImageAlignment(ot::AlignCenter), m_backgroundImageMargins(5., 2., 2., 2.), m_backgroundImageInsertMode(OnLayout),
	m_backgroundImageMaintainAspectRatio(true)
{
	this->setBackgroundColor(ot::Color(50, 50, 50));
	this->setTitleBackgroundColor(ot::Color(70, 70, 70));
	this->setDefaultTitleForegroundGradient();

	m_defaultConnectorStyle.setTextColor(ot::Color(White));
	m_defaultConnectorStyle.setPrimaryColor(ot::Color(ot::White));
	m_defaultConnectorStyle.setSecondaryColor(ot::Color(ot::Black));
}

ot::GraphicsFlowItemBuilder::~GraphicsFlowItemBuilder() {}

void ot::GraphicsFlowItemBuilder::addLeft(const std::string& _name, const std::string& _title, GraphicsFlowItemConnector::ConnectorFigure _figure) {
	GraphicsFlowItemConnector cfg = m_defaultConnectorStyle;
	cfg.setName(_name);
	cfg.setText(_title);
	cfg.setFigure(_figure);

	this->addLeft(cfg);
}

void ot::GraphicsFlowItemBuilder::addLeft(const std::string& _name, const std::string& _title, GraphicsFlowItemConnector::ConnectorFigure _figure, ot::DefaultColor _color) {
	GraphicsFlowItemConnector cfg = m_defaultConnectorStyle;
	cfg.setName(_name);
	cfg.setText(_title);
	cfg.setFigure(_figure);
	cfg.setPrimaryColor(ot::Color(_color));

	this->addLeft(cfg);
}

void ot::GraphicsFlowItemBuilder::addLeft(const std::string& _name, const std::string& _title, GraphicsFlowItemConnector::ConnectorFigure _figure, const ot::Color& _color) {
	GraphicsFlowItemConnector cfg = m_defaultConnectorStyle;
	cfg.setName(_name);
	cfg.setText(_title);
	cfg.setFigure(_figure);
	cfg.setPrimaryColor(_color);

	this->addLeft(cfg);
}

void ot::GraphicsFlowItemBuilder::addLeft(const GraphicsFlowItemConnector& _input) {
	m_left.push_back(_input);
}

void ot::GraphicsFlowItemBuilder::addRight(const std::string& _name, const std::string& _title, GraphicsFlowItemConnector::ConnectorFigure _figure) {
	GraphicsFlowItemConnector cfg = m_defaultConnectorStyle;
	cfg.setName(_name);
	cfg.setText(_title);
	cfg.setFigure(_figure);

	this->addRight(cfg);
}

void ot::GraphicsFlowItemBuilder::addRight(const std::string& _name, const std::string& _title, GraphicsFlowItemConnector::ConnectorFigure _figure, ot::DefaultColor _color) {
	GraphicsFlowItemConnector cfg = m_defaultConnectorStyle;
	cfg.setName(_name);
	cfg.setText(_title);
	cfg.setFigure(_figure);
	cfg.setPrimaryColor(ot::Color(_color));

	this->addRight(cfg);
}

void ot::GraphicsFlowItemBuilder::addRight(const std::string& _name, const std::string& _title, GraphicsFlowItemConnector::ConnectorFigure _figure, const ot::Color& _color) {
	GraphicsFlowItemConnector cfg = m_defaultConnectorStyle;
	cfg.setName(_name);
	cfg.setText(_title);
	cfg.setFigure(_figure);
	cfg.setPrimaryColor(_color);

	this->addRight(cfg);
}

void ot::GraphicsFlowItemBuilder::addRight(const GraphicsFlowItemConnector& _output) {
	m_right.push_back(_output);
}

void ot::GraphicsFlowItemBuilder::setBackgroundPainter(ot::Painter2D* _painter) {
	if (m_backgroundPainter == _painter) return;
	if (m_backgroundPainter) delete m_backgroundPainter;
	m_backgroundPainter = _painter;
}

void ot::GraphicsFlowItemBuilder::setBackgroundColor(const ot::Color& _color) {
	this->setBackgroundPainter(new ot::FillPainter2D(_color));
}

void ot::GraphicsFlowItemBuilder::setTitleBackgroundPainter(ot::Painter2D* _painter) {
	if (m_titleBackgroundPainter == _painter) return;
	if (m_titleBackgroundPainter) delete m_titleBackgroundPainter;
	m_titleBackgroundPainter = _painter;
}

void ot::GraphicsFlowItemBuilder::setTitleBackgroundGradientColor(const ot::Color& _color) {
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

void ot::GraphicsFlowItemBuilder::setTitleBackgroundColor(const ot::Color& _color) {
	this->setTitleBackgroundPainter(new ot::FillPainter2D(_color));
}

void ot::GraphicsFlowItemBuilder::setTitleForegroundPainter(ot::Painter2D* _painter) {
	if (m_titleForegroundPainter == _painter) return;
	if (m_titleForegroundPainter) delete m_titleForegroundPainter;
	m_titleForegroundPainter = _painter;
}

void ot::GraphicsFlowItemBuilder::setTitleForegroundColor(const ot::Color& _color) {
	this->setTitleForegroundPainter(new ot::FillPainter2D(_color));
}

void ot::GraphicsFlowItemBuilder::setDefaultTitleForegroundGradient(void) {
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