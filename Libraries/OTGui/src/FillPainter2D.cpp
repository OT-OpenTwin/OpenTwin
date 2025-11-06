// @otlicense
// File: FillPainter2D.cpp
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
#include "OTGui/FillPainter2D.h"
#include "OTGui/Painter2DFactory.h"

// std header
#include <sstream>

static ot::Painter2DFactoryRegistrar<ot::FillPainter2D> fillCfgRegistrar(OT_FactoryKey_FillPainter2D);

ot::FillPainter2D::FillPainter2D() {}

ot::FillPainter2D::FillPainter2D(ot::DefaultColor _color) : m_color(_color) {}

ot::FillPainter2D::FillPainter2D(int _r, int _g, int _b, int _a) : m_color(_r, _g, _b, _a) {}

ot::FillPainter2D::FillPainter2D(const ot::Color& _color) : m_color(_color) {}

ot::FillPainter2D::~FillPainter2D() {}

void ot::FillPainter2D::addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const {
	ot::Painter2D::addToJsonObject(_object, _allocator);
	JsonObject colorObj;
	m_color.addToJsonObject(colorObj, _allocator);
	_object.AddMember("Color", colorObj, _allocator);
}

void ot::FillPainter2D::setFromJsonObject(const ConstJsonObject& _object) {
	ot::Painter2D::setFromJsonObject(_object);
	m_color.setFromJsonObject(json::getObject(_object, "Color"));
}

std::string ot::FillPainter2D::generateQss(void) const {
	return "rgba(" + std::to_string(m_color.r()) + "," + std::to_string(m_color.g()) + "," + std::to_string(m_color.b()) + "," + std::to_string(m_color.a()) + ")";
}

std::string ot::FillPainter2D::generateSvgColorString(const std::string& _id) const {
	std::ostringstream ss;
	ss << "<linearGradient id=\"" << _id
		<< "\" x1=\"" << 0
		<< "\" y1=\"" << 0
		<< "\" x2=\"" << 1
		<< "\" y2=\"" << 1
		<< "\" gradientUnits=\"objectBoundingBox\""
	    << ">"
		
		// We only specify one color since it's a fill
		<< "<stop offset=\"0%\" stop-color=\"#"
		<< String::numberToHexString(m_color.r(), '0', 2)
		<< String::numberToHexString(m_color.g(), '0', 2)
		<< String::numberToHexString(m_color.b(), '0', 2)
		<< "\" stop-opacity=\""
		<< (ColorF::channelValueFromInt(m_color.a())) << "\"/>"
		<< "</linearGradient>";
	return ss.str();
}

ot::Color ot::FillPainter2D::getDefaultColor(void) const {
	return m_color;
}

bool ot::FillPainter2D::isEqualTo(const Painter2D* _other) const {
	const FillPainter2D* otherPainter = dynamic_cast<const FillPainter2D*>(_other);
	if (!otherPainter) return false;

	return m_color == otherPainter->getColor();
}
