//! @file GraphicsFlowConnectorCfg.cpp
//! 
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTGui/GraphicsFlowItemCfg.h"
#include "OTGui/GraphicsLayoutItemCfg.h"
#include "OTGui/Painter2D.h"

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

ot::GraphicsItemCfg* ot::GraphicsFlowConnectorCfg::toGraphicsItem(bool _isInput) {
	// Connector item
	ot::GraphicsItemCfg* itm = this->createConnectorItem();
	itm->setGraphicsItemFlags(ot::GraphicsItemCfg::ItemIsConnectable);
	itm->setName(m_name);

	// Layout
	ot::GraphicsHBoxLayoutItemCfg* itmLay = new ot::GraphicsHBoxLayoutItemCfg;
	itmLay->setName(m_name + "_lay");

	// Title item
	ot::GraphicsTextItemCfg* itmTxt = new ot::GraphicsTextItemCfg;
	itmTxt->setName(m_name + "_tit");
	itmTxt->setText(m_text);
	itmTxt->setTextColor(m_textColor);
	itmTxt->setTextFont(m_font);

	// Place items in layout (input:  [] text     output:  text [])
	if (_isInput) {
		itmLay->addChildItem(itm, 0);
		itmLay->addChildItem(itmTxt, 0);
		itmLay->addStrech(1);
	}
	else {
		itmLay->addStrech(1);
		itmLay->addChildItem(itmTxt, 0);
		itmLay->addChildItem(itm, 0);
	}

	// Return the layout holding the text and item
	return itmLay;
}

ot::GraphicsItemCfg* ot::GraphicsFlowConnectorCfg::createConnectorItem(void) {
	switch (m_figure)
	{
	case ot::GraphicsFlowConnectorCfg::Square: return this->createSquareItem();
	default:
		OT_LOG_EA("Unknown connector type");
		throw std::exception("Unknown connector type");
	}
}

ot::GraphicsItemCfg* ot::GraphicsFlowConnectorCfg::createSquareItem(void) {
	ot::GraphicsRectangularItemCfg* itm = new ot::GraphicsRectangularItemCfg(new ot::FillPainter2D(m_primaryColor));
	itm->setBorder(ot::Border(m_secondaryColor, 1));

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
	bor->setSize(ot::Size2D(200, 200));
	root->setBottomItem(bor);

	// Layout
	ot::GraphicsVBoxLayoutItemCfg* mLay = new ot::GraphicsVBoxLayoutItemCfg;
	mLay->setName(_name + "_mLay");
	root->setTopItem(mLay);

	// Title: Stack
	ot::GraphicsStackItemCfg* tStack = new ot::GraphicsStackItemCfg;
	tStack->setName(_name + "_tStack");
	mLay->addChildItem(tStack);

	// Title: Border
	ot::GraphicsRectangularItemCfg* tBor = new ot::GraphicsRectangularItemCfg(painterTitleBack);
	tBor->setBorder(ot::Border(ot::Color(0, 0, 0), 1));
	tBor->setName(_name + "_tBor");
	tBor->setCornerRadius(5);
	tBor->setSize(ot::Size2D(200, 30));
	tStack->setBottomItem(tBor);

	// Title: Layout
	ot::GraphicsHBoxLayoutItemCfg* tLay = new ot::GraphicsHBoxLayoutItemCfg;
	tLay->setName(_name + "_tLay");
	tStack->setTopItem(tLay);

	// Title: Title
	ot::GraphicsTextItemCfg* tit = new ot::GraphicsTextItemCfg;
	tit->setName(_name + "_tit");
	tit->setText(_title);
	tLay->addStrech(1);
	tLay->addChildItem(tit);
	tLay->addStrech(1);

	// Central grid
	ot::GraphicsGridLayoutItemCfg* cLay = new ot::GraphicsGridLayoutItemCfg((int)std::max<size_t>(m_inputs.size(), m_outputs.size()), 3);
	cLay->setName(_name + "_cLay");
	cLay->setColumnStretch(1, 1);

	int ix = 0;
	for (auto c : m_inputs) {
		cLay->addChildItem(ix++, 0, c.toGraphicsItem(true));
	}
	ix = 0;
	for (auto c : m_outputs) {
		cLay->addChildItem(ix++, 2, c.toGraphicsItem(false));
	}

	// Central stack
	if (m_backgroundImagePath.empty()) {
		mLay->addChildItem(cLay);
	}
	else {
		ot::GraphicsStackItemCfg* cStack = new ot::GraphicsStackItemCfg;
		cStack->setName(_name + "_cStack");

		ot::GraphicsImageItemCfg* cImg = new ot::GraphicsImageItemCfg;
		cImg->setImagePath(m_backgroundImagePath);
		cImg->setName(_name + "_cImg");

		cStack->setBottomItem(cImg);
		cStack->setTopItem(cLay);

		mLay->addChildItem(cStack);
	}
	mLay->addStrech(1);

	return root;
}

