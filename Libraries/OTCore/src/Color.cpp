// Project header
#include "OTCore/Color.h"
#include "OTCore/Logger.h"

ot::Color::Color() : m_r(0.f), m_g(0.f), m_b(0.f), m_a(1.f) {}

ot::Color::Color(DefaultColor _color) : m_r(0.f), m_g(0.f), m_b(0.f), m_a(1.f) {
	set(_color);
}

ot::Color::Color(float _r, float _g, float _b, float _a) : m_r(_r), m_g(_g), m_b(_b), m_a(_a) {}

ot::Color::Color(int r, int g, int b, int a) { set(r, g, b, a); }

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
    auto x = ot::json::getObjectList(_object, "A");
    for (const ot::ConstJsonObject& xx : x) {

    }
}

void ot::Color::setFromJsonObject(const ConstJsonObject& _object) {
    m_r = json::getFloat(_object, "r");
    m_g = json::getFloat(_object, "g");
    m_b = json::getFloat(_object, "b");
    m_a = json::getFloat(_object, "a");
}

void ot::Color::set(ot::Color::DefaultColor _color) {
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

void ot::Color::set(int _r, int _g, int _b, int _a) { setR(_r); setG(_g); setB(_b); setA(_a); }
void ot::Color::setR(int _r) { m_r = _r / 255.f; }
void ot::Color::setG(int _g) { m_g = _g / 255.f; }
void ot::Color::setB(int _b) { m_b = _b / 255.f; }
void ot::Color::setA(int _a) { m_a = _a / 255.f; }

int ot::Color::rInt(void) const { return (int)(m_r * 255.f); }
int ot::Color::gInt(void) const { return (int)(m_g * 255.f); }
int ot::Color::bInt(void) const { return (int)(m_b * 255.f); }
int ot::Color::aInt(void) const { return (int)(m_a * 255.f); }