// Project header
#include "OTCore/Color.h"
#include "OTCore/LogDispatcher.h"

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
        OT_LOG_EA("Unknown color provided");
        break;
    }
}

ot::ColorF ot::Color::toColorF(void) const {
    return ColorF(m_r / 255.f, m_g / 255.f, m_b / 255.f, m_a / 255.f);
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
        OT_LOG_EA("Unknown color provided");
        break;
    }
}

ot::Color ot::ColorF::toColor(void) const {
    return Color((int)(m_r * 255.f), (int)(m_g * 255.f), (int)(m_b * 255.f), (int)(m_a * 255.f)); 
}
