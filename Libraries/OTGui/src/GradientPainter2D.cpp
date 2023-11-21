//! @file GradientPainter2D.cpp
//! @author Alexander Kuester (alexk95)
//! @date November 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/rJSONHelper.h"
#include "OTGui/GradientPainter2D.h"

#define OT_JSON_MEMBER_Stops "Stops"
#define OT_JSON_MEMBER_Spread "Spread"

ot::GradientPainter2D::GradientPainter2D()
	: m_spread(PadSpread)
{}

ot::GradientPainter2D::GradientPainter2D(const std::vector<GradientPainterStop2D>& _stops)
	: m_spread(PadSpread), m_stops(_stops)
{}

ot::GradientPainter2D::~GradientPainter2D() {}

void ot::GradientPainter2D::addToJsonObject(OT_rJSON_doc& _document, OT_rJSON_val& _object) const {
	ot::Painter2D::addToJsonObject(_document, _object);
	OTAssert(!m_stops.empty(), "Exporting empty linear gradient painter 2D");
	OT_rJSON_createValueArray(stepArr);
	for (auto s : m_stops) {
		OT_rJSON_createValueObject(stepObj);
		s.addToJsonObject(_document, stepObj);
		stepArr.PushBack(stepObj, _document.GetAllocator());
	}
	ot::rJSON::add(_document, _object, OT_JSON_MEMBER_Stops, stepArr);
	ot::rJSON::add(_document, _object, OT_JSON_MEMBER_Spread, ot::toString(m_spread));
}

void ot::GradientPainter2D::setFromJsonObject(OT_rJSON_val& _object) {
	ot::Painter2D::setFromJsonObject(_object);
	OT_rJSON_checkMember(_object, OT_JSON_MEMBER_Stops, Array);
	OT_rJSON_checkMember(_object, OT_JSON_MEMBER_Spread, String);

	OT_rJSON_val stepArr = _object[OT_JSON_MEMBER_Stops].GetArray();
	m_stops.clear();
	for (rapidjson::SizeType i = 0; i < stepArr.Size(); i++) {
		OT_rJSON_checkArrayEntryType(stepArr, i, Object);
		OT_rJSON_val stepObj = stepArr[i].GetObject();
		GradientPainterStop2D newStep;
		newStep.setFromJsonObject(stepObj);
		m_stops.push_back(newStep);
	}

	m_spread = ot::stringToGradientSpread(_object[OT_JSON_MEMBER_Spread].GetString());
}

void ot::GradientPainter2D::addStop(const GradientPainterStop2D& _stop) {
	m_stops.push_back(_stop);
}

void ot::GradientPainter2D::addStops(const std::vector<GradientPainterStop2D>& _stops) {
	for (auto s : _stops) m_stops.push_back(s);
}