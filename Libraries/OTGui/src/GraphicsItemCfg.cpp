//! @file GraphicsItemCfg.h
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTGui/GraphicsItemCfg.h"
#include "OTGui/GraphicsLayoutItemCfg.h"
#include "OTGui/Painter2D.h"
#include "OpenTwinCore/rJSON.h"
#include "OpenTwinCore/rJSONHelper.h"

#define OT_JSON_MEMBER_Top "Top" // <^^\\ '## u ##' \\ ^^^^^^O^          .    .          .     '                   '      .             '                     ^O^^^^^^// '## w ##' //^^3
#define OT_JSON_MEMBER_Name "Name" //  <^^\\ '## u ##' \\ ^^^^^^O^                                                                                          ^O^^^^^^// '## w ##' //^^3
#define OT_JSON_MEMBER_Text "Text" //   <^^\\ '## < ##' \\ ^^^^^^O^                                                                                       ^O^^^^^^// '## w ##' //^^3
#define OT_JSON_MEMBER_Size "Size" //   <^^\\ '## w ##' \\ ^^^^^^O^                 .                '                          .                        ^O^^^^^^// '## W ##' //^^3
#define OT_JSON_MEMBER_Title "Title" //    <^^\\ '## w ##' \\ ^^^^^^O^                            .              .                  .                     ^O^^^^^^// '## w ##' //^^3
#define OT_JSON_MEMBER_Flags "Flags" //     <^^\\  ## o ##' \\ ^^^^^^O^                                   '                                                  ^O^^^^^^// '## m ##' //^^3
#define OT_JSON_MEMBER_Border "Border" //      <^^\\  ## x ##  \\ ^^^^^^O^                                                       '                    '          ^O^^^^^^ // '## o ##' //^^3
#define OT_JSON_MEMBER_Margin "Margin" //        <^^\\  ## < ##  \\ ^^^^^^O^           .                       '                          .                            ^O^^^^^^  // '## > ##. //^^3
#define OT_JSON_MEMBER_Bottom "Bottom" //         < ^^\\ '## < ##' \\  ^^^^^^O^                                                                                              ^O^^^^^^  // .## > ##..//^^ 3
#define OT_JSON_MEMBER_Radius "Radius" //          <  ^^\\ .## < ##: \\   ^^^^^^Ov                  .                '                          .             '                      ^O^^^^^^   //.:## > ##:.//^^  3
#define OT_JSON_MEMBER_RadiusX "RadiusX" //         <   ^^\\.:## x ##:.\\   ^^^^^^Ov                     .                    '                                      '                           ^O^^^^^^   //.:## > ##:.//^^   3
#define OT_JSON_MEMBER_RadiusY "RadiusY"
#define OT_JSON_MEMBER_TextFont "TextFont"
#define OT_JSON_MEMBER_TextColor "TextColor"
#define OT_JSON_MEMBER_ImagePath "ImagePath"
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
	if (m_backgroundPainter == nullptr) {
		// If no background painter was provided, we set a transparent background as default
		m_backgroundPainter = new ot::FillPainter2D(ot::Color(0, 0, 0, 0));
	}
}

ot::GraphicsRectangularItemCfg::~GraphicsRectangularItemCfg() {
	if (m_backgroundPainter) delete m_backgroundPainter;
}

void ot::GraphicsRectangularItemCfg::addToJsonObject(OT_rJSON_doc& _document, OT_rJSON_val& _object) const {
	OTAssertNullptr(m_backgroundPainter);

	GraphicsItemCfg::addToJsonObject(_document, _object);
	
	ot::rJSON::add(_document, _object, OT_JSON_MEMBER_CornerRadius, m_cornerRadius);

	OT_rJSON_createValueObject(backgroundPainterObj);
	m_backgroundPainter->addToJsonObject(_document, backgroundPainterObj);
	ot::rJSON::add(_document, _object, OT_JSON_MEMBER_BackgroundPainter, backgroundPainterObj);

	OT_rJSON_createValueObject(borderObj);
	m_border.addToJsonObject(_document, borderObj);
	ot::rJSON::add(_document, _object, OT_JSON_MEMBER_Border, borderObj);
}

void ot::GraphicsRectangularItemCfg::setFromJsonObject(OT_rJSON_val& _object) {
	GraphicsItemCfg::setFromJsonObject(_object);

	OT_rJSON_checkMember(_object, OT_JSON_MEMBER_CornerRadius, Int);
	OT_rJSON_checkMember(_object, OT_JSON_MEMBER_Border, Object);
	OT_rJSON_checkMember(_object, OT_JSON_MEMBER_BackgroundPainter, Object);

	m_cornerRadius = _object[OT_JSON_MEMBER_CornerRadius].GetInt();

	OT_rJSON_val backgroundPainterObj = _object[OT_JSON_MEMBER_BackgroundPainter].GetObject();
	ot::Painter2D* p = ot::SimpleFactory::instance().createType<ot::Painter2D>(backgroundPainterObj);
	p->setFromJsonObject(backgroundPainterObj);
	this->setBackgroundPainer(p);

	OT_rJSON_val borderObj = _object[OT_JSON_MEMBER_Border].GetObject();
	m_border.setFromJsonObject(borderObj);
}

