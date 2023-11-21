//! @file LinearGradientPainter2D.cpp
//! @author Alexander Kuester (alexk95)
//! @date March 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/rJSONHelper.h"
#include "OTGui/LinearGradientPainter2D.h"

#define OT_JSON_MEMBER_Pos "Pos"
#define OT_JSON_MEMBER_Color "Color"
#define OT_JSON_MEMBER_Start "Start"
#define OT_JSON_MEMBER_Stops "Stops"
#define OT_JSON_MEMBER_Spread "Spread"
#define OT_JSON_MEMBER_FinalStop "FinalStop"

static ot::SimpleFactoryRegistrar<ot::LinearGradientPainter2D> linGradCfg(OT_SimpleFactoryJsonKeyValue_LinearGradientPainter2DCfg);

ot::LinearGradientPainter2DStop::LinearGradientPainter2DStop()
	: m_pos(0.) {}

ot::LinearGradientPainter2DStop::LinearGradientPainter2DStop(double _pos, const ot::Color& _color)
	: m_pos(_pos), m_color(_color) {}

ot::LinearGradientPainter2DStop::LinearGradientPainter2DStop(const LinearGradientPainter2DStop& _other)
	: m_pos(_other.m_pos), m_color(_other.m_color) {}

ot::LinearGradientPainter2DStop::~LinearGradientPainter2DStop() {}

ot::LinearGradientPainter2DStop& ot::LinearGradientPainter2DStop::operator = (const LinearGradientPainter2DStop& _other) {
	m_pos = _other.m_pos;
	m_color = _other.m_color;

	return *this;
}

bool ot::LinearGradientPainter2DStop::operator == (const LinearGradientPainter2DStop& _other) const {
	return m_pos == _other.m_pos && m_color == _other.m_color;
}

bool ot::LinearGradientPainter2DStop::operator != (const LinearGradientPainter2DStop& _other) const {
	return m_pos == _other.m_pos || m_color != _other.m_color;
}

void ot::LinearGradientPainter2DStop::addToJsonObject(OT_rJSON_doc& _document, OT_rJSON_val& _object) const {
	ot::rJSON::add(_document, _object, OT_JSON_MEMBER_Pos, m_pos);
	OT_rJSON_createValueObject(colorObj);
	m_color.addToJsonObject(_document, colorObj);
	ot::rJSON::add(_document, _object, OT_JSON_MEMBER_Color, colorObj);
}

void ot::LinearGradientPainter2DStop::setFromJsonObject(OT_rJSON_val& _object) {
	OT_rJSON_checkMember(_object, OT_JSON_MEMBER_Pos, Double);
	OT_rJSON_checkMember(_object, OT_JSON_MEMBER_Color, Object);
	m_pos = _object[OT_JSON_MEMBER_Pos].GetDouble();
	OT_rJSON_val colorObj = _object[OT_JSON_MEMBER_Color].GetObject();
	m_color.setFromJsonObject(colorObj);
}

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

ot::LinearGradientPainter2D::LinearGradientPainter2D()
	: m_start(0., 0.), m_finalStop(1., 1.), m_spread(PadSpread) 
{}

ot::LinearGradientPainter2D::LinearGradientPainter2D(const std::vector<LinearGradientPainter2DStop>& _stops)
	: m_start(0., 0.), m_finalStop(1., 1.), m_spread(PadSpread), m_stops(_stops) 
{}

ot::LinearGradientPainter2D::~LinearGradientPainter2D() {}

void ot::LinearGradientPainter2D::addToJsonObject(OT_rJSON_doc& _document, OT_rJSON_val& _object) const {
	ot::Painter2D::addToJsonObject(_document, _object);
	OTAssert(!m_stops.empty(), "Exporting empty linear gradient painter 2D");
	OT_rJSON_createValueArray(stepArr);
	for (auto s : m_stops) {
		OT_rJSON_createValueObject(stepObj);
		s.addToJsonObject(_document, stepObj);
		stepArr.PushBack(stepObj, _document.GetAllocator());
	}
	ot::rJSON::add(_document, _object, OT_JSON_MEMBER_Stops, stepArr);

	OT_rJSON_createValueObject(startObj);
	m_start.addToJsonObject(_document, startObj);
	ot::rJSON::add(_document, _object, OT_JSON_MEMBER_Start, startObj);

	OT_rJSON_createValueObject(finalStopObj);
	m_finalStop.addToJsonObject(_document, finalStopObj);
	ot::rJSON::add(_document, _object, OT_JSON_MEMBER_FinalStop, finalStopObj);

	ot::rJSON::add(_document, _object, OT_JSON_MEMBER_Spread, ot::toString(m_spread));
}

void ot::LinearGradientPainter2D::setFromJsonObject(OT_rJSON_val& _object) {
	ot::Painter2D::setFromJsonObject(_object);
	OT_rJSON_checkMember(_object, OT_JSON_MEMBER_Stops, Array);
	OT_rJSON_checkMember(_object, OT_JSON_MEMBER_Start, Object);
	OT_rJSON_checkMember(_object, OT_JSON_MEMBER_FinalStop, Object);
	OT_rJSON_checkMember(_object, OT_JSON_MEMBER_Spread, String);

	OT_rJSON_val stepArr = _object[OT_JSON_MEMBER_Stops].GetArray();
	m_stops.clear();
	for (rapidjson::SizeType i = 0; i < stepArr.Size(); i++) {
		OT_rJSON_checkArrayEntryType(stepArr, i, Object);
		OT_rJSON_val stepObj = stepArr[i].GetObject();
		LinearGradientPainter2DStop newStep;
		newStep.setFromJsonObject(stepObj);
		m_stops.push_back(newStep);
	}

	OT_rJSON_val startObj = _object[OT_JSON_MEMBER_Start].GetObject();
	m_start.setFromJsonObject(startObj);

	OT_rJSON_val finalStopObj = _object[OT_JSON_MEMBER_FinalStop].GetObject();
	m_finalStop.setFromJsonObject(finalStopObj);

	m_spread = ot::stringToGradientSpread(_object[OT_JSON_MEMBER_Spread].GetString());
}

void ot::LinearGradientPainter2D::addStop(const LinearGradientPainter2DStop& _stop) {
	m_stops.push_back(_stop);
}

void ot::LinearGradientPainter2D::addStops(const std::vector<LinearGradientPainter2DStop>& _stops) {
	for (auto s : _stops) m_stops.push_back(s);
}