//! @file GraphicsPolygonItemCfg.cpp
//! 
//! @author Alexander Kuester (alexk95)
//! @date May 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTGui/GraphicsPolygonItemCfg.h"
#include "OTGui/Painter2D.h"
#include "OTGui/FillPainter2D.h"

static ot::SimpleFactoryRegistrar<ot::GraphicsPolygonItemCfg> polyItemCfg(OT_SimpleFactoryJsonKeyValue_GraphicsPolygonItemCfg);

ot::GraphicsPolygonItemCfg::GraphicsPolygonItemCfg()
{
	m_backgroundPainter = new FillPainter2D(Transparent);
}

ot::GraphicsPolygonItemCfg::~GraphicsPolygonItemCfg() {
	if (m_backgroundPainter) delete m_backgroundPainter;
}

void ot::GraphicsPolygonItemCfg::addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const {
	GraphicsItemCfg::addToJsonObject(_object, _allocator);

	JsonArray ptArr;
	for (const Point2D& pt : m_points) {
		JsonObject ptObj;
		pt.addToJsonObject(ptObj, _allocator);
		ptArr.PushBack(ptObj, _allocator);
	}
	_object.AddMember("Points", ptArr, _allocator);

	JsonObject backgroundObj;
	m_backgroundPainter->addToJsonObject(backgroundObj, _allocator);
	_object.AddMember("Background", backgroundObj, _allocator);

	JsonObject outlineObj;
	m_outline.addToJsonObject(outlineObj, _allocator);
	_object.AddMember("Outline", outlineObj, _allocator);
}

void ot::GraphicsPolygonItemCfg::setFromJsonObject(const ConstJsonObject& _object) {
	GraphicsItemCfg::setFromJsonObject(_object);

	m_points.clear();
	ConstJsonArray ptArr = json::getArray(_object, "Points");
	for (JsonSizeType i = 0; i < ptArr.Size(); i++) {
		ConstJsonObject ptObj = json::getObject(ptArr, i);
		Point2D newPoint;
		newPoint.setFromJsonObject(ptObj);
		m_points.push_back(newPoint);
	}

	ConstJsonObject backgroundObj = json::getObject(_object, "Background");
	Painter2D* backPainter = SimpleFactory::instance().createType<Painter2D>(backgroundObj);
	if (backPainter) {
		backPainter->setFromJsonObject(backgroundObj);
		this->setBackgroundPainter(backPainter);
	}

	m_outline.setFromJsonObject(json::getObject(_object, "Outline"));
}

// ###########################################################################################################################################################################################################################################################################################################################

// Setter / Getter

void ot::GraphicsPolygonItemCfg::setBackgroundPainter(Painter2D* _painter) {
	if (m_backgroundPainter == _painter) return;
	if (!_painter) {
		OT_LOG_E("Nullptr provided.");
		return;
	}
	if (m_backgroundPainter) delete m_backgroundPainter;
	m_backgroundPainter = _painter;
}

ot::Painter2D* ot::GraphicsPolygonItemCfg::takeBackgroundPainter(void) {
	ot::Painter2D* ret = m_backgroundPainter;
	m_backgroundPainter = new FillPainter2D(Transparent);
	return ret;
}
