//! @file GraphicsRectangularItemCfg.cpp
//! 
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTGui/GraphicsRectangularItemCfg.h"
#include "OTGui/Painter2D.h"
#include "OTGui/FillPainter2D.h"

#define OT_JSON_MEMBER_Size "Size"
#define OT_JSON_MEMBER_Border "Border"
#define OT_JSON_MEMBER_CornerRadius "CornerRadius"
#define OT_JSON_MEMBER_BackgroundPainter "BackgroundPainter"

static ot::SimpleFactoryRegistrar<ot::GraphicsRectangularItemCfg> rectItemCfg(OT_SimpleFactoryJsonKeyValue_GraphicsRectangularItemCfg);

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

void ot::GraphicsRectangularItemCfg::addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const {
	OTAssertNullptr(m_backgroundPainter);

	GraphicsItemCfg::addToJsonObject(_object, _allocator);

	_object.AddMember(OT_JSON_MEMBER_CornerRadius, m_cornerRadius, _allocator);

	JsonObject sizeObj;
	m_size.addToJsonObject(sizeObj, _allocator);
	_object.AddMember(OT_JSON_MEMBER_Size, sizeObj, _allocator);

	JsonObject backgroundPainterObj;
	m_backgroundPainter->addToJsonObject(backgroundPainterObj, _allocator);
	_object.AddMember(OT_JSON_MEMBER_BackgroundPainter, backgroundPainterObj, _allocator);

	JsonObject borderObj;
	m_border.addToJsonObject(borderObj, _allocator);
	_object.AddMember(OT_JSON_MEMBER_Border, borderObj, _allocator);
}

void ot::GraphicsRectangularItemCfg::setFromJsonObject(const ConstJsonObject& _object) {
	GraphicsItemCfg::setFromJsonObject(_object);

	m_cornerRadius = json::getInt(_object, OT_JSON_MEMBER_CornerRadius);

	ConstJsonObject backgroundPainterObj = json::getObject(_object, OT_JSON_MEMBER_BackgroundPainter);
	ot::Painter2D* p = ot::SimpleFactory::instance().createType<ot::Painter2D>(backgroundPainterObj);
	p->setFromJsonObject(backgroundPainterObj);
	this->setBackgroundPainer(p);

	m_border.setFromJsonObject(json::getObject(_object, OT_JSON_MEMBER_Border));
	m_size.setFromJsonObject(json::getObject(_object, OT_JSON_MEMBER_Size));
}

void ot::GraphicsRectangularItemCfg::setBackgroundPainer(ot::Painter2D* _painter) {
	if (_painter == m_backgroundPainter) return;
	if (m_backgroundPainter) delete m_backgroundPainter;
	m_backgroundPainter = _painter;
}
