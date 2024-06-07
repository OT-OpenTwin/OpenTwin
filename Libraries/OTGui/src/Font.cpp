//! @file Font.cpp
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTGui/Font.h"

ot::Font::Font() : m_sizePx(12), m_isBold(false), m_isItalic(false), m_family("Consolas") {}

ot::Font::Font(FontFamily _fontFamily, int _sizePx, bool _isBold, bool _isItalic) : m_sizePx(_sizePx), m_isBold(_isBold), m_isItalic(_isItalic) {
	m_family = toString(_fontFamily);
}

ot::Font::Font(const std::string& _fontFamily, int _sizePx, bool _isBold, bool _isItalic) : m_family(_fontFamily), m_sizePx(_sizePx), m_isBold(_isBold), m_isItalic(_isItalic) {}

ot::Font::Font(const Font& _other) : m_family(_other.m_family), m_sizePx(_other.m_sizePx), m_isBold(_other.m_isBold), m_isItalic(_other.m_isItalic) {}

ot::Font::~Font() {}

ot::Font& ot::Font::operator = (const Font& _other) {
	m_family = _other.m_family;
	m_sizePx = _other.m_sizePx;
	m_isBold = _other.m_isBold;
	m_isItalic = _other.m_isItalic;
	return *this;
}

bool ot::Font::operator == (const Font& _other) const {
	return m_family == _other.m_family && m_sizePx == _other.m_sizePx && m_isBold == _other.m_isBold && m_isItalic == _other.m_isItalic;
}

bool ot::Font::operator != (const Font& _other) const {
	return !(*this == _other);
}

void ot::Font::addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const {
	_object.AddMember("Family", JsonString(m_family, _allocator), _allocator);
	_object.AddMember("SizePx", m_sizePx, _allocator);
	_object.AddMember("Bold", m_isBold, _allocator);
	_object.AddMember("Italic", m_isItalic, _allocator);
}

void ot::Font::setFromJsonObject(const ConstJsonObject& _object) {
	m_family = json::getString(_object, "Family");
	m_sizePx = json::getInt(_object, "SizePx");
	m_isBold = json::getBool(_object, "Bold");
	m_isItalic = json::getBool(_object, "Italic");
}

void ot::Font::setFamily(FontFamily _fontFamily) {
	m_family = toString(_fontFamily);
}