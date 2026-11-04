// @otlicense
// File: Color.cpp
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

// Project header
#include "OTCore/Color.h"
#include "OTCore/Logging/Logger.h"

// std header
#include <cmath>
#include <algorithm>

ot::Color::Color() : m_r(0), m_g(0), m_b(0), m_a(255) {}

ot::Color::Color(DefaultColor _color) : m_r(0), m_g(0), m_b(0), m_a(255) {
	this->set(_color);
}

ot::Color::Color(int _r, int _g, int _b, int _a) : m_r(_r), m_g(_g), m_b(_b), m_a(_a) {}

ot::Color::Color(const Color& _other) : m_r(_other.m_r), m_g(_other.m_g), m_b(_other.m_b), m_a(_other.m_a) {}

ot::Color::~Color() {}

ot::Color & ot::Color::operator = (const Color & _other) { m_r = _other.m_r; m_g = _other.m_g; m_b = _other.m_b; m_a = _other.m_a; return *this; }

bool ot::Color::operator == (const Color & _other) const {
	return m_r == _other.m_r && m_g == _other.m_g && m_b == _other.m_b && m_a == _other.m_a;
}

bool ot::Color::operator != (const Color & _other) const {
	return m_r != _other.m_r || m_g != _other.m_g || m_b != _other.m_b || m_a != _other.m_a;
}

void ot::Color::addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const {
    _object.AddMember("r", m_r, _allocator);
    _object.AddMember("g", m_g, _allocator);
    _object.AddMember("b", m_b, _allocator);
    _object.AddMember("a", m_a, _allocator);
}

void ot::Color::setFromJsonObject(const ConstJsonObject& _object) {
    m_r = json::getInt(_object, "r");
    m_g = json::getInt(_object, "g");
    m_b = json::getInt(_object, "b");
    m_a = json::getInt(_object, "a");
}