void ot::GraphicsRectangularItemCfg::setBackgroundPainer(ot::Painter2D* _painter) {
	if (_painter == m_backgroundPainter) return;
	if (m_backgroundPainter) delete m_backgroundPainter;
	m_backgroundPainter = _painter;
}

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

ot::GraphicsEllipseItemCfg::GraphicsEllipseItemCfg(int _radiusX, int _radiusY, ot::Painter2D* _backgroundPainter)
	: m_backgroundPainter(_backgroundPainter), m_radiusX(_radiusX), m_radiusY(_radiusY)
{
	if (m_backgroundPainter == nullptr) {
		// If no background painter was provided, we set a transparent background as default
		m_backgroundPainter = new ot::FillPainter2D(ot::Color(0, 0, 0, 0));
	}
}

ot::GraphicsEllipseItemCfg::~GraphicsEllipseItemCfg() {
	if (m_backgroundPainter) delete m_backgroundPainter;
}

void ot::GraphicsEllipseItemCfg::addToJsonObject(OT_rJSON_doc& _document, OT_rJSON_val& _object) const {
	OTAssertNullptr(m_backgroundPainter);

	GraphicsItemCfg::addToJsonObject(_document, _object);

	ot::rJSON::add(_document, _object, OT_JSON_MEMBER_RadiusX, m_radiusX);
	ot::rJSON::add(_document, _object, OT_JSON_MEMBER_RadiusY, m_radiusY);

	OT_rJSON_createValueObject(backgroundPainterObj);
	m_backgroundPainter->addToJsonObject(_document, backgroundPainterObj);
	ot::rJSON::add(_document, _object, OT_JSON_MEMBER_BackgroundPainter, backgroundPainterObj);

	OT_rJSON_createValueObject(borderObj);
	m_border.addToJsonObject(_document, borderObj);
	ot::rJSON::add(_document, _object, OT_JSON_MEMBER_Border, borderObj);
}

void ot::GraphicsEllipseItemCfg::setFromJsonObject(OT_rJSON_val& _object) {
	GraphicsItemCfg::setFromJsonObject(_object);

	OT_rJSON_checkMember(_object, OT_JSON_MEMBER_RadiusX, Int);
	OT_rJSON_checkMember(_object, OT_JSON_MEMBER_RadiusY, Int);
	OT_rJSON_checkMember(_object, OT_JSON_MEMBER_Border, Object);
	OT_rJSON_checkMember(_object, OT_JSON_MEMBER_BackgroundPainter, Object);

	m_radiusX = _object[OT_JSON_MEMBER_RadiusX].GetInt();
	m_radiusY = _object[OT_JSON_MEMBER_RadiusY].GetInt();

	OT_rJSON_val backgroundPainterObj = _object[OT_JSON_MEMBER_BackgroundPainter].GetObject();
	ot::Painter2D* p = ot::SimpleFactory::instance().createType<ot::Painter2D>(backgroundPainterObj);
	p->setFromJsonObject(backgroundPainterObj);
	this->setBackgroundPainer(p);

	OT_rJSON_val borderObj = _object[OT_JSON_MEMBER_Border].GetObject();
	m_border.setFromJsonObject(borderObj);
}

void ot::GraphicsEllipseItemCfg::setBackgroundPainer(ot::Painter2D* _painter) {
	if (_painter == m_backgroundPainter) return;
	if (m_backgroundPainter) delete m_backgroundPainter;
	m_backgroundPainter = _painter;
}

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

// Register at class factory
static ot::SimpleFactoryRegistrar<ot::GraphicsTextItemCfg> textItemCfg(OT_SimpleFactoryJsonKeyValue_GraphicsTextItemCfg);
static ot::SimpleFactoryRegistrar<ot::GraphicsStackItemCfg> stackItemCfg(OT_SimpleFactoryJsonKeyValue_GraphicsStackItemCfg);
static ot::SimpleFactoryRegistrar<ot::GraphicsImageItemCfg> imageItemCfg(OT_SimpleFactoryJsonKeyValue_GraphicsImageItemCfg);
static ot::SimpleFactoryRegistrar<ot::GraphicsEllipseItemCfg> ellipseItemCfg(OT_SimpleFactoryJsonKeyValue_GraphicsEllipseItemCfg);
static ot::SimpleFactoryRegistrar<ot::GraphicsRectangularItemCfg> rectItemCfg(OT_SimpleFactoryJsonKeyValue_GraphicsRectangularItemCfg);