ot::GraphicsFlowItemCfg::GraphicsFlowItemCfg() : m_backgroundPainter(nullptr), m_titleBackgroundPainter(nullptr) {
	this->setBackgroundColor(ot::Color(50, 50, 50));
	this->setTitleBackgroundColor(ot::Color(70, 70, 70));


}

ot::GraphicsFlowItemCfg::~GraphicsFlowItemCfg() {}

void ot::GraphicsFlowItemCfg::addInput(const std::string& _name, const std::string& _title, GraphicsFlowConnectorCfg::ConnectorFigure _figure) {
	GraphicsFlowConnectorCfg cfg = m_defaultConnectorStyle;
	cfg.setName(_name);
	cfg.setText(_title);
	cfg.setFigure(_figure);

	this->addInput(cfg);
}

void ot::GraphicsFlowItemCfg::addInput(const std::string& _name, const std::string& _title, GraphicsFlowConnectorCfg::ConnectorFigure _figure, ot::Color::DefaultColor _color) {
	GraphicsFlowConnectorCfg cfg = m_defaultConnectorStyle;
	cfg.setName(_name);
	cfg.setText(_title);
	cfg.setFigure(_figure);
	cfg.setPrimaryColor(ot::Color(_color));

	this->addInput(cfg);
}

void ot::GraphicsFlowItemCfg::addInput(const std::string& _name, const std::string& _title, GraphicsFlowConnectorCfg::ConnectorFigure _figure, const ot::Color& _color) {
	GraphicsFlowConnectorCfg cfg = m_defaultConnectorStyle;
	cfg.setName(_name);
	cfg.setText(_title);
	cfg.setFigure(_figure);
	cfg.setPrimaryColor(_color);

	this->addInput(cfg);
}

void ot::GraphicsFlowItemCfg::addInput(const GraphicsFlowConnectorCfg& _input) {
	m_inputs.push_back(_input);
}

void ot::GraphicsFlowItemCfg::addOutput(const std::string& _name, const std::string& _title, GraphicsFlowConnectorCfg::ConnectorFigure _figure) {
	GraphicsFlowConnectorCfg cfg = m_defaultConnectorStyle;
	cfg.setName(_name);
	cfg.setText(_title);
	cfg.setFigure(_figure);

	this->addOutput(cfg);
}

void ot::GraphicsFlowItemCfg::addOutput(const std::string& _name, const std::string& _title, GraphicsFlowConnectorCfg::ConnectorFigure _figure, ot::Color::DefaultColor _color) {
	GraphicsFlowConnectorCfg cfg = m_defaultConnectorStyle;
	cfg.setName(_name);
	cfg.setText(_title);
	cfg.setFigure(_figure);
	cfg.setPrimaryColor(ot::Color(_color));

	this->addOutput(cfg);
}

void ot::GraphicsFlowItemCfg::addOutput(const std::string& _name, const std::string& _title, GraphicsFlowConnectorCfg::ConnectorFigure _figure, const ot::Color& _color) {
	GraphicsFlowConnectorCfg cfg = m_defaultConnectorStyle;
	cfg.setName(_name);
	cfg.setText(_title);
	cfg.setFigure(_figure);
	cfg.setPrimaryColor(_color);

	this->addOutput(cfg);
}

void ot::GraphicsFlowItemCfg::addOutput(const GraphicsFlowConnectorCfg& _output) {
	m_outputs.push_back(_output);
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

void ot::GraphicsFlowItemCfg::setTitleBackgroundColor(const ot::Color& _color) {
	this->setTitleBackgroundPainter(new ot::FillPainter2D(_color));
}