void ot::Color::set(ot::DefaultColor _color)
 {
    switch (_color) {
    case DefaultColor::Aqua:
        m_r = 0;
        m_g = 255;
        m_b = 255;
        m_a = 255;
        break;
    case DefaultColor::Beige:
        m_r = 245;
        m_g = 245;
        m_b = 219;
        m_a = 255;
        break;
    case DefaultColor::Black:
        m_r = 0;
        m_g = 0;
        m_b = 0;
        m_a = 255;
        break;
    case DefaultColor::Blue:
        m_r = 0;
        m_g = 0;
        m_b = 255;
        m_a = 255;
        break;
    case DefaultColor::Chocolate:
        m_r = 209;
        m_g = 105;
        m_b = 31;
        m_a = 255;
        break;
    case DefaultColor::Coral:
        m_r = 255;
        m_g = 127;
        m_b = 80;
        m_a = 255;
        break;
    case DefaultColor::Cyan:
        m_r = 0;
        m_g = 255;
        m_b = 255;
        m_a = 255;
        break;
    case DefaultColor::DarkGray:
        m_r = 168;
        m_g = 168;
        m_b = 168;
        m_a = 255;
        break;
    case DefaultColor::DarkGreen:
        m_r = 0;
        m_g = 100;
        m_b = 0;
        m_a = 255;
        break;
    case DefaultColor::DarkOrange:
        m_r = 255;
        m_g = 140;
        m_b = 0;
        m_a = 255;
        break;
    case DefaultColor::DarkViolet:
        m_r = 148;
        m_g = 0;
        m_b = 213;
        m_a = 255;
        break;
    case DefaultColor::DodgerBlue:
        m_r = 31;
        m_g = 144;
        m_b = 255;
        m_a = 255;
        break;
    case DefaultColor::Fuchsia:
        m_r = 255;
        m_g = 0;
        m_b = 255;
        m_a = 255;
        break;
    case DefaultColor::Gold:
        m_r = 255;
        m_g = 215;
        m_b = 0;
        m_a = 255;
        break;
    case DefaultColor::Gray:
        m_r = 128;
        m_g = 128;
        m_b = 128;
        m_a = 255;
        break;
    case DefaultColor::Green:
        m_r = 0;
        m_g = 128;
        m_b = 0;
        m_a = 255;
        break;
    case DefaultColor::IndianRed:
        m_r = 205;
        m_g = 92;
        m_b = 92;
        m_a = 255;
        break;
    case DefaultColor::Indigo:
        m_r = 75;
        m_g = 0;
        m_b = 130;
        m_a = 255;
        break;
    case DefaultColor::Ivory:
        m_r = 255;
        m_g = 255;
        m_b = 240;
        m_a = 255;
        break;
    case DefaultColor::Khaki:
        m_r = 240;
        m_g = 230;
        m_b = 140;
        m_a = 255;
        break;
    case DefaultColor::Lavender:
        m_r = 230;
        m_g = 230;
        m_b = 250;
        m_a = 255;
        break;
    case DefaultColor::Lime:
        m_r = 0;
        m_g = 255;
        m_b = 0;
        m_a = 255;
        break;
    case DefaultColor::LightGray:
        m_r = 211;
        m_g = 211;
        m_b = 211;
        m_a = 255;
        break;
    case DefaultColor::Maroon:
        m_r = 128;
        m_g = 0;
        m_b = 0;
        m_a = 255;
        break;
    case DefaultColor::MidnightBlue:
        m_r = 25;
        m_g = 25;
        m_b = 112;
        m_a = 255;
        break;
    case DefaultColor::Mint:
        m_r = 188;
        m_g = 252;
        m_b = 201;
        m_a = 255;
        break;
    case DefaultColor::Navy:
        m_r = 0;
        m_g = 0;
        m_b = 128;
        m_a = 255;
        break;
    case DefaultColor::Olive:
        m_r = 128;
        m_g = 128;
        m_b = 0;
        m_a = 255;
        break;
    case DefaultColor::Orange:
        m_r = 255;
        m_g = 165;
        m_b = 0;
        m_a = 255;
        break;
    case DefaultColor::Orchid:
        m_r = 218;
        m_g = 112;
        m_b = 214;
        m_a = 255;
        break;
    case DefaultColor::Pink:
        m_r = 255;
        m_g = 192;
        m_b = 203;
        m_a = 255;
        break;
    case DefaultColor::Plum:
        m_r = 221;
        m_g = 160;
        m_b = 221;
        m_a = 255;
        break;
    case DefaultColor::Purple:
        m_r = 128;
        m_g = 0;
        m_b = 128;
        m_a = 255;
        break;
    case DefaultColor::Red:
        m_r = 255;
        m_g = 0;
        m_b = 0;
        m_a = 255;
        break;
    case DefaultColor::RoyalBlue:
        m_r = 65;
        m_g = 105;
        m_b = 225;
        m_a = 255;
        break;
    case DefaultColor::Salmon:
        m_r = 250;
        m_g = 128;
        m_b = 114;
        m_a = 255;
        break;
    case DefaultColor::Sienna:
        m_r = 160;
        m_g = 82;
        m_b = 45;
        m_a = 255;
        break;
    case DefaultColor::Silver:
        m_r = 192;
        m_g = 192;
        m_b = 192;
        m_a = 255;
        break;
    case DefaultColor::SlateGray:
        m_r = 112;
        m_g = 128;
        m_b = 144;
        m_a = 255;
        break;
    case DefaultColor::SkyBlue:
        m_r = 135;
        m_g = 206;
        m_b = 235;
        m_a = 255;
        break;
    case DefaultColor::SteelBlue:
        m_r = 70;
        m_g = 130;
        m_b = 180;
        m_a = 255;
        break;
    case DefaultColor::Tan:
        m_r = 210;
        m_g = 180;
        m_b = 140;
        m_a = 255;
        break;
    case DefaultColor::Teal:
        m_r = 0;
        m_g = 128;
        m_b = 128;
        m_a = 255;
        break;
    case DefaultColor::Tomato:
        m_r = 255;
        m_g = 99;
        m_b = 71;
        m_a = 255;
        break;
    case DefaultColor::Transparent:
        m_r = 0;
        m_g = 0;
        m_b = 0;
        m_a = 0;
        break;
    case DefaultColor::Turquoise:
        m_r = 64;
        m_g = 224;
        m_b = 208;
        m_a = 255;
        break;
    case DefaultColor::Violet:
        m_r = 238;
        m_g = 130;
        m_b = 238;
        m_a = 255;
        break;
    case DefaultColor::White:
        m_r = 255;
        m_g = 255;
        m_b = 255;
        m_a = 255;
        break;
    case DefaultColor::Yellow:
        m_r = 255;
        m_g = 255;
        m_b = 0;
        m_a = 255;
        break;
    default:
		OTAssert(0, "Unknown color provided");
        OT_LOG_E("Unknown color provided");
        break;
    }
}

