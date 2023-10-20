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

#define OT_JSON_MEMBER_Uid "UID"
#define OT_JSON_MEMBER_Name "Name" // <^^\\ '## u ##' \\ ^^^^^^O^          .    .          .     '                   '      .             '                     ^O^^^^^^// '## w ##' //^^3
#define OT_JSON_MEMBER_Text "Text" //  <^^\\ '## u ##' \\ ^^^^^^O^                                                                                          ^O^^^^^^// '## w ##' //^^3
#define OT_JSON_MEMBER_Size "Size" //   <^^\\ '## < ##' \\ ^^^^^^O^                                                                                       ^O^^^^^^// '## w ##' //^^3
#define OT_JSON_MEMBER_Item "Item" //   <^^\\ '## w ##' \\ ^^^^^^O^                 .                '                          .                        ^O^^^^^^// '## W ##' //^^3
#define OT_JSON_MEMBER_Items "Items" //    <^^\\ '## w ##' \\ ^^^^^^O^                            .              .                  .                     ^O^^^^^^// '## w ##' //^^3
#define OT_JSON_MEMBER_Title "Title" //     <^^\\  ## o ##' \\ ^^^^^^O^                                   '                                                  ^O^^^^^^// '## m ##' //^^3
#define OT_JSON_MEMBER_Flags "Flags" //      <^^\\  ## x ##  \\ ^^^^^^O^                                                       '                    '          ^O^^^^^^ // '## o ##' //^^3
#define OT_JSON_MEMBER_Border "Border" //        <^^\\  ## < ##  \\ ^^^^^^O^           .                       '                          .                            ^O^^^^^^  // '## > ##. //^^3
#define OT_JSON_MEMBER_Margin "Margin" //         < ^^\\ '## < ##' \\  ^^^^^^O^                                                                                              ^O^^^^^^  // .## > ##..//^^ 3
#define OT_JSON_MEMBER_Radius "Radius" //          <  ^^\\ .## < ##: \\   ^^^^^^Ov                  .                '                          .             '                      ^O^^^^^^   //.:## > ##:.//^^  3
#define OT_JSON_MEMBER_RadiusX "RadiusX" //         <   ^^\\.:## x ##:.\\   ^^^^^^Ov                     .                    '                                      '                           ^O^^^^^^   //.:## > ##:.//^^   3
#define OT_JSON_MEMBER_RadiusY "RadiusY"
#define OT_JSON_MEMBER_MinSize "Size.Min"
#define OT_JSON_MEMBER_MaxSize "Size.Max"
#define OT_JSON_MEMBER_IsSlave "IsSlave"
#define OT_JSON_MEMBER_Position "Position"
#define OT_JSON_MEMBER_IsMaster "IsMaster"
#define OT_JSON_MEMBER_TextFont "TextFont"
#define OT_JSON_MEMBER_TextColor "TextColor"
#define OT_JSON_MEMBER_ImagePath "ImagePath"
#define OT_JSON_MEMBER_Alignment "Alignment"
#define OT_JSON_MEMBER_CornerRadius "CornerRadius"
#define OT_JSON_MEMBER_BackgroundPainter "BackgroundPainter"

#define OT_JSON_VALUE_Connectable "Connectable"

ot::GraphicsItemCfg::GraphicsItemCfg() : m_pos(0., 0.), m_flags(GraphicsItemCfg::NoFlags), m_alignment(ot::AlignCenter), m_minSize(0., 0.), m_maxSize(DBL_MAX, DBL_MAX) {}

ot::GraphicsItemCfg::~GraphicsItemCfg() {}

