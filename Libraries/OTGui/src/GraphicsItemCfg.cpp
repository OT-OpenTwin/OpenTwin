//! @file GraphicsItemCfg.h
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTGui/GraphicsItemCfg.h"
#include "OTGui/GraphicsLayoutItemCfg.h"
#include "OTGui/Painter2D.h"
#include "OTGui/Painter2DFactory.h"
#include "OpenTwinCore/rJSON.h"
#include "OpenTwinCore/rJSONHelper.h"

#define OT_JSON_MEMBER_Top "Top" // <^^\\ '## u ##' \\ ^^^^^^O^          .    .          .     '                   '      .             '                     ^O^^^^^^// '## w ##' //^^3
#define OT_JSON_MEMBER_Name "Name" //  <^^\\ '## u ##' \\ ^^^^^^O^                                                                                           ^O^^^^^^// '## w ##' //^^3
#define OT_JSON_MEMBER_Text "Text" //   <^^\\ '## < ##' \\ ^^^^^^O^                                                                                         ^O^^^^^^// '## w ##' //^^3
#define OT_JSON_MEMBER_Size "Size" //   <^^\\ '## w ##' \\ ^^^^^^O^                 .                '                          .                        ^O^^^^^^// '## W ##' //^^3
#define OT_JSON_MEMBER_Title "Title" //    <^^\\ '## w ##' \\ ^^^^^^O^                            .              .                  .                     ^O^^^^^^// '## w ##' //^^3
#define OT_JSON_MEMBER_Flags "Flags" //     <^^\\  ## o ##' \\ ^^^^^^O^                                   '                                                  ^O^^^^^^// '## m ##' //^^3
#define OT_JSON_MEMBER_Margin "Margin" //      <^^\\  ## x ##  \\ ^^^^^^O^                                                       '                    '        ^O^^^^^^ // '## m ##' //^^3
#define OT_JSON_MEMBER_Bottom "Bottom" //        <^^\\  ## < ##  \\ ^^^^^^O^           .                       '                          .                          ^O^^^^^^  // '## > ##. //^^3
#define OT_JSON_MEMBER_TextFont "TextFont" //         < ^^\\ '## < ##' \\  ^^^^^^O^                                                                                                   ^O^^^^^^  // .## > ##..//^^ 3
#define OT_JSON_MEMBER_TextColor "TextColor" //          <  ^^\\ .## < ##: \\   ^^^^^^Ov                  .                '                          .             '                          ^O^^^^^^   //.:## > ##:.//^^  3
#define OT_JSON_MEMBER_ImagePath "ImagePath" //         <   ^^\\.:## x ##:.\\   ^^^^^^Ov                     .                    '                                      '                      ^O^^^^^^   //.:## > ##:.//^^   3
#define OT_JSON_MEMBER_CornerRadius "CornerRadius"
#define OT_JSON_MEMBER_BackgroundPainter "BackgroundPainter"

#define OT_JSON_VALUE_Connectable "Connectable"

ot::GraphicsItemCfg::GraphicsItemCfg() : m_size(10, 10), m_flags(GraphicsItemCfg::NoFlags) {}

ot::GraphicsItemCfg::~GraphicsItemCfg() {}

void ot::GraphicsItemCfg::addToJsonObject(OT_rJSON_doc& _document, OT_rJSON_val& _object) const {
	OT_rJSON_createValueObject(sizeObj);
	m_size.addToJsonObject(_document, sizeObj);
	ot::rJSON::add(_document, _object, OT_JSON_MEMBER_Size, sizeObj);

	OT_rJSON_createValueObject(marginObj);
	m_margins.addToJsonObject(_document, marginObj);
	ot::rJSON::add(_document, _object, OT_JSON_MEMBER_Margin, marginObj);

	OT_rJSON_createValueArray(flagArr);
	if (m_flags & GraphicsItemCfg::ItemIsConnectable) flagArr.PushBack(rapidjson::Value(OT_JSON_VALUE_Connectable, _document.GetAllocator()), _document.GetAllocator());
	ot::rJSON::add(_document, _object, OT_JSON_MEMBER_Flags, flagArr);

	ot::rJSON::add(_document, _object, OT_JSON_MEMBER_Name, m_name);
	ot::rJSON::add(_document, _object, OT_JSON_MEMBER_Title, m_tile);
	ot::rJSON::add(_document, _object, OT_SimpleFactoryJsonKey, this->simpleFactoryObjectKey());
}

