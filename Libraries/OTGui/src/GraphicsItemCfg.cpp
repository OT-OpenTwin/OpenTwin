//! @file GraphicsItemCfg.h
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTGui/GraphicsItemCfg.h"
#include "OTGui/Painter2D.h"
#include "OTGui/Painter2DFactory.h"
#include "OpenTwinCore/rJSON.h"
#include "OpenTwinCore/rJSONHelper.h"

#define OT_JSON_MEMBER_Top "Top"
#define OT_JSON_MEMBER_Bottom "Bottom"
#define OT_JSON_MEMBER_Flags "Flags"
#define OT_JSON_MEMBER_Name "Name" // <^^\\ '## u ##' \\ ^^^^^^O^          .    .          .     '                   '      .             '                     ^O^^^^^^// '## w ##' //^^3
#define OT_JSON_MEMBER_Text "Text" //  <^^\\ '## u ##' \\ ^^^^^^O^                                                                                           ^O^^^^^^// '## w ##' //^^3
#define OT_JSON_MEMBER_Size "Size" //   <^^\\ '## < ##' \\ ^^^^^^O^                                                                                         ^O^^^^^^// '## w ##' //^^3
#define OT_JSON_MEMBER_Title "Title" //   <^^\\ '## w ##' \\ ^^^^^^O^                 .                '                          .                        ^O^^^^^^// '## W ##' //^^3
#define OT_JSON_MEMBER_Border "Border" //    <^^\\ '## w ##' \\ ^^^^^^O^                            .              .                  .                     ^O^^^^^^// '## w ##' //^^3
#define OT_JSON_MEMBER_Margin "Margin" //     <^^\\  ## o ##' \\ ^^^^^^O^                                   '                                                  ^O^^^^^^// '## m ##' //^^3
#define OT_JSON_MEMBER_TextFont "TextFont" //      <^^\\  ## x ##  \\ ^^^^^^O^                                                       '                    '        ^O^^^^^^ // '## m ##' //^^3
#define OT_JSON_MEMBER_TextColor "TextColor" //        <^^\\  ## < ##  \\ ^^^^^^O^           .                       '                          .                          ^O^^^^^^  // '## > ##. //^^3
#define OT_JSON_MEMBER_ImagePath "ImagePath" //         < ^^\\ '## < ##' \\  ^^^^^^O^                                                                                                   ^O^^^^^^  // .## > ##..//^^ 3
#define OT_JSON_MEMBER_CornerRadius "CornerRadius" //          <  ^^\\ .## < ##: \\   ^^^^^^Ov                  .                '                          .             '                          ^O^^^^^^   //.:## > ##:.//^^  3
#define OT_JSON_MEMBER_BackgroundPainter "BackgroundPainter" //         <   ^^\\.:## x ##:.\\   ^^^^^^Ov                     .                    '                                      '                      ^O^^^^^^   //.:## > ##:.//^^   3

ot::GraphicsItemCfg::GraphicsItemCfg() : m_size(10, 10), m_flags(GraphicsItemCfg::NoFlags) {}

ot::GraphicsItemCfg::~GraphicsItemCfg() {}

void ot::GraphicsItemCfg::addToJsonObject(OT_rJSON_doc& _document, OT_rJSON_val& _object) const {
	OT_rJSON_createValueObject(sizeObj);
	m_size.addToJsonObject(_document, sizeObj);
	ot::rJSON::add(_document, _object, OT_JSON_MEMBER_Size, sizeObj);

	OT_rJSON_createValueObject(borderObj);
	m_border.addToJsonObject(_document, borderObj);
	ot::rJSON::add(_document, _object, OT_JSON_MEMBER_Border, borderObj);

	OT_rJSON_createValueObject(marginObj);
	m_margins.addToJsonObject(_document, marginObj);
	ot::rJSON::add(_document, _object, OT_JSON_MEMBER_Margin, marginObj);

	OT_rJSON_createValueArray(flagArr);
	if (m_flags & GraphicsItemCfg::ItemIsConnectable) flagArr.PushBack(rapidjson::Value("Connectable", _document.GetAllocator()), _document.GetAllocator());
	ot::rJSON::add(_document, _object, OT_JSON_MEMBER_Flags, flagArr);

	ot::rJSON::add(_document, _object, OT_JSON_MEMBER_Name, m_name);
	ot::rJSON::add(_document, _object, OT_JSON_MEMBER_Title, m_tile);
	ot::rJSON::add(_document, _object, OT_SimpleFactoryJsonKey, this->simpleFactoryObjectKey());
}

void ot::GraphicsItemCfg::setFromJsonObject(OT_rJSON_val& _object) {
	OT_rJSON_checkMember(_object, OT_JSON_MEMBER_Name, String);
	OT_rJSON_checkMember(_object, OT_JSON_MEMBER_Title, String);
	OT_rJSON_checkMember(_object, OT_JSON_MEMBER_Size, Object);
	OT_rJSON_checkMember(_object, OT_JSON_MEMBER_Border, Object);
	OT_rJSON_checkMember(_object, OT_JSON_MEMBER_Margin, Object);
	OT_rJSON_checkMember(_object, OT_JSON_MEMBER_Flags, Array);

	m_name = _object[OT_JSON_MEMBER_Name].GetString();
	m_tile = _object[OT_JSON_MEMBER_Title].GetString();

	OT_rJSON_val sizeObj = _object[OT_JSON_MEMBER_Size].GetObject();
	OT_rJSON_val borderObj = _object[OT_JSON_MEMBER_Border].GetObject();
	OT_rJSON_val marginObj = _object[OT_JSON_MEMBER_Margin].GetObject();

	m_size.setFromJsonObject(sizeObj);
	m_border.setFromJsonObject(borderObj);
	m_margins.setFromJsonObject(marginObj);

	m_flags = NoFlags;
	OT_rJSON_val flagArr = _object[OT_JSON_MEMBER_Flags].GetArray();
	for (rapidjson::SizeType i = 0; i < flagArr.Size(); i++) {
		OT_rJSON_checkArrayEntryType(flagArr, i, String);
		if (flagArr[i].GetString() == "Connectable") m_flags |= ItemIsConnectable;
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
	m_top->setFromJsonObject(topObj);
	m_bottom = ot::SimpleFactory::instance().createType<ot::GraphicsItemCfg>(bottomObj);
	m_bottom->setFromJsonObject(bottomObj);
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

// Register at class factory
static ot::SimpleFactoryRegistrar<ot::GraphicsTextItemCfg> textItemCfg(OT_SimpleFactoryJsonKeyValue_GraphicsTextItemCfg);
static ot::SimpleFactoryRegistrar<ot::GraphicsStackItemCfg> stackItemCfg(OT_SimpleFactoryJsonKeyValue_GraphicsStackItemCfg);
static ot::SimpleFactoryRegistrar<ot::GraphicsImageItemCfg> imageItemCfg(OT_SimpleFactoryJsonKeyValue_GraphicsImageItemCfg);
static ot::SimpleFactoryRegistrar<ot::GraphicsRectangularItemCfg> rectItemCfg(OT_SimpleFactoryJsonKeyValue_GraphicsRectangularItemCfg);