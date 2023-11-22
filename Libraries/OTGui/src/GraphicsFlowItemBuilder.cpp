//! @file GraphicsFlowItemBuilder.cpp
//! 
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
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
#include "OTGui/LinearGradientPainter2D.h"
#include "OTGui/RadialGradientPainter2D.h"

namespace ot {
	namespace intern {

		// Column description
		enum FlowLayoutColumns {
			flcLeftConnector,
			flcLeftTitle,
			flcSpacer,
			flcRightTitle,
			flcRightConnector,
			flcCount // Number of columns (keep this at the end of the enum)
		};
	}
}

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

	// Title item
	ot::GraphicsTextItemCfg* itmTxt = new ot::GraphicsTextItemCfg;
	itmTxt->setName(m_name + "_ctit");
	itmTxt->setText(m_text);
	itmTxt->setTextColor(m_textColor);
	itmTxt->setTextFont(m_font);
	//itmTxt->setMargins(ot::MarginsD(2., 2., 2., 2.));

	// Place into layout
	if (_isLeft) {
		itm->setAlignment(ot::AlignLeft);
		itm->setConnectionDirection(ot::ConnectLeft);
		itmTxt->setAlignment(ot::AlignLeft);

		_gridLayout->addChildItem(_row, ot::intern::flcLeftConnector, itm);
		_gridLayout->addChildItem(_row, ot::intern::flcLeftTitle, itmTxt);
		
	}
	else {
		itm->setAlignment(ot::AlignRight);
		itm->setConnectionDirection(ot::ConnectRight);
		itmTxt->setAlignment(ot::AlignRight);

		_gridLayout->addChildItem(_row, ot::intern::flcRightConnector, itm);
		_gridLayout->addChildItem(_row, ot::intern::flcRightTitle, itmTxt);
	}
	
}