void ot::GraphicsItemCfg::setFromJsonObject(OT_rJSON_val& _object) {
	OT_rJSON_checkMember(_object, OT_JSON_MEMBER_Name, String);
	OT_rJSON_checkMember(_object, OT_JSON_MEMBER_Title, String);
	OT_rJSON_checkMember(_object, OT_JSON_MEMBER_Size, Object);
	OT_rJSON_checkMember(_object, OT_JSON_MEMBER_Margin, Object);
	OT_rJSON_checkMember(_object, OT_JSON_MEMBER_Flags, Array);

	m_name = _object[OT_JSON_MEMBER_Name].GetString();
	m_tile = _object[OT_JSON_MEMBER_Title].GetString();

	OT_rJSON_val sizeObj = _object[OT_JSON_MEMBER_Size].GetObject();
	OT_rJSON_val marginObj = _object[OT_JSON_MEMBER_Margin].GetObject();

	m_size.setFromJsonObject(sizeObj);
	m_margins.setFromJsonObject(marginObj);

	m_flags = NoFlags;
	OT_rJSON_val flagArr = _object[OT_JSON_MEMBER_Flags].GetArray();
	for (rapidjson::SizeType i = 0; i < flagArr.Size(); i++) {
		OT_rJSON_checkArrayEntryType(flagArr, i, String);
		std::string f = flagArr[i].GetString();
		if (f == OT_JSON_VALUE_Connectable) m_flags |= ItemIsConnectable;
	}
}

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

ot::GraphicsStackItemCfg::GraphicsStackItemCfg() : m_bottom(nullptr), m_top(nullptr) {}

ot::GraphicsStackItemCfg::GraphicsStackItemCfg(ot::GraphicsItemCfg* _bottomItem, ot::GraphicsItemCfg* _topItem) : m_bottom(_bottomItem), m_top(_topItem) {}

ot::GraphicsStackItemCfg::~GraphicsStackItemCfg() {
	if (m_top) delete m_top;
	if (m_bottom) delete m_bottom;
}

void ot::GraphicsStackItemCfg::addToJsonObject(OT_rJSON_doc& _document, OT_rJSON_val& _object) const {
	ot::GraphicsItemCfg::addToJsonObject(_document, _object);

	OTAssertNullptr(m_top);
	OTAssertNullptr(m_bottom);

	OT_rJSON_createValueObject(topObj);
	if (m_top) m_top->addToJsonObject(_document, topObj);

	OT_rJSON_createValueObject(bottomObj);
	if (m_bottom) m_bottom->addToJsonObject(_document, bottomObj);

	ot::rJSON::add(_document, _object, OT_JSON_MEMBER_Top, topObj);
	ot::rJSON::add(_document, _object, OT_JSON_MEMBER_Bottom, bottomObj);
}

void ot::GraphicsStackItemCfg::setFromJsonObject(OT_rJSON_val & _object) {
	ot::GraphicsItemCfg::setFromJsonObject(_object);

	OT_rJSON_checkMember(_object, OT_JSON_MEMBER_Top, Object);
	OT_rJSON_checkMember(_object, OT_JSON_MEMBER_Bottom, Object);

	OT_rJSON_val topObj = _object[OT_JSON_MEMBER_Top].GetObject();
	OT_rJSON_val bottomObj = _object[OT_JSON_MEMBER_Bottom].GetObject();

	if (m_top) delete m_top;
	if (m_bottom) delete m_bottom;
	m_top = nullptr;
	m_bottom = nullptr;

	m_top = ot::SimpleFactory::instance().createType<ot::GraphicsItemCfg>(topObj);
	if (m_top) m_top->setFromJsonObject(topObj);
	m_bottom = ot::SimpleFactory::instance().createType<ot::GraphicsItemCfg>(bottomObj);
	if (m_bottom) m_bottom->setFromJsonObject(bottomObj);
}

