//! @file RadialGradientPainter2D.cpp
//! @author Alexander Kuester (alexk95)
//! @date November 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/rJSONHelper.h"
#include "OTGui/RadialGradientPainter2D.h"

#define OT_JSON_MEMBER_CenterPoint "Center.Point"
#define OT_JSON_MEMBER_CenterRadius "Center.Radius"
#define OT_JSON_MEMBER_FocalPoint "Focal.Point"
#define OT_JSON_MEMBER_FocalRadius "Focal.Radius"
#define OT_JSON_MEMBER_FocalIsSet "Focal.IsSet"

static ot::SimpleFactoryRegistrar<ot::RadialGradientPainter2D> linGradCfg(OT_SimpleFactoryJsonKeyValue_RadialGradientPainter2DCfg);

ot::RadialGradientPainter2D::RadialGradientPainter2D()
	: m_centerRadius(1.), m_focalRadius(1.), m_focalSet(false)
{}

ot::RadialGradientPainter2D::~RadialGradientPainter2D() {}

void ot::RadialGradientPainter2D::addToJsonObject(OT_rJSON_doc& _document, OT_rJSON_val& _object) const {
	ot::GradientPainter2D::addToJsonObject(_document, _object);

	OT_rJSON_createValueObject(centerObj);
	m_center.addToJsonObject(_document, centerObj);
	ot::rJSON::add(_document, _object, OT_JSON_MEMBER_CenterPoint, centerObj);
	ot::rJSON::add(_document, _object, OT_JSON_MEMBER_CenterRadius, m_centerRadius);

	OT_rJSON_createValueObject(focalObj);
	m_center.addToJsonObject(_document, focalObj);
	ot::rJSON::add(_document, _object, OT_JSON_MEMBER_FocalPoint, focalObj);
	ot::rJSON::add(_document, _object, OT_JSON_MEMBER_FocalRadius, m_centerRadius);
	ot::rJSON::add(_document, _object, OT_JSON_MEMBER_FocalIsSet, m_focalSet);
}

void ot::RadialGradientPainter2D::setFromJsonObject(OT_rJSON_val& _object) {
	ot::GradientPainter2D::setFromJsonObject(_object);
	OT_rJSON_checkMember(_object, OT_JSON_MEMBER_CenterPoint, Object);
	OT_rJSON_checkMember(_object, OT_JSON_MEMBER_CenterRadius, Double);
	OT_rJSON_checkMember(_object, OT_JSON_MEMBER_FocalPoint, Object);
	OT_rJSON_checkMember(_object, OT_JSON_MEMBER_FocalRadius, Double);
	OT_rJSON_checkMember(_object, OT_JSON_MEMBER_FocalIsSet, Bool);

	m_centerRadius = _object[OT_JSON_MEMBER_CenterRadius].GetDouble();
	OT_rJSON_val centerObj = _object[OT_JSON_MEMBER_CenterPoint].GetObject();
	m_center.setFromJsonObject(centerObj);

	m_focalSet = _object[OT_JSON_MEMBER_FocalIsSet].GetBool();
	m_focalRadius = _object[OT_JSON_MEMBER_FocalRadius].GetDouble();
	OT_rJSON_val finalStopObj = _object[OT_JSON_MEMBER_FocalPoint].GetObject();
	m_focal.setFromJsonObject(finalStopObj);
}

void ot::RadialGradientPainter2D::setFocalPoint(const ot::Point2DD& _focal) {
	m_focal = _focal;
	m_focalSet = true;
}
