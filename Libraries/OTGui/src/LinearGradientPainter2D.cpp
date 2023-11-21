//! @file LinearGradientPainter2D.cpp
//! @author Alexander Kuester (alexk95)
//! @date March 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/rJSONHelper.h"
#include "OTGui/LinearGradientPainter2D.h"

#define OT_JSON_MEMBER_Start "Start"
#define OT_JSON_MEMBER_FinalStop "FinalStop"

static ot::SimpleFactoryRegistrar<ot::LinearGradientPainter2D> linGradCfg(OT_SimpleFactoryJsonKeyValue_LinearGradientPainter2DCfg);

ot::LinearGradientPainter2D::LinearGradientPainter2D()
	: m_start(0., 0.), m_finalStop(1., 1.) 
{}

ot::LinearGradientPainter2D::LinearGradientPainter2D(const std::vector<GradientPainterStop2D>& _stops)
	: m_start(0., 0.), m_finalStop(1., 1.)
{}

ot::LinearGradientPainter2D::~LinearGradientPainter2D() {}

void ot::LinearGradientPainter2D::addToJsonObject(OT_rJSON_doc& _document, OT_rJSON_val& _object) const {
	ot::GradientPainter2D::addToJsonObject(_document, _object);

	OT_rJSON_createValueObject(startObj);
	m_start.addToJsonObject(_document, startObj);
	ot::rJSON::add(_document, _object, OT_JSON_MEMBER_Start, startObj);

	OT_rJSON_createValueObject(finalStopObj);
	m_finalStop.addToJsonObject(_document, finalStopObj);
	ot::rJSON::add(_document, _object, OT_JSON_MEMBER_FinalStop, finalStopObj);
}

void ot::LinearGradientPainter2D::setFromJsonObject(OT_rJSON_val& _object) {
	ot::GradientPainter2D::setFromJsonObject(_object);
	OT_rJSON_checkMember(_object, OT_JSON_MEMBER_Start, Object);
	OT_rJSON_checkMember(_object, OT_JSON_MEMBER_FinalStop, Object);

	OT_rJSON_val startObj = _object[OT_JSON_MEMBER_Start].GetObject();
	m_start.setFromJsonObject(startObj);

	OT_rJSON_val finalStopObj = _object[OT_JSON_MEMBER_FinalStop].GetObject();
	m_finalStop.setFromJsonObject(finalStopObj);
}