void ot::GraphicsStackItemCfg::setBottomItem(ot::GraphicsItemCfg* _bottom) {
	if (m_bottom == _bottom) m_bottom = _bottom;
	if (m_bottom) delete m_bottom;
	m_bottom = _bottom;
}

void ot::GraphicsStackItemCfg::setTopItem(ot::GraphicsItemCfg* _top) {
	if (m_top == _top) m_top = _top;
	if (m_top) delete m_top;
	m_top = _top;
}

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

ot::GraphicsTextItemCfg::GraphicsTextItemCfg(const std::string& _text, const ot::Color& _textColor)
	: m_text(_text), m_textColor(_textColor) {}

ot::GraphicsTextItemCfg::~GraphicsTextItemCfg() {}

void ot::GraphicsTextItemCfg::addToJsonObject(OT_rJSON_doc& _document, OT_rJSON_val& _object) const {
	GraphicsItemCfg::addToJsonObject(_document, _object);

	ot::rJSON::add(_document, _object, OT_JSON_MEMBER_Text, m_text);

	OT_rJSON_createValueObject(fontObj);
	OT_rJSON_createValueObject(colorObj);

	m_textFont.addToJsonObject(_document, fontObj);
	m_textColor.addToJsonObject(_document, colorObj);

	ot::rJSON::add(_document, _object, OT_JSON_MEMBER_TextFont, fontObj);
	ot::rJSON::add(_document, _object, OT_JSON_MEMBER_TextColor, colorObj);
}

void ot::GraphicsTextItemCfg::setFromJsonObject(OT_rJSON_val& _object) {
	GraphicsItemCfg::setFromJsonObject(_object);

	OT_rJSON_checkMember(_object, OT_JSON_MEMBER_Text, String);
	OT_rJSON_checkMember(_object, OT_JSON_MEMBER_TextFont, Object);
	OT_rJSON_checkMember(_object, OT_JSON_MEMBER_TextColor, Object);

	m_text = _object[OT_JSON_MEMBER_Text].GetString();

	OT_rJSON_val fontObj = _object[OT_JSON_MEMBER_TextFont].GetObject();
	OT_rJSON_val colorObj = _object[OT_JSON_MEMBER_TextColor].GetObject();

	m_textFont.setFromJsonObject(fontObj);
	m_textColor.setFromJsonObject(colorObj);
}

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

ot::GraphicsImageItemCfg::GraphicsImageItemCfg(const std::string& _imageSubPath) : m_imageSubPath(_imageSubPath) {}

ot::GraphicsImageItemCfg::~GraphicsImageItemCfg() {}

void ot::GraphicsImageItemCfg::addToJsonObject(OT_rJSON_doc& _document, OT_rJSON_val& _object) const {
	GraphicsItemCfg::addToJsonObject(_document, _object);
	
	ot::rJSON::add(_document, _object, OT_JSON_MEMBER_ImagePath, m_imageSubPath);
}

void ot::GraphicsImageItemCfg::setFromJsonObject(OT_rJSON_val& _object) {
	GraphicsItemCfg::setFromJsonObject(_object);
	OT_rJSON_checkMember(_object, OT_JSON_MEMBER_ImagePath, String);
	m_imageSubPath = _object[OT_JSON_MEMBER_ImagePath].GetString();
}

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

ot::GraphicsRectangularItemCfg::GraphicsRectangularItemCfg(ot::Painter2D* _backgroundPainter, int _cornerRadius)
	: m_backgroundPainter(_backgroundPainter), m_cornerRadius(_cornerRadius)
{

}

ot::GraphicsRectangularItemCfg::~GraphicsRectangularItemCfg() {
	if (m_backgroundPainter) delete m_backgroundPainter;
}

void ot::GraphicsRectangularItemCfg::addToJsonObject(OT_rJSON_doc& _document, OT_rJSON_val& _object) const {
	GraphicsItemCfg::addToJsonObject(_document, _object);
	
	ot::rJSON::add(_document, _object, OT_JSON_MEMBER_CornerRadius, m_cornerRadius);

	if (m_backgroundPainter) {
		OT_rJSON_createValueObject(backgroundPainterObj);
		m_backgroundPainter->addToJsonObject(_document, backgroundPainterObj);
		ot::rJSON::add(_document, _object, OT_JSON_MEMBER_BackgroundPainter, backgroundPainterObj);
	}
	else {
		OT_rJSON_createValueNull(backgroundPainterObj);
		ot::rJSON::add(_document, _object, OT_JSON_MEMBER_BackgroundPainter, backgroundPainterObj);
	}
}

