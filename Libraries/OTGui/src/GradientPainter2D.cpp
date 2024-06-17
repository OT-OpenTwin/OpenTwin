//! @file GradientPainter2D.cpp
//! @author Alexander Kuester (alexk95)
//! @date November 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/Logger.h"
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

void ot::GradientPainter2D::addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const {
	ot::Painter2D::addToJsonObject(_object, _allocator);
	OTAssert(!m_stops.empty(), "Exporting empty linear gradient painter 2D");
	JsonArray stepArr;
	for (auto s : m_stops) {
		JsonObject stepObj;
		s.addToJsonObject(stepObj, _allocator);
		stepArr.PushBack(stepObj, _allocator);
	}
	_object.AddMember(OT_JSON_MEMBER_Stops, stepArr, _allocator);
	_object.AddMember(OT_JSON_MEMBER_Spread, JsonString(toString(m_spread), _allocator), _allocator);
}

void ot::GradientPainter2D::setFromJsonObject(const ConstJsonObject& _object) {
	ot::Painter2D::setFromJsonObject(_object);
	
	std::list<ConstJsonObject> stepArr = json::getObjectList(_object, OT_JSON_MEMBER_Stops);
	m_stops.clear();
	for (auto stepObj : stepArr) {
		GradientPainterStop2D newStep;
		newStep.setFromJsonObject(stepObj);
		m_stops.push_back(newStep);
	}

	m_spread = ot::stringToGradientSpread(json::getString(_object, OT_JSON_MEMBER_Spread));
}

ot::Color ot::GradientPainter2D::getDefaultColor(void) const {
	if (this->getStops().empty()) return Color();
	else return this->getStops().front().getColor();
}

bool ot::GradientPainter2D::isEqualTo(const Painter2D* _other) const {
	const GradientPainter2D* otherPainter = dynamic_cast<const GradientPainter2D*>(_other);
	if (!otherPainter) return false;

	if (m_stops.size() != otherPainter->getStops().size()) return false;
	for (size_t i = 0; i < m_stops.size(); i++) {
		if (m_stops[i] != otherPainter->getStops()[i]) return false;
	}
	return m_spread == otherPainter->getSpread();
}

void ot::GradientPainter2D::addStop(const GradientPainterStop2D& _stop) {
	m_stops.push_back(_stop);
}

void ot::GradientPainter2D::addStops(const std::vector<GradientPainterStop2D>& _stops) {
	for (auto s : _stops) m_stops.push_back(s);
}

void ot::GradientPainter2D::addStopsAndSpreadToQss(std::string& _targetString) const {
	switch (m_spread)
	{
	case ot::PadSpread: _targetString.append(", spread: pad"); break;
	case ot::RepeatSpread: _targetString.append(", spread: repeat"); break;
	case ot::ReflectSpread: _targetString.append(", spread: reflect"); break;
	default:
		OT_LOG_E("Unknown spread");
		break;
	}
	for (const GradientPainterStop2D& s : m_stops) {
		_targetString.append(", stop: " + std::to_string(s.getPos()) +
			" rgba(" + std::to_string(s.getColor().r()) +
			", " + std::to_string(s.getColor().g()) +
			", " + std::to_string(s.getColor().b()) +
			", " + std::to_string(s.getColor().a()) + ")");
	}
}