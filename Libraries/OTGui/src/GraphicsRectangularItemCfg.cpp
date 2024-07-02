//! @file GraphicsRectangularItemCfg.cpp
//! 
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTGui/Painter2D.h"
#include "OTGui/FillPainter2D.h"
#include "OTGui/Painter2DFactory.h"
#include "OTGui/GraphicsItemCfgFactory.h"
#include "OTGui/GraphicsRectangularItemCfg.h"

#define OT_JSON_MEMBER_Size "Size"
#define OT_JSON_MEMBER_Outline "Outline"
#define OT_JSON_MEMBER_CornerRadius "CornerRadius"
#define OT_JSON_MEMBER_BackgroundPainter "BackgroundPainter"

static ot::GraphicsItemCfgFactoryRegistrar<ot::GraphicsRectangularItemCfg> rectItemCfg(OT_FactoryKey_GraphicsRectangularItem);

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

ot::GraphicsItemCfg* ot::GraphicsRectangularItemCfg::createCopy(void) const {
	ot::GraphicsRectangularItemCfg* copy = new GraphicsRectangularItemCfg;
	this->copyConfigDataToItem(copy);

	copy->m_cornerRadius = m_cornerRadius;
	copy->m_outline = m_outline;
	copy->m_size = m_size;
	copy->setBackgroundPainer(m_backgroundPainter->createCopy());

	return copy;
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

	JsonObject outlineObj;
	m_outline.addToJsonObject(outlineObj, _allocator);
	_object.AddMember(OT_JSON_MEMBER_Outline, outlineObj, _allocator);
}

void ot::GraphicsRectangularItemCfg::setFromJsonObject(const ConstJsonObject& _object) {
	GraphicsItemCfg::setFromJsonObject(_object);

	m_cornerRadius = json::getInt(_object, OT_JSON_MEMBER_CornerRadius);

	ConstJsonObject backgroundPainterObj = json::getObject(_object, OT_JSON_MEMBER_BackgroundPainter);
	ot::Painter2D* p = Painter2DFactory::instance().create(backgroundPainterObj);
	if (p) {
		this->setBackgroundPainer(p);
	}

	m_outline.setFromJsonObject(json::getObject(_object, OT_JSON_MEMBER_Outline));
	m_size.setFromJsonObject(json::getObject(_object, OT_JSON_MEMBER_Size));
}

void ot::GraphicsRectangularItemCfg::setBackgroundPainer(ot::Painter2D* _painter) {
	if (_painter == m_backgroundPainter) return;
	if (m_backgroundPainter) delete m_backgroundPainter;
	m_backgroundPainter = _painter;
}