void ot::GraphicsRectangularItemCfg::setFromJsonObject(OT_rJSON_val& _object) {
	GraphicsItemCfg::setFromJsonObject(_object);

	OT_rJSON_checkMember(_object, OT_JSON_MEMBER_CornerRadius, Int);
	OT_rJSON_checkMemberExists(_object, OT_JSON_MEMBER_BackgroundPainter);

	m_cornerRadius = _object[OT_JSON_MEMBER_CornerRadius].GetInt();

	if (_object[OT_JSON_MEMBER_BackgroundPainter].IsObject()) {
		OT_rJSON_val backgroundPainterObj = _object[OT_JSON_MEMBER_BackgroundPainter].GetObject();
		setBackgroundPainer(ot::Painter2DFactory::painter2DFromJson(backgroundPainterObj));
	}
	else if (_object[OT_JSON_MEMBER_BackgroundPainter].IsNull()) {
		setBackgroundPainer(nullptr);
	}
	else {
		OT_LOG_E("Background painter member is not an object (or null)");
		throw std::exception("JSON object member invalid type");
	}
}

void ot::GraphicsRectangularItemCfg::setBackgroundPainer(ot::Painter2D* _painter) {
	if (_painter == m_backgroundPainter) return;
	if (m_backgroundPainter) delete m_backgroundPainter;
	m_backgroundPainter = _painter;
}

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

ot::GraphicsFlowItemCfg::GraphicsFlowItemCfg() {

}
	
ot::GraphicsFlowItemCfg::~GraphicsFlowItemCfg() {}

void ot::GraphicsFlowItemCfg::addToJsonObject(OT_rJSON_doc& _document, OT_rJSON_val& _object) const {
	// --- Create items ------------------------------------------------
	
	// Root
	ot::GraphicsStackItemCfg* root = new ot::GraphicsStackItemCfg;
	root->setGraphicsItemFlags(this->graphicsItemFlags());
	root->setName(this->name());
	root->setTitle(this->title());

	// Border
	ot::GraphicsRectangularItemCfg* bor = new ot::GraphicsRectangularItemCfg;
	bor->setCornerRadius(5);
	bor->setName(this->name() + "_bor");
	bor->setSize(ot::Size2D(200, 200));

	// Layout
	ot::GraphicsVBoxLayoutItemCfg* mLay = new ot::GraphicsVBoxLayoutItemCfg;
	mLay->setName(this->name() + "_mLay");

	// Title: Stack
	ot::GraphicsStackItemCfg* tStack = new ot::GraphicsStackItemCfg;
	tStack->setName(this->name() + "_tStack");

	// Title: Border
	ot::GraphicsRectangularItemCfg* tBor = new ot::GraphicsRectangularItemCfg;
	tBor->setName(this->name() + "_tBor");
	tBor->setCornerRadius(5);
	tBor->setSize(ot::Size2D(200, 30));

	// Title: Layout
	ot::GraphicsHBoxLayoutItemCfg* tLay = new ot::GraphicsHBoxLayoutItemCfg;
	tLay->setName(this->name() + "_tLay");

	// Title: Title
	ot::GraphicsTextItemCfg* tit = new ot::GraphicsTextItemCfg;
	tit->setName(this->name() + "_tit");
	tit->setText(this->title());

	// Central stack
	//ot::GraphicsStackItemCfg* cStack = new ot::GraphicsStackItemCfg;
	//cStack->setName(this->name() + "_cStack");

	ot::GraphicsGridLayoutItemCfg* cLay = new ot::GraphicsGridLayoutItemCfg((int)std::max<size_t>(m_inputs.size(), m_outputs.size()), 3);
	cLay->setName(this->name() + "_cLay");
	cLay->setColumnStretch(1, 1);

	int ix = 0;
	for (auto c : m_inputs) {
		cLay->addChildItem(ix++, 0, this->createConnector(c, true));
	}
	ix = 0;
	for (auto c : m_outputs) {
		cLay->addChildItem(ix++, 2, this->createConnector(c, false));
	}

	// --- Setup item relationships -------------------------------------

	root->setBottomItem(bor);
	root->setTopItem(mLay);

	tStack->setBottomItem(tBor);
	tStack->setTopItem(tLay);

	mLay->addChildItem(tStack);
	mLay->addChildItem(cLay); // Switch to stack (cStack) once image implemented
	mLay->addStrech(1);

	tLay->addStrech(1);
	tLay->addChildItem(tit);
	tLay->addStrech(1);

	// Finally add the root to the document (Flow block exports only)
	root->addToJsonObject(_document, _object);
}

