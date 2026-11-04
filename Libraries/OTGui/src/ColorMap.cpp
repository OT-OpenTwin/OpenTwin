// @otlicense
// File: ColorMap.cpp
// 
// License:
// Copyright 2026 by OpenTwin
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
#include "OTGui/ColorMap.h"
#include "OTCore/Logging/Logger.h"

// std header
#include <algorithm>

std::string ot::toString(ColorMapPreset _preset) {
	switch (_preset) {
	case ColorMapPreset::Custom: return "Custom";
	case ColorMapPreset::Viridis: return "Viridis";
	case ColorMapPreset::Plasma: return "Plasma";
	case ColorMapPreset::Turbo: return "Turbo";
	case ColorMapPreset::CoolWarm: return "CoolWarm";
	case ColorMapPreset::OkabeIto: return "OkabeIto";
	case ColorMapPreset::TolVibrant: return "TolVibrant";
	case ColorMapPreset::Rainbow: return "Rainbow";
	default:
		OT_LOG_E("Unknown ColorMapPreset");
		return "Custom";
	}
}

ot::ColorMapPreset ot::stringToColorMapPreset(const std::string& _str) {
	if (_str == "Viridis") return ColorMapPreset::Viridis;
	if (_str == "Plasma") return ColorMapPreset::Plasma;
	if (_str == "Turbo") return ColorMapPreset::Turbo;
	if (_str == "CoolWarm") return ColorMapPreset::CoolWarm;
	if (_str == "OkabeIto") return ColorMapPreset::OkabeIto;
	if (_str == "TolVibrant") return ColorMapPreset::TolVibrant;
	if (_str == "Rainbow") return ColorMapPreset::Rainbow;
	return ColorMapPreset::Custom;
}

ot::ColorMap::ColorMap()
	: m_name("Viridis"),
	  m_preset(ColorMapPreset::Viridis),
	  m_interpolationSpace(ColorInterpolationSpace::Oklab)
{
	setPreset(ColorMapPreset::Viridis);
}

ot::ColorMap::ColorMap(ColorMapPreset _preset)
	: m_name(toString(_preset)),
	  m_preset(_preset),
	  m_interpolationSpace(ColorInterpolationSpace::Oklab)
{
	setPreset(_preset);
}

ot::ColorMap::ColorMap(const std::string& _name, const std::vector<ColorStop>& _stops, ColorInterpolationSpace _space)
	: m_name(_name),
	  m_preset(ColorMapPreset::Custom),
	  m_interpolationSpace(_space),
	  m_stops(_stops)
{
	sortStops();
}

bool ot::ColorMap::operator==(const ColorMap& _other) const {
	return m_name == _other.m_name &&
	       m_preset == _other.m_preset &&
	       m_interpolationSpace == _other.m_interpolationSpace &&
	       m_stops == _other.m_stops;
}

bool ot::ColorMap::operator!=(const ColorMap& _other) const {
	return !(*this == _other);
}

void ot::ColorMap::addStop(float _position, const Color& _color) {
	_position = std::max(0.0f, std::min(1.0f, _position));
	m_stops.push_back(ColorStop{ _position, _color });
	sortStops();
	m_preset = ColorMapPreset::Custom;
}

void ot::ColorMap::clearStops() {
	m_stops.clear();
	m_preset = ColorMapPreset::Custom;
}

void ot::ColorMap::setStops(const std::vector<ColorStop>& _stops) {
	m_stops = _stops;
	sortStops();
	m_preset = ColorMapPreset::Custom;
}

void ot::ColorMap::setPreset(ColorMapPreset _preset) {
	m_preset = _preset;
	m_name = toString(_preset);
	m_interpolationSpace = ColorInterpolationSpace::Oklab;
	m_stops.clear();

	switch (_preset) {
	case ColorMapPreset::Viridis:
		m_stops = {
			ColorStop{ 0.00f, Color(68, 1, 84) },
			ColorStop{ 0.25f, Color(59, 82, 139) },
			ColorStop{ 0.50f, Color(33, 145, 140) },
			ColorStop{ 0.75f, Color(94, 201, 98) },
			ColorStop{ 1.00f, Color(253, 231, 37) }
		};
		break;

	case ColorMapPreset::Plasma:
		m_stops = {
			ColorStop{ 0.00f, Color(13, 8, 135) },
			ColorStop{ 0.25f, Color(106, 0, 168) },
			ColorStop{ 0.50f, Color(177, 42, 144) },
			ColorStop{ 0.75f, Color(225, 100, 98) },
			ColorStop{ 1.00f, Color(240, 249, 33) }
		};
		break;

	case ColorMapPreset::Turbo:
		m_stops = {
			ColorStop{ 0.00f, Color(48, 18, 59) },
			ColorStop{ 0.20f, Color(70, 134, 251) },
			ColorStop{ 0.40f, Color(27, 229, 181) },
			ColorStop{ 0.60f, Color(164, 252, 60) },
			ColorStop{ 0.80f, Color(251, 155, 38) },
			ColorStop{ 1.00f, Color(122, 4, 3) }
		};
		break;

	case ColorMapPreset::CoolWarm:
		m_stops = {
			ColorStop{ 0.00f, Color(59, 76, 192) },
			ColorStop{ 0.25f, Color(144, 178, 254) },
			ColorStop{ 0.50f, Color(220, 220, 220) },
			ColorStop{ 0.75f, Color(245, 156, 129) },
			ColorStop{ 1.00f, Color(180, 4, 38) }
		};
		break;

	case ColorMapPreset::OkabeIto:
		m_stops = {
			ColorStop{ 0.000f, Color(0, 92, 92) },
			ColorStop{ 0.166f, Color(230, 159, 0) },
			ColorStop{ 0.333f, Color(86, 180, 233) },
			ColorStop{ 0.500f, Color(0, 158, 115) },
			ColorStop{ 0.666f, Color(240, 228, 66) },
			ColorStop{ 0.833f, Color(0, 114, 178) },
			ColorStop{ 1.000f, Color(213, 94, 0) }
		};
		break;

	case ColorMapPreset::TolVibrant:
		m_stops = {
			ColorStop{ 0.00f, Color(0, 119, 187) },
			ColorStop{ 0.20f, Color(51, 187, 238) },
			ColorStop{ 0.40f, Color(0, 153, 136) },
			ColorStop{ 0.60f, Color(238, 119, 51) },
			ColorStop{ 0.80f, Color(204, 51, 17) },
			ColorStop{ 1.00f, Color(238, 51, 119) }
		};
		break;

	case ColorMapPreset::Rainbow:
		m_stops = {
			ColorStop{ 0.00f, Color(0, 0, 255) },
			ColorStop{ 0.25f, Color(0, 255, 255) },
			ColorStop{ 0.50f, Color(0, 255, 0) },
			ColorStop{ 0.75f, Color(255, 255, 0) },
			ColorStop{ 1.00f, Color(255, 0, 0) }
		};
		break;

	case ColorMapPreset::Custom:
	default:
		m_stops = {
			ColorStop{ 0.00f, Color(DefaultColor::Black) },
			ColorStop{ 1.00f, Color(DefaultColor::White) }
		};
		break;
	}

	sortStops();
}