ot::ColorF ot::Color::toColorF(void) const {
    return ColorF(m_r / 255.f, m_g / 255.f, m_b / 255.f, m_a / 255.f);
}

void ot::Color::toHsv(float& _h, float& _s, float& _v) const {
	const float r = static_cast<float>(m_r) / 255.f;
	const float g = static_cast<float>(m_g) / 255.f;
	const float b = static_cast<float>(m_b) / 255.f;

	const float cMax = std::max({ r, g, b });
	const float cMin = std::min({ r, g, b });
	const float delta = cMax - cMin;

	_v = cMax;

	if (cMax <= 0.f) {
		_s = 0.f;
		_h = 0.f;
		return;
	}

	_s = delta / cMax;

	if (delta <= 1e-6f) {
		_h = 0.f;
	}
	else if (cMax == r) {
		_h = 60.f * std::fmod(((g - b) / delta), 6.f);
	}
	else if (cMax == g) {
		_h = 60.f * (((b - r) / delta) + 2.f);
	}
	else {
		_h = 60.f * (((r - g) / delta) + 4.f);
	}

	if (_h < 0.f) {
		_h += 360.f;
	}
}

ot::Color ot::Color::fromHsv(float _h, float _s, float _v, int _a) {
	_h = std::fmod(_h, 360.f);
	if (_h < 0.f) {
		_h += 360.f;
	}

	_s = std::max(0.f, std::min(1.f, _s));
	_v = std::max(0.f, std::min(1.f, _v));
	_a = std::max(0, std::min(255, _a));

	const float c = _v * _s;
	const float x = c * (1.f - std::abs(std::fmod(_h / 60.f, 2.f) - 1.f));
	const float m = _v - c;

	float r1 = 0.f, g1 = 0.f, b1 = 0.f;
	if (_h >= 0.f && _h < 60.f) {
		r1 = c; g1 = x; b1 = 0.f;
	}
	else if (_h >= 60.f && _h < 120.f) {
		r1 = x; g1 = c; b1 = 0.f;
	}
	else if (_h >= 120.f && _h < 180.f) {
		r1 = 0.f; g1 = c; b1 = x;
	}
	else if (_h >= 180.f && _h < 240.f) {
		r1 = 0.f; g1 = x; b1 = c;
	}
	else if (_h >= 240.f && _h < 300.f) {
		r1 = x; g1 = 0.f; b1 = c;
	}
	else {
		r1 = c; g1 = 0.f; b1 = x;
	}

	int r = static_cast<int>(std::round((r1 + m) * 255.f));
	int g = static_cast<int>(std::round((g1 + m) * 255.f));
	int b = static_cast<int>(std::round((b1 + m) * 255.f));

	r = std::max(0, std::min(255, r));
	g = std::max(0, std::min(255, g));
	b = std::max(0, std::min(255, b));

	return Color(r, g, b, _a);
}


void ot::Color::toHsl(float& _h, float& _s, float& _l) const {
	const float r = static_cast<float>(m_r) / 255.f;
	const float g = static_cast<float>(m_g) / 255.f;
	const float b = static_cast<float>(m_b) / 255.f;

	const float cMax = std::max({ r, g, b });
	const float cMin = std::min({ r, g, b });
	const float delta = cMax - cMin;

	_l = (cMax + cMin) / 2.f;

	if (delta <= 1e-6f) {
		_s = 0.f;
		_h = 0.f;
		return;
	}

	if (_l <= 0.5f) {
		_s = delta / (cMax + cMin);
	}
	else {
		_s = delta / (2.f - cMax - cMin);
	}

	if (cMax == r) {
		_h = 60.f * std::fmod(((g - b) / delta), 6.f);
	}
	else if (cMax == g) {
		_h = 60.f * (((b - r) / delta) + 2.f);
	}
	else {
		_h = 60.f * (((r - g) / delta) + 4.f);
	}

	if (_h < 0.f) {
		_h += 360.f;
	}
}

