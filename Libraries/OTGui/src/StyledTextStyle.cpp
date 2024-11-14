//! @file StyledTextStyle.cpp
//! @author Alexander Kuester (alexk95)
//! @date November 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTGui/StyledTextStyle.h"

ot::StyledTextStyle::StyledTextStyle() 
	: m_colorReference(StyledText::ColorReference::Default), m_isBold(false), m_isItalic(false)
{

}

ot::StyledTextStyle::StyledTextStyle(const ConstJsonObject& _jsonObject)
	: m_colorReference(StyledText::ColorReference::Default), m_isBold(false), m_isItalic(false)
{
	this->setFromJsonObject(_jsonObject);
}

void ot::StyledTextStyle::addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const {
	_object.AddMember("ColorRef", JsonString(StyledText::toString(m_colorReference), _allocator), _allocator);
	_object.AddMember("Bold", m_isBold, _allocator);
	_object.AddMember("Italic", m_isItalic, _allocator);
}

void ot::StyledTextStyle::setFromJsonObject(const ot::ConstJsonObject& _object) {
	m_colorReference = StyledText::stringToColorReference(json::getString(_object, "ColorRef"));
	m_isBold = json::getBool(_object, "Bold");
	m_isItalic = json::getBool(_object, "Italic");
}
