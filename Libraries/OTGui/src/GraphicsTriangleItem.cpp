//! @file GraphicsTriangleItemCfg.cpp
//! 
//! @author Alexander Kuester (alexk95)
//! @date November 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTGui/GraphicsTriangleItemCfg.h"
#include "OTGui/Painter2D.h"
#include "OTGui/FillPainter2D.h"

#define OT_JSON_MEMBER_Size "Size"
#define OT_JSON_MEMBER_Shape "Shape"
#define OT_JSON_MEMBER_Outline "Outline"
#define OT_JSON_MEMBER_Direction "Direction"
#define OT_JSON_MEMBER_BackgroundPainter "BackgroundPainter"

static ot::SimpleFactoryRegistrar<ot::GraphicsTriangleItemCfg> triaItemCfg(OT_SimpleFactoryJsonKeyValue_GraphicsTriangleItemCfg);

std::string ot::GraphicsTriangleItemCfg::triangleDirectionToString(TriangleDirection _direction) {
	switch (_direction)
	{
	case ot::GraphicsTriangleItemCfg::Left: return "Left";
	case ot::GraphicsTriangleItemCfg::Up: return "Up";
	case ot::GraphicsTriangleItemCfg::Right: return "Right";
	case ot::GraphicsTriangleItemCfg::Down: return "Down";
	default:
		OT_LOG_WA("Unknown triangle direction");
		return "Right";
	}
}

ot::GraphicsTriangleItemCfg::TriangleDirection ot::GraphicsTriangleItemCfg::stringToTriangleDirection(const std::string& _str) {
	if (_str == triangleDirectionToString(ot::GraphicsTriangleItemCfg::Left)) return ot::GraphicsTriangleItemCfg::Left;
	else if (_str == triangleDirectionToString(ot::GraphicsTriangleItemCfg::Up)) return ot::GraphicsTriangleItemCfg::Up;
	else if (_str == triangleDirectionToString(ot::GraphicsTriangleItemCfg::Right)) return ot::GraphicsTriangleItemCfg::Right;
	else if (_str == triangleDirectionToString(ot::GraphicsTriangleItemCfg::Down)) return ot::GraphicsTriangleItemCfg::Down;
	else {
		OT_LOG_WAS("Unknown triangle direction \"" + _str + "\"");
		return ot::GraphicsTriangleItemCfg::Right;
	}
}

std::string ot::GraphicsTriangleItemCfg::triangleShapeToString(TriangleShape _shape) {
	switch (_shape)
	{
	case ot::GraphicsTriangleItemCfg::Triangle: return "Triangle";
	case ot::GraphicsTriangleItemCfg::Kite: return "Kite";
	case ot::GraphicsTriangleItemCfg::IceCone: return "IceCone";
	default:
		OT_LOG_WA("Unknown triangle shape");
		return "Triangle";
	}
}

ot::GraphicsTriangleItemCfg::TriangleShape ot::GraphicsTriangleItemCfg::stringToTriangleShape(const std::string& _shape) {
	if (_shape == triangleShapeToString(GraphicsTriangleItemCfg::Triangle)) return GraphicsTriangleItemCfg::Triangle;
	else if (_shape == triangleShapeToString(GraphicsTriangleItemCfg::Kite)) return GraphicsTriangleItemCfg::Kite;
	else if (_shape == triangleShapeToString(GraphicsTriangleItemCfg::IceCone)) return GraphicsTriangleItemCfg::IceCone;
	else {
		OT_LOG_WA("Unknown triangle shape");
		return GraphicsTriangleItemCfg::Triangle;
	}
}

ot::GraphicsTriangleItemCfg::GraphicsTriangleItemCfg(TriangleDirection _direction, TriangleShape _shape)
	: m_direction(_direction), m_shape(_shape)
{
	m_backgroundPainter = new ot::FillPainter2D(ot::Color(0, 0, 0, 0));
}

ot::GraphicsTriangleItemCfg::~GraphicsTriangleItemCfg() {
	if (m_backgroundPainter) delete m_backgroundPainter;
}

void ot::GraphicsTriangleItemCfg::addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const {
	OTAssertNullptr(m_backgroundPainter);

	GraphicsItemCfg::addToJsonObject(_object, _allocator);

	JsonObject sizeObj;
	m_size.addToJsonObject(sizeObj, _allocator);
	_object.AddMember(OT_JSON_MEMBER_Size, sizeObj, _allocator);

	JsonObject backgroundPainterObj;
	m_backgroundPainter->addToJsonObject(backgroundPainterObj, _allocator);
	_object.AddMember(OT_JSON_MEMBER_BackgroundPainter, backgroundPainterObj, _allocator);

	JsonObject outlineObj;
	m_outline.addToJsonObject(outlineObj, _allocator);
	_object.AddMember(OT_JSON_MEMBER_Outline, outlineObj, _allocator);

	_object.AddMember(OT_JSON_MEMBER_Shape, JsonString(this->triangleShapeToString(m_shape), _allocator), _allocator);
	_object.AddMember(OT_JSON_MEMBER_Direction, JsonString(this->triangleDirectionToString(m_direction), _allocator), _allocator);
}

void ot::GraphicsTriangleItemCfg::setFromJsonObject(const ConstJsonObject& _object) {
	GraphicsItemCfg::setFromJsonObject(_object);

	m_shape = this->stringToTriangleShape(json::getString(_object, OT_JSON_MEMBER_Shape));
	m_direction = this->stringToTriangleDirection(json::getString(_object, OT_JSON_MEMBER_Direction));

	ConstJsonObject backgroundPainterObj = json::getObject(_object, OT_JSON_MEMBER_BackgroundPainter);
	ot::Painter2D* p = ot::SimpleFactory::instance().createType<ot::Painter2D>(backgroundPainterObj);
	p->setFromJsonObject(backgroundPainterObj);
	this->setBackgroundPainer(p);

	m_outline.setFromJsonObject(json::getObject(_object, OT_JSON_MEMBER_Outline));
	m_size.setFromJsonObject(json::getObject(_object, OT_JSON_MEMBER_Size));
}

void ot::GraphicsTriangleItemCfg::setBackgroundPainer(ot::Painter2D* _painter) {
	if (_painter == m_backgroundPainter) return;
	if (m_backgroundPainter) delete m_backgroundPainter;
	m_backgroundPainter = _painter;
}