ot::GraphicsItemCfg* ot::GraphicsFlowItemConnector::createConnectorItem(void) {
	switch (m_figure)
	{
	case ot::GraphicsFlowItemConnector::Square: return this->createSquareItem();
	case ot::GraphicsFlowItemConnector::Circle: return this->createCircleItem();
	case ot::GraphicsFlowItemConnector::TriangleLeft: return this->createLeftTriangleItem();
	case ot::GraphicsFlowItemConnector::TriangleRight: return this->createRightTriangleItem();
	case ot::GraphicsFlowItemConnector::TriangleUp: return this->createUpTriangleItem();
	case ot::GraphicsFlowItemConnector::TriangleDown: return this->createDownTriangleItem();
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
	itm->setBorder(ot::Border(m_secondaryColor, 1));

	return itm;
}

ot::GraphicsItemCfg* ot::GraphicsFlowItemConnector::createCircleItem(void) {
	ot::GraphicsEllipseItemCfg* itm = new ot::GraphicsEllipseItemCfg(5, 5, new ot::FillPainter2D(m_primaryColor));
	itm->setMaximumSize(ot::Size2DD(10., 10.));
	itm->setMinimumSize(ot::Size2DD(10., 10.));
	itm->setBorder(ot::Border(m_secondaryColor, 1));

	return itm;
}

ot::GraphicsItemCfg* ot::GraphicsFlowItemConnector::createLeftTriangleItem(void) {
	ot::GraphicsTriangleItemCfg* itm = new ot::GraphicsTriangleItemCfg(ot::GraphicsTriangleItemCfg::Left);
	itm->setBackgroundPainer(new ot::FillPainter2D(m_primaryColor));
	itm->setMaximumSize(ot::Size2DD(10., 10.));
	itm->setMinimumSize(ot::Size2DD(10., 10.));
	itm->setOutline(ot::Border(m_secondaryColor, 1));

	return itm;
}

ot::GraphicsItemCfg* ot::GraphicsFlowItemConnector::createRightTriangleItem(void) {
	ot::GraphicsTriangleItemCfg* itm = new ot::GraphicsTriangleItemCfg(ot::GraphicsTriangleItemCfg::Right);
	itm->setBackgroundPainer(new ot::FillPainter2D(m_primaryColor));
	itm->setMaximumSize(ot::Size2DD(10., 10.));
	itm->setMinimumSize(ot::Size2DD(10., 10.));
	itm->setOutline(ot::Border(m_secondaryColor, 1));

	return itm;
}

ot::GraphicsItemCfg* ot::GraphicsFlowItemConnector::createUpTriangleItem(void) {
	ot::GraphicsTriangleItemCfg* itm = new ot::GraphicsTriangleItemCfg(ot::GraphicsTriangleItemCfg::Up);
	itm->setBackgroundPainer(new ot::FillPainter2D(m_primaryColor));
	itm->setMaximumSize(ot::Size2DD(10., 10.));
	itm->setMinimumSize(ot::Size2DD(10., 10.));
	itm->setOutline(ot::Border(m_secondaryColor, 1));

	return itm;
}

ot::GraphicsItemCfg* ot::GraphicsFlowItemConnector::createDownTriangleItem(void) {
	ot::GraphicsTriangleItemCfg* itm = new ot::GraphicsTriangleItemCfg(ot::GraphicsTriangleItemCfg::Down);
	itm->setBackgroundPainer(new ot::FillPainter2D(m_primaryColor));
	itm->setMaximumSize(ot::Size2DD(10., 10.));
	itm->setMinimumSize(ot::Size2DD(10., 10.));
	itm->setOutline(ot::Border(m_secondaryColor, 1));

	return itm;
}

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

ot::GraphicsItemCfg* ot::GraphicsFlowItemBuilder::createGraphicsItem(const std::string& _name, const std::string& _title) const {
	OTAssertNullptr(m_backgroundPainter);
	OTAssertNullptr(m_titleBackgroundPainter);
	OTAssertNullptr(m_titleForegroundPainter);
	OTAssert(!_name.empty(), "No name provided");

	// --- Create a copy of local data ------------------------------------------------

	// Create a copy of the painter
	ot::Painter2D* painterBack = nullptr;
	ot::Painter2D* painterTitleBack = nullptr;
	ot::Painter2D* painterTitleFront = nullptr;

	try {
		OT_rJSON_createDOC(backDoc);
		m_backgroundPainter->addToJsonObject(backDoc, backDoc);
		painterBack = ot::SimpleFactory::instance().createType<ot::Painter2D>(backDoc);
		painterBack->setFromJsonObject(backDoc);

		OT_rJSON_createDOC(titleBackDoc);
		m_titleBackgroundPainter->addToJsonObject(titleBackDoc, titleBackDoc);
		painterTitleBack = ot::SimpleFactory::instance().createType<ot::Painter2D>(titleBackDoc);
		painterTitleBack->setFromJsonObject(titleBackDoc);

		OT_rJSON_createDOC(titleFrontDoc);
		m_titleForegroundPainter->addToJsonObject(titleFrontDoc, titleFrontDoc);
		painterTitleFront = ot::SimpleFactory::instance().createType<ot::Painter2D>(titleFrontDoc);
		painterTitleFront->setFromJsonObject(titleFrontDoc);

		

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
	root->setName(_name);
	root->setTitle(_title);
	root->setGraphicsItemFlags(ot::GraphicsItemCfg::ItemIsMoveable);

	// Border
	ot::GraphicsRectangularItemCfg* bor = new ot::GraphicsRectangularItemCfg(painterBack);
	bor->setBorder(ot::Border(ot::Color(0, 0, 0), 1));
	bor->setCornerRadius(5);
	bor->setName(_name + "_bor");
	bor->setSizePolicy(ot::Dynamic);
	root->addItemTop(bor, false, true);

	// Layout
	ot::GraphicsVBoxLayoutItemCfg* mLay = new ot::GraphicsVBoxLayoutItemCfg;
	mLay->setName(_name + "_mLay");
	mLay->setMinimumSize(ot::Size2DD(50., 80.));
	root->addItemTop(mLay, true, false);

	// Title: Stack
	ot::GraphicsStackItemCfg* tStack = new ot::GraphicsStackItemCfg;
	tStack->setName(_name + "_tStack");
	mLay->addChildItem(tStack, 0);

	// Title: Border
	ot::GraphicsRectangularItemCfg* tBor = new ot::GraphicsRectangularItemCfg(painterTitleBack);
	tBor->setBorder(ot::Border(ot::Color(0, 0, 0), 1));
	tBor->setName(_name + "_tBor");
	tBor->setCornerRadius(5);
	//tBor->setSize(ot::Size2DD(200., 30.));
	tBor->setSizePolicy(ot::Dynamic);
	tStack->addItemTop(tBor, false, true);

	// Title: Layout
	ot::GraphicsHBoxLayoutItemCfg* tLay = new ot::GraphicsHBoxLayoutItemCfg;
	tLay->setName(_name + "_tLay");
	tStack->addItemTop(tLay, true, false);

	// Title: Title
	ot::GraphicsTextItemCfg* tit = new ot::GraphicsTextItemCfg;
	tit->setName(_name + "_tit");
	tit->setText(_title);
	tit->setTextPainter(painterTitleFront);
	tit->setMargins(ot::MarginsD(2., 2., 2., 2.));
	tit->setMinimumSize(ot::Size2DD(10., 22.));

	// Title: Left Corner
	if (m_leftTitleImagePath.empty()) {
		tLay->addStrech(1);
	}
	else {
		ot::GraphicsImageItemCfg* titLImg = new ot::GraphicsImageItemCfg;
		titLImg->setName(_name + "_titLImg");
		titLImg->setImagePath(m_leftTitleImagePath);
		titLImg->setMaximumSize(ot::Size2DD(22., 22.));
		titLImg->setAlignment(ot::AlignCenter);
		titLImg->setMargins(ot::MarginsD(2., 0., 2., 15.));
		tLay->addChildItem(titLImg);
	}
	
	tLay->addChildItem(tit);
	
	// Title: Right Corner
	if (m_rightTitleImagePath.empty()) {
		tLay->addStrech(1);
	}
	else {
		ot::GraphicsImageItemCfg* titRImg = new ot::GraphicsImageItemCfg;
		titRImg->setName(_name + "_titRImg");
		titRImg->setImagePath(m_rightTitleImagePath);
		titRImg->setMaximumSize(ot::Size2DD(22., 22.));
		titRImg->setAlignment(ot::AlignCenter);
		titRImg->setMargins(ot::MarginsD(2., 15., 2., 0.));
		tLay->addChildItem(titRImg);
	}

	// Central grid
	size_t lastRow = std::max(m_left.size(), m_right.size());
	ot::GraphicsGridLayoutItemCfg* cLay = new ot::GraphicsGridLayoutItemCfg((int)lastRow + 1, 5);
	cLay->setName(_name + "_cLay");

	int ix = 0;
	for (auto c : m_left) {
		c.addToGrid(ix, cLay, true);
		ix++;
	}
	ix = 0;
	for (auto c : m_right) {
		c.addToGrid(ix, cLay, false);
		ix++;
	}

	// Add stretch item at the bottom of the grid
	ot::GraphicsTextItemCfg* stretcher = new ot::GraphicsTextItemCfg(" ");
	stretcher->setName(_name + "_cStr");
	stretcher->setTextFont(ot::Font(ot::Consolas, 25));

	cLay->addChildItem((int)lastRow, 2, stretcher);
	cLay->setColumnStretch(2, 1);
	cLay->setRowStretch((int)lastRow, 1);

	// Central stack
	if (m_backgroundImagePath.empty()) {
		mLay->addChildItem(cLay, 1);
	}
	else {
		ot::GraphicsStackItemCfg* cStack = new ot::GraphicsStackItemCfg;
		cStack->setName(_name + "_cStack");

		ot::GraphicsImageItemCfg* cImg = new ot::GraphicsImageItemCfg;
		cImg->setImagePath(m_backgroundImagePath);
		cImg->setName(_name + "_cImg");
		cImg->setMargins(m_backgroundImageMargins);
		cImg->setSizePolicy(ot::Dynamic);
		cImg->setAlignment(m_backgroundImageAlignment);
		cImg->setMaintainAspectRatio(true);

		cStack->addItemTop(cImg, false, true);
		cStack->addItemTop(cLay, true, false);

		mLay->addChildItem(cStack, 1);
	}

	return root;
}

ot::GraphicsFlowItemBuilder::GraphicsFlowItemBuilder()
	: m_backgroundPainter(nullptr), m_titleBackgroundPainter(nullptr), m_titleForegroundPainter(nullptr),
	m_backgroundImageAlignment(ot::AlignCenter), m_backgroundImageMargins(2., 2., 5., 2.)
{
	this->setBackgroundColor(ot::Color(50, 50, 50));
	this->setTitleBackgroundColor(ot::Color(70, 70, 70));
	this->setDefaultTitleForegroundGradient();

	m_defaultConnectorStyle.setTextColor(ot::Color(ot::Color::White));
	m_defaultConnectorStyle.setPrimaryColor(ot::Color(ot::Color::White));
	m_defaultConnectorStyle.setSecondaryColor(ot::Color(ot::Color::Black));
}

ot::GraphicsFlowItemBuilder::~GraphicsFlowItemBuilder() {}

void ot::GraphicsFlowItemBuilder::addLeft(const std::string& _name, const std::string& _title, GraphicsFlowItemConnector::ConnectorFigure _figure) {
	GraphicsFlowItemConnector cfg = m_defaultConnectorStyle;
	cfg.setName(_name);
	cfg.setText(_title);
	cfg.setFigure(_figure);

	this->addLeft(cfg);
}

void ot::GraphicsFlowItemBuilder::addLeft(const std::string& _name, const std::string& _title, GraphicsFlowItemConnector::ConnectorFigure _figure, ot::Color::DefaultColor _color) {
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

void ot::GraphicsFlowItemBuilder::addRight(const std::string& _name, const std::string& _title, GraphicsFlowItemConnector::ConnectorFigure _figure, ot::Color::DefaultColor _color) {
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