//! @file GraphicsShapeItemCfg.cpp
//! 
//! @author Alexander Kuester (alexk95)
//! @date May 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTGui/GraphicsShapeItemCfg.h"
#include "OTGui/Painter2D.h"
#include "OTGui/FillPainter2D.h"

static ot::SimpleFactoryRegistrar<ot::GraphicsShapeItemCfg> polyItemCfg(OT_SimpleFactoryJsonKeyValue_GraphicsShapeItemCfg);

ot::GraphicsShapeItemCfg::GraphicsShapeItemCfg() : m_outlineWidth(1.)
{
	m_backgroundPainter = new FillPainter2D(Transparent);
	m_outlinePainter = new FillPainter2D(Black);
}

ot::GraphicsShapeItemCfg::~GraphicsShapeItemCfg() {

}

void ot::GraphicsShapeItemCfg::addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const {
	GraphicsItemCfg::addToJsonObject(_object, _allocator);

	JsonObject pathObj;
	m_path.addToJsonObject(pathObj, _allocator);
	_object.AddMember("Path", pathObj, _allocator);
}

void ot::GraphicsShapeItemCfg::setFromJsonObject(const ConstJsonObject& _object) {
	GraphicsItemCfg::setFromJsonObject(_object);

	m_path.setFromJsonObject(json::getObject(_object, "Path"));
}

// ###########################################################################################################################################################################################################################################################################################################################

// Setter / Getter

void ot::GraphicsShapeItemCfg::setBackgroundPainter(Painter2D* _painter) {
	if (m_backgroundPainter == _painter) return;
	if (!_painter) {
		OT_LOG_E("Nullptr provided.");
		return;
	}
	if (m_backgroundPainter) delete m_backgroundPainter;
	m_backgroundPainter = _painter;
}

ot::Painter2D* ot::GraphicsShapeItemCfg::takeBackgroundPainter(void) {
	ot::Painter2D* ret = m_backgroundPainter;
	m_backgroundPainter = new FillPainter2D(Transparent);
	return ret;
}

void ot::GraphicsShapeItemCfg::setOutlinePainter(Painter2D* _painter) {
	if (m_outlinePainter == _painter) return;
	if (!_painter) {
		OT_LOG_E("Nullptr provided.");
		return;
	}
	if (m_outlinePainter) delete m_outlinePainter;
	m_outlinePainter = _painter;
}

ot::Painter2D* ot::GraphicsShapeItemCfg::takeOutlinePainter(void) {
	ot::Painter2D* ret = m_outlinePainter;
	m_outlinePainter = new FillPainter2D(Transparent);
	return ret;
}