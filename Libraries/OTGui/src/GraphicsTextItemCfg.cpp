//! @file GraphicsTextItemCfg.cpp
//! 
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTGui/FillPainter2D.h"
#include "OTGui/Painter2DFactory.h"
#include "OTGui/GraphicsTextItemCfg.h"
#include "OTGui/GraphicsItemCfgFactory.h"

#define OT_JSON_MEMBER_Text "Text"
#define OT_JSON_MEMBER_TextFont "TextFont"
#define OT_JSON_MEMBER_TextStyle "TextStyle"

static ot::GraphicsItemCfgFactoryRegistrar<ot::GraphicsTextItemCfg> textItemCfg(OT_FactoryKey_GraphicsTextItem);

ot::GraphicsTextItemCfg::GraphicsTextItemCfg(const std::string& _text, const ot::Color& _textColor)
	: m_text(_text)
{
	m_textStyle.setColor(_textColor);
}

ot::GraphicsTextItemCfg::~GraphicsTextItemCfg() {}

ot::GraphicsItemCfg* ot::GraphicsTextItemCfg::createCopy(void) const {
	ot::GraphicsTextItemCfg* copy = new GraphicsTextItemCfg;
	this->copyConfigDataToItem(copy);

	copy->m_text = m_text;
	copy->m_textFont = m_textFont;
	copy->m_textStyle = m_textStyle;

	return copy;
}

void ot::GraphicsTextItemCfg::addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const {
	GraphicsItemCfg::addToJsonObject(_object, _allocator);

	_object.AddMember(OT_JSON_MEMBER_Text, JsonString(m_text, _allocator), _allocator);
	
	JsonObject fontObj;
	m_textFont.addToJsonObject(fontObj, _allocator);
	_object.AddMember(OT_JSON_MEMBER_TextFont, fontObj, _allocator);

	JsonObject styleObj;
	m_textStyle.addToJsonObject(styleObj, _allocator);
	_object.AddMember(OT_JSON_MEMBER_TextStyle, styleObj, _allocator);
}

void ot::GraphicsTextItemCfg::setFromJsonObject(const ConstJsonObject& _object) {
	GraphicsItemCfg::setFromJsonObject(_object);

	m_text = json::getString(_object, OT_JSON_MEMBER_Text);
	m_textFont.setFromJsonObject(json::getObject(_object, OT_JSON_MEMBER_TextFont));
	m_textStyle.setFromJsonObject(json::getObject(_object, OT_JSON_MEMBER_TextStyle));
}
