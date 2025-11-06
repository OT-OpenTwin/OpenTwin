// @otlicense
// File: LinearGradientPainter2D.cpp
// 
// License:
// Copyright 2025 by OpenTwin
//  
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
//  
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// @otlicense-end

// OpenTwin header
#include "OTCore/String.h"
#include "OTGui/Painter2DFactory.h"
#include "OTGui/LinearGradientPainter2D.h"

// std header
#include <sstream>

#define OT_JSON_MEMBER_Start "Start"
#define OT_JSON_MEMBER_FinalStop "FinalStop"

static ot::Painter2DFactoryRegistrar<ot::LinearGradientPainter2D> linGradCfgRegistrar(OT_FactoryKey_LinearGradientPainter2D);

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

std::string ot::LinearGradientPainter2D::generateQss() const {
	std::string ret = "qlineargradient(x1: " + std::to_string(m_start.x()) +
		", y1: " + std::to_string(m_start.y()) +
		", x2: " + std::to_string(m_finalStop.x()) +
		", y2: " + std::to_string(m_finalStop.y());

	this->addStopsAndSpreadToQss(ret);
	ret.append(")");

	return ret;
}

std::string ot::LinearGradientPainter2D::generateSvgColorString(const std::string& _id) const {
	std::ostringstream ss;
	ss << "<linearGradient id=\"" << _id
		<< "\" x1=\"" << m_start.x()
		<< "\" y1=\"" << m_start.y()
		<< "\" x2=\"" << m_finalStop.x()
		<< "\" y2=\"" << m_finalStop.y()
		<< "\" gradientUnits=\"objectBoundingBox\"";

	// Handle spread method if applicable
	switch (getSpread()) {
	case GradientSpread::Pad:   ss << " spreadMethod=\"pad\""; break;
	case GradientSpread::Reflect: ss << " spreadMethod=\"reflect\""; break;
	case GradientSpread::Repeat: ss << " spreadMethod=\"repeat\""; break;
	default: 
		OT_LOG_E("Unknown gradient spread method (" + std::to_string(static_cast<int>(getSpread())) + ")");
		break;
	}

	ss << ">";

	// Add color stops
	for (const auto& stop : getStops()) {
		const Color& c = stop.getColor();
		ss << "<stop offset=\"" << stop.getPos() * 100.
			<< "%\" stop-color=\"#"
			<< String::numberToHexString(stop.getColor().r(), '0', 2)
			<< String::numberToHexString(stop.getColor().g(), '0', 2)
			<< String::numberToHexString(stop.getColor().b(), '0', 2)
			<< "\" stop-opacity=\""
			<< (ColorF::channelValueFromInt(stop.getColor().a())) << "\"/>";
	}

	ss << "</linearGradient>";
	return ss.str();
}

bool ot::LinearGradientPainter2D::isEqualTo(const Painter2D* _other) const {
	if (!GradientPainter2D::isEqualTo(_other)) return false;

	const LinearGradientPainter2D* otherPainter = dynamic_cast<const LinearGradientPainter2D*>(_other);
	if (!otherPainter) return false;

	if (m_start != otherPainter->getStart()) return false;
	return m_finalStop == otherPainter->getFinalStop();
}
