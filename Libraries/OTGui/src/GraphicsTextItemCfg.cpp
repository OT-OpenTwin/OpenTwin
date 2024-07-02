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

static ot::GraphicsItemCfgFactoryRegistrar<ot::GraphicsTextItemCfg> textItemCfg(OT_FactoryKey_GraphicsTextItem);

ot::GraphicsTextItemCfg::GraphicsTextItemCfg(const std::string& _text, const ot::Color& _textColor)
	: m_text(_text), m_textIsReference(false)
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
	copy->m_textIsReference = m_textIsReference;

	return copy;
}

void ot::GraphicsTextItemCfg::addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const {
	GraphicsItemCfg::addToJsonObject(_object, _allocator);

	_object.AddMember("Text", JsonString(m_text, _allocator), _allocator);
	_object.AddMember("IsReference", m_textIsReference, _allocator);
	
	JsonObject fontObj;
	m_textFont.addToJsonObject(fontObj, _allocator);
	_object.AddMember("Font", fontObj, _allocator);

	JsonObject styleObj;
	m_textStyle.addToJsonObject(styleObj, _allocator);
	_object.AddMember("Style", styleObj, _allocator);
}

void ot::GraphicsTextItemCfg::setFromJsonObject(const ConstJsonObject& _object) {
	GraphicsItemCfg::setFromJsonObject(_object);

	m_text = json::getString(_object, "Text");
	m_textFont.setFromJsonObject(json::getObject(_object, "Font"));
	m_textStyle.setFromJsonObject(json::getObject(_object, "Style"));
	m_textIsReference = json::getBool(_object, "IsReference");
}