void ot::GraphicsFlowItemCfg::setFromJsonObject(OT_rJSON_val& _object) {
	otAssert(0, "The GraphicsFlowItem is a export only item, check why GraphicsFlowItemCfg::setFromJsonObject() was called.");
}

void ot::GraphicsFlowItemCfg::addInput(const std::string& _name, const std::string& _title, FlowConnectorType _type) {
	FlowConnectorEntry entry;
	entry.name = _name;
	entry.title = _title;
	entry.type = _type;
	m_inputs.push_back(entry);
}

void ot::GraphicsFlowItemCfg::addOutput(const std::string& _name, const std::string& _title, FlowConnectorType _type) {
	FlowConnectorEntry entry;
	entry.name = _name;
	entry.title = _title;
	entry.type = _type;
	m_outputs.push_back(entry);
}

ot::GraphicsItemCfg* ot::GraphicsFlowItemCfg::createConnectorEntry(const FlowConnectorEntry& _entry, ot::GraphicsItemCfg* _connectorItem, bool _isInput) const {
	_connectorItem->setGraphicsItemFlags(_connectorItem->graphicsItemFlags() | ot::GraphicsItemCfg::ItemIsConnectable);

	ot::GraphicsHBoxLayoutItemCfg* itmLay = new ot::GraphicsHBoxLayoutItemCfg;
	itmLay->setName(_entry.name + "_lay");

	ot::GraphicsTextItemCfg* itmTxt = new ot::GraphicsTextItemCfg;
	itmTxt->setName(_entry.name + "_tit");
	itmTxt->setText(_entry.title);

	if (_isInput) {
		itmLay->addChildItem(_connectorItem);
		itmLay->addChildItem(itmTxt);
		itmLay->addStrech(1);
	}
	else {
		itmLay->addStrech(1);
		itmLay->addChildItem(itmTxt);
		itmLay->addChildItem(_connectorItem);
	}
	return itmLay;
}

ot::GraphicsItemCfg* ot::GraphicsFlowItemCfg::createSquareConnector(const FlowConnectorEntry& _entry, bool _isInput) const {
	ot::GraphicsRectangularItemCfg* itm = new ot::GraphicsRectangularItemCfg;
	itm->setName(_entry.name);
	itm->setTitle(_entry.title);
	itm->setSize(ot::Size2D(20, 20));

	return createConnectorEntry(_entry, itm, _isInput);
}

ot::GraphicsItemCfg* ot::GraphicsFlowItemCfg::createConnector(const FlowConnectorEntry& _entry, bool _isInput) const {
	switch (_entry.type)
	{
	case Square: return createSquareConnector(_entry, _isInput);
	default:
		OT_LOG_EA("Unknown flow connector type");
		throw std::exception("Unknown flow connector type");
	}
}

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

// Register at class factory
static ot::SimpleFactoryRegistrar<ot::GraphicsTextItemCfg> textItemCfg(OT_SimpleFactoryJsonKeyValue_GraphicsTextItemCfg);
static ot::SimpleFactoryRegistrar<ot::GraphicsStackItemCfg> stackItemCfg(OT_SimpleFactoryJsonKeyValue_GraphicsStackItemCfg);
static ot::SimpleFactoryRegistrar<ot::GraphicsImageItemCfg> imageItemCfg(OT_SimpleFactoryJsonKeyValue_GraphicsImageItemCfg);
static ot::SimpleFactoryRegistrar<ot::GraphicsRectangularItemCfg> rectItemCfg(OT_SimpleFactoryJsonKeyValue_GraphicsRectangularItemCfg);