ot::Color ot::Color::fromHsl(float _h, float _s, float _l, int _a) {
	_h = std::fmod(_h, 360.f);
	if (_h < 0.f) {
		_h += 360.f;
	}

	_s = std::max(0.f, std::min(1.f, _s));
	_l = std::max(0.f, std::min(1.f, _l));
	_a = std::max(0, std::min(255, _a));

	const float c = (1.f - std::abs(2.f * _l - 1.f)) * _s;
	const float x = c * (1.f - std::abs(std::fmod(_h / 60.f, 2.f) - 1.f));
	const float m = _l - c / 2.f;

	float r1 = 0.f, g1 = 0.f, b1 = 0.f;
	if (_h >= 0.f && _h < 60.f) {
		r1 = c; g1 = x; b1 = 0.f;
	}
	else if (_h >= 60.f && _h < 120.f) {
		r1 = x; g1 = c; b1 = 0.f;
	}
	else if (_h >= 120.f && _h < 180.f) {
		r1 = 0.f; g1 = c; b1 = x;
	}
	else if (_h >= 180.f && _h < 240.f) {
		r1 = 0.f; g1 = x; b1 = c;
	}
	else if (_h >= 240.f && _h < 300.f) {
		r1 = x; g1 = 0.f; b1 = c;
	}
	else {
		r1 = c; g1 = 0.f; b1 = x;
	}

	int r = static_cast<int>(std::round((r1 + m) * 255.f));
	int g = static_cast<int>(std::round((g1 + m) * 255.f));
	int b = static_cast<int>(std::round((b1 + m) * 255.f));

	r = std::max(0, std::min(255, r));
	g = std::max(0, std::min(255, g));
	b = std::max(0, std::min(255, b));

	return Color(r, g, b, _a);
}

namespace {
	constexpr float s_pi = 3.14159265358979323846f;

	inline float sRgbToLinear(float _c) {
		return (_c <= 0.04045f) ? (_c / 12.92f) : std::pow((_c + 0.055f) / 1.055f, 2.4f);
	}

	inline float linearToSRgb(float _c) {
		if (_c <= 0.0031308f) {
			return (_c <= 0.f) ? 0.f : (12.92f * _c);
		}
		if (_c >= 1.0f) {
			return 1.0f;
		}
		return 1.055f * std::pow(_c, 1.0f / 2.4f) - 0.055f;
	}
}

void ot::Color::toOklab(float& _L, float& _a, float& _b) const {
	const float rLin = sRgbToLinear(static_cast<float>(m_r) / 255.f);
	const float gLin = sRgbToLinear(static_cast<float>(m_g) / 255.f);
	const float bLin = sRgbToLinear(static_cast<float>(m_b) / 255.f);

	const float l = 0.4122214708f * rLin + 0.5363325363f * gLin + 0.0514459929f * bLin;
	const float m = 0.2119034982f * rLin + 0.6806995451f * gLin + 0.1073969566f * bLin;
	const float s = 0.0883024619f * rLin + 0.2817188376f * gLin + 0.6299787005f * bLin;

	const float l_ = std::cbrt(l);
	const float m_ = std::cbrt(m);
	const float s_ = std::cbrt(s);

	_L = 0.2104542553f * l_ + 0.7936177850f * m_ - 0.0040720468f * s_;
	_a = 1.9779984951f * l_ - 2.4285922050f * m_ + 0.4505937099f * s_;
	_b = 0.0259040371f * l_ + 0.7827717662f * m_ - 0.8086757660f * s_;
}

ot::Color ot::Color::fromOklab(float _L, float _a, float _b, int _alpha) {
	if (_L <= 0.f) {
		return Color(0, 0, 0, std::max(0, std::min(255, _alpha)));
	}

	const float l_ = _L + 0.3963377774f * _a + 0.2158037573f * _b;
	const float m_ = _L - 0.1055613458f * _a - 0.0638541728f * _b;
	const float s_ = _L - 0.0894841775f * _a - 1.2914855480f * _b;

	const float l = l_ * l_ * l_;
	const float m = m_ * m_ * m_;
	const float s = s_ * s_ * s_;

	const float rLin = +4.0767416621f * l - 3.3077115913f * m + 0.2309699292f * s;
	const float gLin = -1.2684380046f * l + 2.6097574011f * m - 0.3413193965f * s;
	const float bLin = -0.0041960863f * l - 0.7034186147f * m + 1.7076147010f * s;

	const float rSrgb = linearToSRgb(rLin);
	const float gSrgb = linearToSRgb(gLin);
	const float bSrgb = linearToSRgb(bLin);

	int r = static_cast<int>(std::round(rSrgb * 255.f));
	int g = static_cast<int>(std::round(gSrgb * 255.f));
	int b = static_cast<int>(std::round(bSrgb * 255.f));

	r = std::max(0, std::min(255, r));
	g = std::max(0, std::min(255, g));
	b = std::max(0, std::min(255, b));
	int a = std::max(0, std::min(255, _alpha));

	return Color(r, g, b, a);
}

