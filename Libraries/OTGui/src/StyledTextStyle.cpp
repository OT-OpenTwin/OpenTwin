// @otlicense

// OpenTwin header
#include "OTCore/LogDispatcher.h"
#include "OTGui/StyledTextStyle.h"

std::string ot::StyledTextStyle::toString(TextSize _size) {
	switch (_size) {
	case ot::StyledTextStyle::TextSize::Regular: return "Regular";
	case ot::StyledTextStyle::TextSize::Header1: return "Header1";
	case ot::StyledTextStyle::TextSize::Header2: return "Header2";
	case ot::StyledTextStyle::TextSize::Header3: return "Header3";
	default:
		OT_LOG_EAS("Unknown text size (" + std::to_string((int)_size) + ")");
		return "Regular";
	}
}

ot::StyledTextStyle::TextSize ot::StyledTextStyle::stringToTextSize(const std::string& _size) {
	if (_size == StyledTextStyle::toString(TextSize::Regular)) return TextSize::Regular;
	else if (_size == StyledTextStyle::toString(TextSize::Header1)) return TextSize::Header1;
	else if (_size == StyledTextStyle::toString(TextSize::Header2)) return TextSize::Header2;
	else if (_size == StyledTextStyle::toString(TextSize::Header3)) return TextSize::Header3;
	else {
		OT_LOG_EAS("Unknown text size \"" + _size + "\"");
		return TextSize::Regular;
	}
}

ot::StyledTextStyle::StyledTextStyle()
	: m_colorReference(StyledText::ColorReference::Default), m_isBold(false), m_isItalic(false), 
	m_isUnderline(false), m_isLineTrough(false), m_size(TextSize::Regular)
{

}

ot::StyledTextStyle::StyledTextStyle(const ConstJsonObject& _jsonObject)
	: m_colorReference(StyledText::ColorReference::Default), m_isBold(false), m_isItalic(false),
	m_isUnderline(false), m_isLineTrough(false), m_size(TextSize::Regular)
{
	this->setFromJsonObject(_jsonObject);
}

bool ot::StyledTextStyle::operator==(const StyledTextStyle& _other) const {
	return m_colorReference == _other.m_colorReference &&
		m_isBold == _other.m_isBold &&
		m_isItalic == _other.m_isItalic &&
		m_isUnderline == _other.m_isUnderline &&
		m_isLineTrough == _other.m_isLineTrough &&
		m_size == _other.m_size;
}

bool ot::StyledTextStyle::operator!=(const StyledTextStyle& _other) const {
	return m_colorReference != _other.m_colorReference ||
		m_isBold != _other.m_isBold ||
		m_isItalic != _other.m_isItalic ||
		m_isUnderline != _other.m_isUnderline ||
		m_isLineTrough != _other.m_isLineTrough ||
		m_size != _other.m_size;
}

void ot::StyledTextStyle::addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const {
	_object.AddMember("ColorRef", JsonString(StyledText::toString(m_colorReference), _allocator), _allocator);
	_object.AddMember("Bold", m_isBold, _allocator);
	_object.AddMember("Italic", m_isItalic, _allocator);
	_object.AddMember("Underline", m_isUnderline, _allocator);
	_object.AddMember("LineTrough", m_isLineTrough, _allocator);
	_object.AddMember("Size", JsonString(this->toString(m_size), _allocator), _allocator);
}

void ot::StyledTextStyle::setFromJsonObject(const ot::ConstJsonObject& _object) {
	m_colorReference = StyledText::stringToColorReference(json::getString(_object, "ColorRef"));
	m_isBold = json::getBool(_object, "Bold");
	m_isItalic = json::getBool(_object, "Italic");
	m_isUnderline = json::getBool(_object, "Underline");
	m_isLineTrough = json::getBool(_object, "LineTrough");
	m_size = this->stringToTextSize(json::getString(_object, "Size"));
}
