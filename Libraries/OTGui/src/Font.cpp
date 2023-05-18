//! @file Border.h
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTGui/Font.h"
#include "OpenTwinCore/rJSONHelper.h"

ot::Font::Font() : m_sizePx(12), m_isBold(false), m_isItalic(false) {}

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

void ot::Font::addToJsonObject(OT_rJSON_doc& _document, OT_rJSON_val& _object) const {
	ot::rJSON::add(_document, _object, "Family", m_family);
	ot::rJSON::add(_document, _object, "SizePx", m_sizePx);
	ot::rJSON::add(_document, _object, "Bold", m_isBold);
	ot::rJSON::add(_document, _object, "Italic", m_isItalic);
}

void ot::Font::setFromJsonObject(OT_rJSON_val& _object) {
	OT_rJSON_checkMember(_object, "Family", String);
	OT_rJSON_checkMember(_object, "SizePx", Int);
	OT_rJSON_checkMember(_object, "Bold", Bool);
	OT_rJSON_checkMember(_object, "Italic", Bool);

	m_family = _object["Family"].GetString();
	m_sizePx = _object["SizePx"].GetInt();
	m_isBold = _object["Bold"].GetBool();
	m_isItalic = _object["Italic"].GetBool();
}

void ot::Font::setFamily(FontFamily _fontFamily) {
	m_family = toString(_fontFamily);
}