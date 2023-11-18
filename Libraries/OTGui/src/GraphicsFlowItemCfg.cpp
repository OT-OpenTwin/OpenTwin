//! @file GraphicsFlowConnectorCfg.cpp
//! 
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTGui/GraphicsFlowItemCfg.h"
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

ot::GraphicsFlowConnectorCfg::GraphicsFlowConnectorCfg() : m_figure(ot::GraphicsFlowConnectorCfg::Square) {}

ot::GraphicsFlowConnectorCfg::GraphicsFlowConnectorCfg(const GraphicsFlowConnectorCfg& _other) {
	*this = _other;
}

ot::GraphicsFlowConnectorCfg::~GraphicsFlowConnectorCfg() {}

ot::GraphicsFlowConnectorCfg& ot::GraphicsFlowConnectorCfg::operator = (const GraphicsFlowConnectorCfg& _other) {
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

void ot::GraphicsFlowConnectorCfg::addToGrid(int _row, GraphicsGridLayoutItemCfg* _gridLayout, bool _isLeft) {
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

ot::GraphicsItemCfg* ot::GraphicsFlowConnectorCfg::createConnectorItem(void) {
	switch (m_figure)
	{
	case ot::GraphicsFlowConnectorCfg::Square: return this->createSquareItem();
	case ot::GraphicsFlowConnectorCfg::Circle: return this->createCircleItem();
	case ot::GraphicsFlowConnectorCfg::TriangleLeft: return this->createLeftTriangleItem();
	case ot::GraphicsFlowConnectorCfg::TriangleRight: return this->createRightTriangleItem();
	case ot::GraphicsFlowConnectorCfg::TriangleUp: return this->createUpTriangleItem();
	case ot::GraphicsFlowConnectorCfg::TriangleDown: return this->createDownTriangleItem();
	default:
		OT_LOG_EA("Unknown connector type");
		throw std::exception("Unknown connector type");
	}
}

ot::GraphicsItemCfg* ot::GraphicsFlowConnectorCfg::createSquareItem(void) {
	ot::GraphicsRectangularItemCfg* itm = new ot::GraphicsRectangularItemCfg(new ot::FillPainter2D(m_primaryColor));
	itm->setSize(ot::Size2DD(10., 10.));
	itm->setMaximumSize(ot::Size2DD(10., 10.));
	itm->setMinimumSize(ot::Size2DD(10., 10.));
	itm->setBorder(ot::Border(m_secondaryColor, 1));

	return itm;
}

ot::GraphicsItemCfg* ot::GraphicsFlowConnectorCfg::createCircleItem(void) {
	ot::GraphicsEllipseItemCfg* itm = new ot::GraphicsEllipseItemCfg(5, 5, new ot::FillPainter2D(m_primaryColor));
	itm->setMaximumSize(ot::Size2DD(10., 10.));
	itm->setMinimumSize(ot::Size2DD(10., 10.));
	itm->setBorder(ot::Border(m_secondaryColor, 1));

	return itm;
}

ot::GraphicsItemCfg* ot::GraphicsFlowConnectorCfg::createLeftTriangleItem(void) {
	ot::GraphicsTriangleItemCfg* itm = new ot::GraphicsTriangleItemCfg(ot::GraphicsTriangleItemCfg::Left);
	itm->setBackgroundPainer(new ot::FillPainter2D(m_primaryColor));
	itm->setMaximumSize(ot::Size2DD(10., 10.));
	itm->setMinimumSize(ot::Size2DD(10., 10.));
	itm->setOutline(ot::Border(m_secondaryColor, 1));

	return itm;
}

ot::GraphicsItemCfg* ot::GraphicsFlowConnectorCfg::createRightTriangleItem(void) {
	ot::GraphicsTriangleItemCfg* itm = new ot::GraphicsTriangleItemCfg(ot::GraphicsTriangleItemCfg::Right);
	itm->setBackgroundPainer(new ot::FillPainter2D(m_primaryColor));
	itm->setMaximumSize(ot::Size2DD(10., 10.));
	itm->setMinimumSize(ot::Size2DD(10., 10.));
	itm->setOutline(ot::Border(m_secondaryColor, 1));

	return itm;
}

ot::GraphicsItemCfg* ot::GraphicsFlowConnectorCfg::createUpTriangleItem(void) {
	ot::GraphicsTriangleItemCfg* itm = new ot::GraphicsTriangleItemCfg(ot::GraphicsTriangleItemCfg::Up);
	itm->setBackgroundPainer(new ot::FillPainter2D(m_primaryColor));
	itm->setMaximumSize(ot::Size2DD(10., 10.));
	itm->setMinimumSize(ot::Size2DD(10., 10.));
	itm->setOutline(ot::Border(m_secondaryColor, 1));

	return itm;
}

ot::GraphicsItemCfg* ot::GraphicsFlowConnectorCfg::createDownTriangleItem(void) {
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

ot::GraphicsItemCfg* ot::GraphicsFlowItemCfg::createGraphicsItem(const std::string& _name, const std::string& _title) const {
	OTAssertNullptr(m_backgroundPainter);
	OTAssertNullptr(m_titleBackgroundPainter);
	otAssert(!_name.empty(), "No name provided");

	// --- Create a copy of local data ------------------------------------------------

	// Create a copy of the painter
	ot::Painter2D* painterBack = nullptr;
	ot::Painter2D* painterTitleBack = nullptr;

	try {
		OT_rJSON_createDOC(backDoc);
		m_backgroundPainter->addToJsonObject(backDoc, backDoc);
		painterBack = ot::SimpleFactory::instance().createType<ot::Painter2D>(backDoc);
		painterBack->setFromJsonObject(backDoc);

		OT_rJSON_createDOC(titleBackDoc);
		m_titleBackgroundPainter->addToJsonObject(titleBackDoc, titleBackDoc);
		painterTitleBack = ot::SimpleFactory::instance().createType<ot::Painter2D>(titleBackDoc);
		painterTitleBack->setFromJsonObject(titleBackDoc);
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
		return nullptr;
	}
	OTAssertNullptr(painterBack);
	OTAssertNullptr(painterTitleBack);

	// --- Create items ------------------------------------------------

	// Root
	ot::GraphicsStackItemCfg* root = new ot::GraphicsStackItemCfg;
	root->setName(_name);
	root->setTitle(_title);

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
	mLay->setMinimumSize(ot::Size2DD(20., 50.));
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

	// Title: Left Corner
	if (m_leftTitleImagePath.empty()) {
		tLay->addStrech(1);
	}
	else {
		ot::GraphicsImageItemCfg* titLImg = new ot::GraphicsImageItemCfg;
		titLImg->setName(_name + "_titLImg");
		titLImg->setImagePath(m_leftTitleImagePath);
		titLImg->setMaximumSize(ot::Size2DD(16., 16.));
		titLImg->setAlignment(ot::AlignCenter);
		titLImg->setMargins(ot::MarginsD(0., 0., 0., 15.));
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
		titRImg->setMaximumSize(ot::Size2DD(16., 16.));
		titRImg->setAlignment(ot::AlignCenter);
		titRImg->setMargins(ot::MarginsD(5., 15., 0., 0.));
		tLay->addChildItem(titRImg);
	}

	// Central grid
	ot::GraphicsGridLayoutItemCfg* cLay = new ot::GraphicsGridLayoutItemCfg((int)std::max<size_t>(m_left.size(), m_right.size()), 5);
	cLay->setName(_name + "_cLay");

	int ix = 0;
	for (auto c : m_left) {
		c.addToGrid(ix++, cLay, true);
	}
	ix = 0;
	for (auto c : m_right) {
		c.addToGrid(ix++, cLay, false);
	}

	cLay->setColumnStretch(2, 1);

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
		cImg->setMargins(ot::MarginsD(5., 5., 5., 5.));
		cImg->setSizePolicy(ot::Dynamic);

		cStack->addItemTop(cImg, false, true);
		cStack->addItemTop(cLay, true, false);

		mLay->addChildItem(cStack, 1);
	}

	return root;
}

ot::GraphicsFlowItemCfg::GraphicsFlowItemCfg() : m_backgroundPainter(nullptr), m_titleBackgroundPainter(nullptr) {
	this->setBackgroundColor(ot::Color(50, 50, 50));
	this->setTitleBackgroundColor(ot::Color(70, 70, 70));


}

ot::GraphicsFlowItemCfg::~GraphicsFlowItemCfg() {}

void ot::GraphicsFlowItemCfg::addLeft(const std::string& _name, const std::string& _title, GraphicsFlowConnectorCfg::ConnectorFigure _figure) {
	GraphicsFlowConnectorCfg cfg = m_defaultConnectorStyle;
	cfg.setName(_name);
	cfg.setText(_title);
	cfg.setFigure(_figure);

	this->addLeft(cfg);
}

void ot::GraphicsFlowItemCfg::addLeft(const std::string& _name, const std::string& _title, GraphicsFlowConnectorCfg::ConnectorFigure _figure, ot::Color::DefaultColor _color) {
	GraphicsFlowConnectorCfg cfg = m_defaultConnectorStyle;
	cfg.setName(_name);
	cfg.setText(_title);
	cfg.setFigure(_figure);
	cfg.setPrimaryColor(ot::Color(_color));

	this->addLeft(cfg);
}

void ot::GraphicsFlowItemCfg::addLeft(const std::string& _name, const std::string& _title, GraphicsFlowConnectorCfg::ConnectorFigure _figure, const ot::Color& _color) {
	GraphicsFlowConnectorCfg cfg = m_defaultConnectorStyle;
	cfg.setName(_name);
	cfg.setText(_title);
	cfg.setFigure(_figure);
	cfg.setPrimaryColor(_color);

	this->addLeft(cfg);
}

void ot::GraphicsFlowItemCfg::addLeft(const GraphicsFlowConnectorCfg& _input) {
	m_left.push_back(_input);
}

void ot::GraphicsFlowItemCfg::addRight(const std::string& _name, const std::string& _title, GraphicsFlowConnectorCfg::ConnectorFigure _figure) {
	GraphicsFlowConnectorCfg cfg = m_defaultConnectorStyle;
	cfg.setName(_name);
	cfg.setText(_title);
	cfg.setFigure(_figure);

	this->addRight(cfg);
}

void ot::GraphicsFlowItemCfg::addRight(const std::string& _name, const std::string& _title, GraphicsFlowConnectorCfg::ConnectorFigure _figure, ot::Color::DefaultColor _color) {
	GraphicsFlowConnectorCfg cfg = m_defaultConnectorStyle;
	cfg.setName(_name);
	cfg.setText(_title);
	cfg.setFigure(_figure);
	cfg.setPrimaryColor(ot::Color(_color));

	this->addRight(cfg);
}

void ot::GraphicsFlowItemCfg::addRight(const std::string& _name, const std::string& _title, GraphicsFlowConnectorCfg::ConnectorFigure _figure, const ot::Color& _color) {
	GraphicsFlowConnectorCfg cfg = m_defaultConnectorStyle;
	cfg.setName(_name);
	cfg.setText(_title);
	cfg.setFigure(_figure);
	cfg.setPrimaryColor(_color);

	this->addRight(cfg);
}

void ot::GraphicsFlowItemCfg::addRight(const GraphicsFlowConnectorCfg& _output) {
	m_right.push_back(_output);
}

void ot::GraphicsFlowItemCfg::setBackgroundPainter(ot::Painter2D* _painter) {
	if (m_backgroundPainter == _painter) return;
	if (m_backgroundPainter) delete m_backgroundPainter;
	m_backgroundPainter = _painter;
}

void ot::GraphicsFlowItemCfg::setBackgroundColor(const ot::Color& _color) {
	this->setBackgroundPainter(new ot::FillPainter2D(_color));
}

void ot::GraphicsFlowItemCfg::setTitleBackgroundPainter(ot::Painter2D* _painter) {
	if (m_titleBackgroundPainter == _painter) return;
	if (m_titleBackgroundPainter) delete m_titleBackgroundPainter;
	m_titleBackgroundPainter = _painter;
}

void ot::GraphicsFlowItemCfg::setTitleBackgroundGradientColor(const ot::Color& _color) {
	ot::LinearGradientPainter2D* painter = new ot::LinearGradientPainter2D;
	painter->setStart(ot::Point2DD(0., 0.5));
	painter->setFinalStop(ot::Point2DD(1., 0.5));

	painter->addStop(ot::LinearGradientPainter2DStop(0, _color));
	painter->addStop(ot::LinearGradientPainter2DStop(1, ot::Color(ot::Color::Silver)));

	this->setTitleBackgroundPainter(painter);
}

void ot::GraphicsFlowItemCfg::setTitleBackgroundColor(const ot::Color& _color) {
	this->setTitleBackgroundPainter(new ot::FillPainter2D(_color));
}