ot::ColorMap ot::ColorMap::createPreset(ColorMapPreset _preset) {
	ColorMap cm(_preset);
	return cm;
}

ot::Color ot::ColorMap::getColor(float _normalizedValue) const {
	if (m_stops.empty()) {
		return Color(DefaultColor::Black);
	}
	if (m_stops.size() == 1) {
		return m_stops.front().color;
	}

	const float clampedVal = std::max(0.0f, std::min(1.0f, _normalizedValue));

	if (clampedVal <= m_stops.front().position) {
		return m_stops.front().color;
	}
	if (clampedVal >= m_stops.back().position) {
		return m_stops.back().color;
	}

	for (size_t i = 0; i + 1 < m_stops.size(); ++i) {
		const auto& s1 = m_stops[i];
		const auto& s2 = m_stops[i + 1];

		if (clampedVal >= s1.position && clampedVal <= s2.position) {
			const float delta = s2.position - s1.position;
			if (delta <= 1e-6f) {
				return s1.color;
			}
			const float t = (clampedVal - s1.position) / delta;
			return s1.color.interpolate(s2.color, t, m_interpolationSpace);
		}
	}

	return m_stops.back().color;
}

void ot::ColorMap::sortStops() {
	std::sort(m_stops.begin(), m_stops.end(), [](const ColorStop& a, const ColorStop& b) {
		return a.position < b.position;
	});
}

void ot::ColorMap::addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const {
	_object.AddMember(JsonString(c_colorMapNameKey, _allocator), JsonString(m_name, _allocator), _allocator);
	_object.AddMember(JsonString(c_colorMapPresetKey, _allocator), JsonString(toString(m_preset), _allocator), _allocator);
	_object.AddMember(JsonString(c_colorMapInterpolationSpaceKey, _allocator), JsonValue(static_cast<int>(m_interpolationSpace)), _allocator);

	JsonArray stopsArr;
	for (const auto& stop : m_stops) {
		JsonObject stopObj;
		stopObj.AddMember(JsonString(c_colorStopPositionKey, _allocator), JsonValue(stop.position), _allocator);

		JsonObject colorObj;
		stop.color.addToJsonObject(colorObj, _allocator);
		stopObj.AddMember(JsonString(c_colorStopColorKey, _allocator), colorObj, _allocator);

		stopsArr.PushBack(stopObj, _allocator);
	}
	_object.AddMember(JsonString(c_colorMapStopsKey, _allocator), stopsArr, _allocator);
}

void ot::ColorMap::setFromJsonObject(const ConstJsonObject& _object) {
	if (_object.HasMember(c_colorMapNameKey)) {
		m_name = ot::json::getString(_object, c_colorMapNameKey);
	}
	else {
		m_name.clear();
	}

	if (_object.HasMember(c_colorMapPresetKey)) {
		m_preset = stringToColorMapPreset(ot::json::getString(_object, c_colorMapPresetKey));
	}
	else {
		m_preset = ColorMapPreset::Custom;
	}

	if (_object.HasMember(c_colorMapInterpolationSpaceKey)) {
		m_interpolationSpace = static_cast<ColorInterpolationSpace>(ot::json::getInt(_object, c_colorMapInterpolationSpaceKey));
	}
	else {
		m_interpolationSpace = ColorInterpolationSpace::Oklab;
	}

	m_stops.clear();
	if (_object.HasMember(c_colorMapStopsKey)) {
		ConstJsonArray stopsArr = ot::json::getArray(_object, c_colorMapStopsKey);
		for (JsonSizeType i = 0; i < stopsArr.Size(); ++i) {
			ConstJsonObject stopObj = ot::json::getObject(stopsArr, i);
			ColorStop stop;
			stop.position = static_cast<float>(ot::json::getDouble(stopObj, c_colorStopPositionKey));
			ConstJsonObject colorObj = ot::json::getObject(stopObj, c_colorStopColorKey);
			stop.color.setFromJsonObject(colorObj);
			m_stops.push_back(stop);
		}
		sortStops();
	}
}
