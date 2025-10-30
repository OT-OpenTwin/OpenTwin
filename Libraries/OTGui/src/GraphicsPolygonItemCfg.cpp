// @otlicense

// OpenTwin header
#include "OTCore/LogDispatcher.h"
#include "OTGui/Painter2D.h"
#include "OTGui/FillPainter2D.h"
#include "OTGui/Painter2DFactory.h"
#include "OTGui/GraphicsPolygonItemCfg.h"
#include "OTGui/GraphicsItemCfgFactory.h"

static ot::GraphicsItemCfgFactoryRegistrar<ot::GraphicsPolygonItemCfg> polyItemCfg(ot::GraphicsPolygonItemCfg::className());

ot::GraphicsPolygonItemCfg::GraphicsPolygonItemCfg()
	: m_fillPolygon(false)
{
	m_backgroundPainter = new FillPainter2D(Transparent);
}

ot::GraphicsPolygonItemCfg::GraphicsPolygonItemCfg(const GraphicsPolygonItemCfg& _other) 
	: ot::GraphicsItemCfg(_other), m_fillPolygon(_other.m_fillPolygon), m_points(_other.m_points), m_outline(_other.m_outline)
{
	if (_other.m_backgroundPainter) {
		m_backgroundPainter = _other.m_backgroundPainter->createCopy();
	}
	else {
		m_backgroundPainter = new FillPainter2D(Transparent);
	}
}

ot::GraphicsPolygonItemCfg::~GraphicsPolygonItemCfg() {
	if (m_backgroundPainter) delete m_backgroundPainter;
}

void ot::GraphicsPolygonItemCfg::addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const {
	GraphicsItemCfg::addToJsonObject(_object, _allocator);

	JsonArray ptArr;
	for (const Point2DD& pt : m_points) {
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
	_object.AddMember("Fill", m_fillPolygon, _allocator);
}

void ot::GraphicsPolygonItemCfg::setFromJsonObject(const ConstJsonObject& _object) {
	GraphicsItemCfg::setFromJsonObject(_object);

	m_points.clear();
	ConstJsonArray ptArr = json::getArray(_object, "Points");
	for (JsonSizeType i = 0; i < ptArr.Size(); i++) {
		ConstJsonObject ptObj = json::getObject(ptArr, i);
		Point2DD newPoint;
		newPoint.setFromJsonObject(ptObj);
		m_points.push_back(newPoint);
	}

	ConstJsonObject backgroundObj = json::getObject(_object, "Background");
	Painter2D* backPainter = Painter2DFactory::create(backgroundObj);
	if (backPainter) {
		this->setBackgroundPainter(backPainter);
	}

	m_outline.setFromJsonObject(json::getObject(_object, "Outline"));
	m_fillPolygon = json::getBool(_object, "Fill");
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