void ot::GraphicsItemCfg::addToJsonObject(OT_rJSON_doc& _document, OT_rJSON_val& _object) const {
	OT_rJSON_createValueObject(posObj);
	m_pos.addToJsonObject(_document, posObj);
	ot::rJSON::add(_document, _object, OT_JSON_MEMBER_Position, posObj);

	OT_rJSON_createValueObject(minSizeObj);
	m_minSize.addToJsonObject(_document, minSizeObj);
	ot::rJSON::add(_document, _object, OT_JSON_MEMBER_MinSize, minSizeObj);

	OT_rJSON_createValueObject(maxSizeObj);
	m_maxSize.addToJsonObject(_document, maxSizeObj);
	ot::rJSON::add(_document, _object, OT_JSON_MEMBER_MaxSize, maxSizeObj);

	OT_rJSON_createValueObject(marginObj);
	m_margins.addToJsonObject(_document, marginObj);
	ot::rJSON::add(_document, _object, OT_JSON_MEMBER_Margin, marginObj);

	OT_rJSON_createValueArray(flagArr);
	if (m_flags & GraphicsItemCfg::ItemIsConnectable) flagArr.PushBack(rapidjson::Value(OT_JSON_VALUE_Connectable, _document.GetAllocator()), _document.GetAllocator());
	ot::rJSON::add(_document, _object, OT_JSON_MEMBER_Flags, flagArr);

	ot::rJSON::add(_document, _object, OT_JSON_MEMBER_Uid, m_uid);
	ot::rJSON::add(_document, _object, OT_JSON_MEMBER_Name, m_name);
	ot::rJSON::add(_document, _object, OT_JSON_MEMBER_Title, m_tile);
	ot::rJSON::add(_document, _object, OT_JSON_MEMBER_Alignment, ot::toString(m_alignment));
	ot::rJSON::add(_document, _object, OT_SimpleFactoryJsonKey, this->simpleFactoryObjectKey());
}

