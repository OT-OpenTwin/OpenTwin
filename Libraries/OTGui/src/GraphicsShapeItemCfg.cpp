// @otlicense

// OpenTwin header
#include "OTCore/LogDispatcher.h"
#include "OTGui/Painter2D.h"
#include "OTGui/FillPainter2D.h"
#include "OTGui/Painter2DFactory.h"
#include "OTGui/GraphicsShapeItemCfg.h"
#include "OTGui/GraphicsItemCfgFactory.h"

static ot::GraphicsItemCfgFactoryRegistrar<ot::GraphicsShapeItemCfg> polyItemCfg(ot::GraphicsShapeItemCfg::className());

ot::GraphicsShapeItemCfg::GraphicsShapeItemCfg()
{
	m_backgroundPainter = new FillPainter2D(Transparent);
}

ot::GraphicsShapeItemCfg::GraphicsShapeItemCfg(const GraphicsShapeItemCfg& _other) 
	: ot::GraphicsItemCfg(_other), m_path(_other.m_path), m_outline(_other.m_outline), m_fillShape(_other.m_fillShape)
{
	if (_other.m_backgroundPainter) {
		m_backgroundPainter = _other.m_backgroundPainter->createCopy();
	}
	else {
		m_backgroundPainter = new FillPainter2D(Transparent);
	}
}

ot::GraphicsShapeItemCfg::~GraphicsShapeItemCfg() {
	if (m_backgroundPainter) delete m_backgroundPainter;
}

void ot::GraphicsShapeItemCfg::addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const {
	GraphicsItemCfg::addToJsonObject(_object, _allocator);

	JsonObject pathObj;
	m_path.addToJsonObject(pathObj, _allocator);
	_object.AddMember("Path", pathObj, _allocator);

	JsonObject backgroundObj;
	m_backgroundPainter->addToJsonObject(backgroundObj, _allocator);
	_object.AddMember("Background", backgroundObj, _allocator);

	JsonObject outlineObj;
	m_outline.addToJsonObject(outlineObj, _allocator);
	_object.AddMember("Outline", outlineObj, _allocator);

	_object.AddMember("Fill", m_fillShape, _allocator);
}

void ot::GraphicsShapeItemCfg::setFromJsonObject(const ConstJsonObject& _object) {
	GraphicsItemCfg::setFromJsonObject(_object);

	m_path.setFromJsonObject(json::getObject(_object, "Path"));

	ConstJsonObject backgroundObj = json::getObject(_object, "Background");
	Painter2D* backPainter = Painter2DFactory::create(backgroundObj);
	if (backPainter) {
		this->setBackgroundPainter(backPainter);
	}

	m_outline.setFromJsonObject(json::getObject(_object, "Outline"));

	m_fillShape = json::getBool(_object, "Fill");
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
