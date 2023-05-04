// Project header
#include "OpenTwinCore/Color.h"

ot::Color::Color() : m_r(0.f), m_g(0.f), m_b(0.f), m_a(1.f) {}

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

void ot::Color::addToJsonObject(OT_rJSON_doc& _document, OT_rJSON_val& _object) const {
	ot::rJSON::add(_document, _object, "R", m_r);
	ot::rJSON::add(_document, _object, "G", m_g);
	ot::rJSON::add(_document, _object, "B", m_b);
	ot::rJSON::add(_document, _object, "A", m_a);
}

void ot::Color::setFromJsonObject(OT_rJSON_val& _object) {
	m_r = ot::rJSON::getFloat(_object, "R");
	m_g = ot::rJSON::getFloat(_object, "G");
	m_b = ot::rJSON::getFloat(_object, "B");
	m_a = ot::rJSON::getFloat(_object, "A");
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