void ot::Color::toOklch(float& _L, float& _C, float& _h) const {
	float a = 0.f, b = 0.f;
	toOklab(_L, a, b);

	_C = std::sqrt(a * a + b * b);
	if (_C <= 1e-6f) {
		_h = 0.f;
	}
	else {
		_h = std::atan2(b, a) * (180.f / s_pi);
		if (_h < 0.f) {
			_h += 360.f;
		}
	}
}

ot::Color ot::Color::fromOklch(float _L, float _C, float _h, int _alpha) {
	_h = std::fmod(_h, 360.f);
	if (_h < 0.f) {
		_h += 360.f;
	}

	_C = std::max(0.f, _C);

	const float rad = _h * (s_pi / 180.f);
	const float a = _C * std::cos(rad);
	const float b = _C * std::sin(rad);

	return fromOklab(_L, a, b, _alpha);
}

double ot::Color::relativeLuminance() const {
	const float rLin = sRgbToLinear(static_cast<float>(m_r) / 255.f);
	const float gLin = sRgbToLinear(static_cast<float>(m_g) / 255.f);
	const float bLin = sRgbToLinear(static_cast<float>(m_b) / 255.f);

	return 0.2126 * static_cast<double>(rLin) +
	       0.7152 * static_cast<double>(gLin) +
	       0.0722 * static_cast<double>(bLin);
}

double ot::Color::contrastRatio(const Color& _other) const {
	const double l1 = relativeLuminance();
	const double l2 = _other.relativeLuminance();
	const double lighter = std::max(l1, l2);
	const double darker = std::min(l1, l2);
	return (lighter + 0.05) / (darker + 0.05);
}

ot::Color ot::Color::getContrastingTextColor() const {
	// WCAG 2.2 contrast threshold between black and white:
	// (L + 0.05) / 0.05 = 1.05 / (L + 0.05) ==> L = sqrt(0.0525) - 0.05 approx 0.1791287847
	if (relativeLuminance() > 0.1791287847) {
		return Color(DefaultColor::Black);
	}
	else {
		return Color(DefaultColor::White);
	}
}

int ot::Color::channelValueFromFloat(float _value) {
	return static_cast<int>(_value * 255.f);
}

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

ot::ColorF::ColorF() : m_r(0.f), m_g(0.f), m_b(0.f), m_a(1.f) {}

ot::ColorF::ColorF(DefaultColor _color) : m_r(0.f), m_g(0.f), m_b(0.f), m_a(1.f) {
    set(_color);
}

ot::ColorF::ColorF(float _r, float _g, float _b, float _a) : m_r(_r), m_g(_g), m_b(_b), m_a(_a) {}

ot::ColorF::ColorF(const ColorF& _other) : m_r(_other.m_r), m_g(_other.m_g), m_b(_other.m_b), m_a(_other.m_a) {}

ot::ColorF::~ColorF() {}

ot::ColorF& ot::ColorF::operator = (const ColorF& _other) { m_r = _other.m_r; m_g = _other.m_g; m_b = _other.m_b; m_a = _other.m_a; return *this; }

bool ot::ColorF::operator == (const ColorF& _other) const {
    return m_r == _other.m_r && m_g == _other.m_g && m_b == _other.m_b && m_a == _other.m_a;
}

bool ot::ColorF::operator != (const ColorF& _other) const {
    return m_r != _other.m_r || m_g != _other.m_g || m_b != _other.m_b || m_a != _other.m_a;
}

void ot::ColorF::addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const {
    _object.AddMember("r", m_r, _allocator);
    _object.AddMember("g", m_g, _allocator);
    _object.AddMember("b", m_b, _allocator);
    _object.AddMember("a", m_a, _allocator);
}

void ot::ColorF::setFromJsonObject(const ConstJsonObject& _object) {
    m_r = json::getFloat(_object, "r");
    m_g = json::getFloat(_object, "g");
    m_b = json::getFloat(_object, "b");
    m_a = json::getFloat(_object, "a");
}