void ot::GraphicsItemCfg::setFromJsonObject(OT_rJSON_val& _object) {
	OT_rJSON_checkMember(_object, OT_JSON_MEMBER_Uid, String);
	OT_rJSON_checkMember(_object, OT_JSON_MEMBER_Name, String);
	OT_rJSON_checkMember(_object, OT_JSON_MEMBER_Title, String);
	OT_rJSON_checkMember(_object, OT_JSON_MEMBER_Position, Object);
	OT_rJSON_checkMember(_object, OT_JSON_MEMBER_MinSize, Object);
	OT_rJSON_checkMember(_object, OT_JSON_MEMBER_MaxSize, Object);
	OT_rJSON_checkMember(_object, OT_JSON_MEMBER_Margin, Object);
	OT_rJSON_checkMember(_object, OT_JSON_MEMBER_Flags, Array);
	OT_rJSON_checkMember(_object, OT_JSON_MEMBER_Alignment, String);

	m_uid = _object[OT_JSON_MEMBER_Uid].GetString();
	m_name = _object[OT_JSON_MEMBER_Name].GetString();
	m_tile = _object[OT_JSON_MEMBER_Title].GetString();
	m_alignment = ot::stringToAlignment(_object[OT_JSON_MEMBER_Alignment].GetString());

	OT_rJSON_val posObj = _object[OT_JSON_MEMBER_Position].GetObject();
	OT_rJSON_val marginObj = _object[OT_JSON_MEMBER_Margin].GetObject();
	OT_rJSON_val minSizeObj = _object[OT_JSON_MEMBER_MinSize].GetObject();
	OT_rJSON_val maxSizeObj = _object[OT_JSON_MEMBER_MaxSize].GetObject();

	m_pos.setFromJsonObject(posObj);
	m_margins.setFromJsonObject(marginObj);
	m_minSize.setFromJsonObject(minSizeObj);
	m_maxSize.setFromJsonObject(maxSizeObj);

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

ot::GraphicsStackItemCfg::GraphicsStackItemCfg()
{

}

ot::GraphicsStackItemCfg::~GraphicsStackItemCfg() {
	this->memClear();
}

void ot::GraphicsStackItemCfg::addToJsonObject(OT_rJSON_doc& _document, OT_rJSON_val& _object) const {
	ot::GraphicsItemCfg::addToJsonObject(_document, _object);

	otAssert(m_items.size() > 1, "Please add at least two items to the stack");
#ifdef _DEBUG
	{
		bool checkOK = false;
		for (auto itm : m_items) {
			if (itm.isMaster) {
				checkOK = true;
				break;
			}
		}
		otAssert(checkOK, "No master item was set. The size adjustment between the items won't be executed");
	}
#endif // _DEBUG


	OT_rJSON_createValueArray(itemArr);
	for (const GraphicsStackItemCfgEntry& itm : m_items) {
		OT_rJSON_createValueObject(itemObj);
		OT_rJSON_createValueObject(itemContObj);
		itm.item->addToJsonObject(_document, itemContObj);
		ot::rJSON::add(_document, itemObj, OT_JSON_MEMBER_Item, itemContObj);
		ot::rJSON::add(_document, itemObj, OT_JSON_MEMBER_IsMaster, itm.isMaster);
		ot::rJSON::add(_document, itemObj, OT_JSON_MEMBER_IsSlave, itm.isSlave);
		itemArr.PushBack(itemObj, _document.GetAllocator());
	}
	ot::rJSON::add(_document, _object, OT_JSON_MEMBER_Items, itemArr);
}

void ot::GraphicsStackItemCfg::setFromJsonObject(OT_rJSON_val & _object) {
	ot::GraphicsItemCfg::setFromJsonObject(_object);

	this->memClear();

	OT_rJSON_checkMember(_object, OT_JSON_MEMBER_Items, Array);
	OT_rJSON_val itemArr = _object[OT_JSON_MEMBER_Items].GetArray();

	for (rapidjson::SizeType i = 0; i < itemArr.Size(); i++) {
		OT_rJSON_checkArrayEntryType(itemArr, i, Object);
		OT_rJSON_val itemObj = itemArr[i].GetObject();

		OT_rJSON_checkMember(itemObj, OT_JSON_MEMBER_Item, Object);
		OT_rJSON_checkMember(itemObj, OT_JSON_MEMBER_IsMaster, Bool);
		OT_rJSON_val itemContObj = itemObj[OT_JSON_MEMBER_Item].GetObject();

		ot::GraphicsItemCfg* itm = nullptr;
		try {
			itm = ot::SimpleFactory::instance().createType<ot::GraphicsItemCfg>(itemContObj);
			if (itm) {
				itm->setFromJsonObject(itemContObj);

				GraphicsStackItemCfgEntry e;
				e.isMaster = itemObj[OT_JSON_MEMBER_IsMaster].GetBool();
				e.isSlave = itemObj[OT_JSON_MEMBER_IsSlave].GetBool();
				e.item = itm;
				m_items.push_back(e);
			}
			else {
				OT_LOG_EA("Failed to create child item from factory. Ignore");
			}
		}
		catch (const std::exception& _e) {
			OT_LOG_EAS("Failed to create child item from factory: " + std::string(_e.what()));
			if (itm) delete itm;
			throw _e;
		}
		catch (...) {
			OT_LOG_EA("[FATAL] Unknown error");
			if (itm) delete itm;
			throw std::exception("[FATAL] Unknown error");
		}
	}
}

void ot::GraphicsStackItemCfg::addItemTop(ot::GraphicsItemCfg* _item, bool _isMaster, bool _isSlave) {
	GraphicsStackItemCfgEntry e;
	e.isMaster = _isMaster;
	e.isSlave = _isSlave;
	e.item = _item;
	m_items.push_back(e);
}

void ot::GraphicsStackItemCfg::addItemBottom(ot::GraphicsItemCfg* _item, bool _isMaster, bool _isSlave) {
	GraphicsStackItemCfgEntry e;
	e.isMaster = _isMaster;
	e.isSlave = _isSlave;
	e.item = _item;
	m_items.push_front(e);
}

void ot::GraphicsStackItemCfg::memClear(void) {
	for (auto itm : m_items) delete itm.item;
	m_items.clear();
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

	OT_rJSON_createValueObject(sizeObj);
	m_size.addToJsonObject(_document, sizeObj);
	ot::rJSON::add(_document, _object, OT_JSON_MEMBER_Size, sizeObj);

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
	OT_rJSON_checkMember(_object, OT_JSON_MEMBER_Size, Object);

	m_cornerRadius = _object[OT_JSON_MEMBER_CornerRadius].GetInt();

	OT_rJSON_val backgroundPainterObj = _object[OT_JSON_MEMBER_BackgroundPainter].GetObject();
	ot::Painter2D* p = ot::SimpleFactory::instance().createType<ot::Painter2D>(backgroundPainterObj);
	p->setFromJsonObject(backgroundPainterObj);
	this->setBackgroundPainer(p);

	OT_rJSON_val borderObj = _object[OT_JSON_MEMBER_Border].GetObject();
	m_border.setFromJsonObject(borderObj);

	OT_rJSON_val sizeObj = _object[OT_JSON_MEMBER_Size].GetObject();
	m_size.setFromJsonObject(sizeObj);
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