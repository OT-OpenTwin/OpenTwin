//! \file GraphicsPickerItemInfo.cpp
//! \author Alexander Kuester (alexk95)
//! \date August 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTGui/GraphicsPickerItemInfo.h"

ot::GraphicsPickerItemInfo::GraphicsPickerItemInfo() {

}

ot::GraphicsPickerItemInfo::GraphicsPickerItemInfo(const std::string& _name, const std::string& _title, const std::string& _previewIcon) 
	: m_name(_name), m_title(_title), m_previewIcon(_previewIcon)
{}

ot::GraphicsPickerItemInfo::GraphicsPickerItemInfo(const GraphicsPickerItemInfo& _other) {
	*this = _other;
}

ot::GraphicsPickerItemInfo::~GraphicsPickerItemInfo() {}

ot::GraphicsPickerItemInfo& ot::GraphicsPickerItemInfo::operator = (const GraphicsPickerItemInfo& _other) {
	if (this == &_other) return *this;

	m_name = _other.m_name;
	m_title = _other.m_title;
	m_previewIcon = _other.m_previewIcon;

	return *this;
}

void ot::GraphicsPickerItemInfo::addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const {
	_object.AddMember("Name", JsonString(m_name, _allocator), _allocator);
	_object.AddMember("Title", JsonString(m_title, _allocator), _allocator);
	_object.AddMember("PreviewIcon", JsonString(m_previewIcon, _allocator), _allocator);
}

void ot::GraphicsPickerItemInfo::setFromJsonObject(const ConstJsonObject& _object) {
	m_name = json::getString(_object, "Name");
	m_title = json::getString(_object, "Title");
	m_previewIcon = json::getString(_object, "PreviewIcon");
}