void ot::ColorF::set(ot::DefaultColor _color)
{
    switch (_color) {
    case DefaultColor::Aqua:
        m_r = 0.0f;
        m_g = 1.0f;
        m_b = 1.0f;
        m_a = 1.0f;
        break;
    case DefaultColor::Beige:
        m_r = 0.96f;
        m_g = 0.96f;
        m_b = 0.86f;
        m_a = 1.0f;
        break;
    case DefaultColor::Black:
        m_r = 0.0f;
        m_g = 0.0f;
        m_b = 0.0f;
        m_a = 1.0f;
        break;
    case DefaultColor::Blue:
        m_r = 0.0f;
        m_g = 0.0f;
        m_b = 1.0f;
        m_a = 1.0f;
        break;
    case DefaultColor::Chocolate:
        m_r = 0.82f;
        m_g = 0.41f;
        m_b = 0.12f;
        m_a = 1.0f;
        break;
    case DefaultColor::Coral:
        m_r = 1.0f;
        m_g = 0.5f;
        m_b = 0.31f;
        m_a = 1.0f;
        break;
    case DefaultColor::Cyan:
        m_r = 0.0f;
        m_g = 1.0f;
        m_b = 1.0f;
        m_a = 1.0f;
        break;
    case DefaultColor::DarkGray:
        m_r = 0.66f;
        m_g = 0.66f;
        m_b = 0.66f;
        m_a = 1.0f;
        break;
    case DefaultColor::DarkGreen:
        m_r = 0.0f;
        m_g = 0.39f;
        m_b = 0.0f;
        m_a = 1.0f;
        break;
    case DefaultColor::DarkOrange:
        m_r = 1.0f;
        m_g = 0.55f;
        m_b = 0.0f;
        m_a = 1.0f;
        break;
    case DefaultColor::DarkViolet:
        m_r = 0.58f;
        m_g = 0.0f;
        m_b = 0.83f;
        m_a = 1.0f;
        break;
    case DefaultColor::DodgerBlue:
        m_r = 0.12f;
        m_g = 0.56f;
        m_b = 1.0f;
        m_a = 1.0f;
        break;
    case DefaultColor::Fuchsia:
        m_r = 1.0f;
        m_g = 0.0f;
        m_b = 1.0f;
        m_a = 1.0f;
        break;
    case DefaultColor::Gold:
        m_r = 1.0f;
        m_g = 0.84f;
        m_b = 0.0f;
        m_a = 1.0f;
        break;
    case DefaultColor::Gray:
        m_r = 0.5f;
        m_g = 0.5f;
        m_b = 0.5f;
        m_a = 1.0f;
        break;
    case DefaultColor::Green:
        m_r = 0.0f;
        m_g = 0.5f;
        m_b = 0.0f;
        m_a = 1.0f;
        break;
    case DefaultColor::IndianRed:
        m_r = 0.80f;
        m_g = 0.36f;
        m_b = 0.36f;
        m_a = 1.0f;
        break;
    case DefaultColor::Indigo:
        m_r = 0.29f;
        m_g = 0.0f;
        m_b = 0.51f;
        m_a = 1.0f;
        break;
    case DefaultColor::Ivory:
        m_r = 1.0f;
        m_g = 1.0f;
        m_b = 0.94f;
        m_a = 1.0f;
        break;
    case DefaultColor::Khaki:
        m_r = 0.94f;
        m_g = 0.90f;
        m_b = 0.55f;
        m_a = 1.0f;
        break;
    case DefaultColor::Lavender:
        m_r = 0.90f;
        m_g = 0.90f;
        m_b = 0.98f;
        m_a = 1.0f;
        break;
    case DefaultColor::Lime:
        m_r = 0.0f;
        m_g = 1.0f;
        m_b = 0.0f;
        m_a = 1.0f;
        break;
    case DefaultColor::LightGray:
        m_r = 0.83f;
        m_g = 0.83f;
        m_b = 0.83f;
        m_a = 1.0f;
        break;
    case DefaultColor::Maroon:
        m_r = 0.50f;
        m_g = 0.0f;
        m_b = 0.0f;
        m_a = 1.0f;
        break;
    case DefaultColor::MidnightBlue:
        m_r = 0.10f;
        m_g = 0.10f;
        m_b = 0.44f;
        m_a = 1.0f;
        break;
    case DefaultColor::Mint:
        m_r = 0.74f;
        m_g = 0.99f;
        m_b = 0.79f;
        m_a = 1.0f;
        break;
    case DefaultColor::Navy:
        m_r = 0.0f;
        m_g = 0.0f;
        m_b = 0.50f;
        m_a = 1.0f;
        break;
    case DefaultColor::Olive:
        m_r = 0.50f;
        m_g = 0.50f;
        m_b = 0.0f;
        m_a = 1.0f;
        break;
    case DefaultColor::Orange:
        m_r = 1.0f;
        m_g = 0.65f;
        m_b = 0.0f;
        m_a = 1.0f;
        break;
    case DefaultColor::Orchid:
        m_r = 0.85f;
        m_g = 0.44f;
        m_b = 0.84f;
        m_a = 1.0f;
        break;
    case DefaultColor::Pink:
        m_r = 1.0f;
        m_g = 0.75f;
        m_b = 0.80f;
        m_a = 1.0f;
        break;
    case DefaultColor::Plum:
        m_r = 0.87f;
        m_g = 0.63f;
        m_b = 0.87f;
        m_a = 1.0f;
        break;
    case DefaultColor::Purple:
        m_r = 0.50f;
        m_g = 0.0f;
        m_b = 0.50f;
        m_a = 1.0f;
        break;
    case DefaultColor::Red:
        m_r = 1.0f;
        m_g = 0.0f;
        m_b = 0.0f;
        m_a = 1.0f;
        break;
    case DefaultColor::RoyalBlue:
        m_r = 0.25f;
        m_g = 0.41f;
        m_b = 0.88f;
        m_a = 1.0f;
        break;
    case DefaultColor::Salmon:
        m_r = 0.98f;
        m_g = 0.50f;
        m_b = 0.45f;
        m_a = 1.0f;
        break;
    case DefaultColor::Sienna:
        m_r = 0.63f;
        m_g = 0.32f;
        m_b = 0.18f;
        m_a = 1.0f;
        break;
    case DefaultColor::Silver:
        m_r = 0.75f;
        m_g = 0.75f;
        m_b = 0.75f;
        m_a = 1.0f;
        break;
    case DefaultColor::SlateGray:
        m_r = 0.44f;
        m_g = 0.50f;
        m_b = 0.56f;
        m_a = 1.0f;
        break;
    case DefaultColor::SkyBlue:
        m_r = 0.53f;
        m_g = 0.81f;
        m_b = 0.92f;
        m_a = 1.0f;
        break;
    case DefaultColor::SteelBlue:
        m_r = 0.27f;
        m_g = 0.51f;
        m_b = 0.71f;
        m_a = 1.0f;
        break;
    case DefaultColor::Tan:
        m_r = 0.82f;
        m_g = 0.71f;
        m_b = 0.55f;
        m_a = 1.0f;
        break;
    case DefaultColor::Teal:
        m_r = 0.0f;
        m_g = 0.50f;
        m_b = 0.50f;
        m_a = 1.0f;
        break;
    case DefaultColor::Tomato:
        m_r = 1.0f;
        m_g = 0.39f;
        m_b = 0.28f;
        m_a = 1.0f;
        break;
    case DefaultColor::Transparent:
        m_r = 0.f;
        m_g = 0.f;
        m_b = 0.f;
        m_a = 0.f;
        break;
    case DefaultColor::Turquoise:
        m_r = 0.25f;
        m_g = 0.88f;
        m_b = 0.82f;
        m_a = 1.0f;
        break;
    case DefaultColor::Violet:
        m_r = 0.93f;
        m_g = 0.51f;
        m_b = 0.93f;
        m_a = 1.0f;
        break;
    case DefaultColor::White:
        m_r = 1.0f;
        m_g = 1.0f;
        m_b = 1.0f;
        m_a = 1.0f;
        break;
    case DefaultColor::Yellow:
        m_r = 1.0f;
        m_g = 1.0f;
        m_b = 0.0f;
        m_a = 1.0f;
        break;
    default:
        OT_LOG_E("Unknown color provided");
        break;
    }
}

ot::Color ot::ColorF::toColor(void) const {
    return Color((int)(m_r * 255.f), (int)(m_g * 255.f), (int)(m_b * 255.f), (int)(m_a * 255.f)); 
}

float ot::ColorF::channelValueFromInt(int _value) {
	return static_cast<float>(_value) / 255.f;
}
