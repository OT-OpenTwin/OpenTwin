//! @file LinearGradientPainter2D.cpp
//! @author Alexander Kuester (alexk95)
//! @date March 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTGui/Painter2DFactory.h"
#include "OTGui/LinearGradientPainter2D.h"

#define OT_JSON_MEMBER_Start "Start"
#define OT_JSON_MEMBER_FinalStop "FinalStop"

static ot::SimpleFactoryRegistrar<ot::LinearGradientPainter2D> linGradCfg(OT_SimpleFactoryJsonKeyValue_LinearGradientPainter2DCfg);
static ot::Painter2DFactoryRegistrar<ot::LinearGradientPainter2D> linGradCfgRegistrar(OT_SimpleFactoryJsonKeyValue_LinearGradientPainter2DCfg);

ot::LinearGradientPainter2D::LinearGradientPainter2D()
	: m_start(0., 0.), m_finalStop(1., 1.) 
{}

ot::LinearGradientPainter2D::LinearGradientPainter2D(const std::vector<GradientPainterStop2D>& _stops)
	: m_start(0., 0.), m_finalStop(1., 1.)
{}

ot::LinearGradientPainter2D::~LinearGradientPainter2D() {}

void ot::LinearGradientPainter2D::addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const {
	ot::GradientPainter2D::addToJsonObject(_object, _allocator);

	JsonObject startObj;
	m_start.addToJsonObject(startObj, _allocator);
	_object.AddMember(OT_JSON_MEMBER_Start, startObj, _allocator);

	JsonObject finalStopObj;
	m_finalStop.addToJsonObject(finalStopObj, _allocator);
	_object.AddMember(OT_JSON_MEMBER_FinalStop, finalStopObj, _allocator);
}

void ot::LinearGradientPainter2D::setFromJsonObject(const ConstJsonObject& _object) {
	ot::GradientPainter2D::setFromJsonObject(_object);
	m_start.setFromJsonObject(json::getObject(_object, OT_JSON_MEMBER_Start));
	m_finalStop.setFromJsonObject(json::getObject(_object, OT_JSON_MEMBER_FinalStop));
}

std::string ot::LinearGradientPainter2D::generateQss(void) const {
	std::string ret = "qlineargradient(x1: " + std::to_string(m_start.x()) +
		", y1: " + std::to_string(m_start.y()) +
		", x2: " + std::to_string(m_finalStop.x()) +
		", y2: " + std::to_string(m_finalStop.y());

	this->addStopsAndSpreadToQss(ret);
	ret.append(")");

	return ret;
}

ot::Color ot::LinearGradientPainter2D::getDefaultColor(void) const {
	if (this->stops().empty()) return Color();
	else return this->stops().front().color();
}