// @otlicense

#pragma once

// OpenTwin header
#include "OTGui/GraphicsPickerItemInfo.h"

ot::GraphicsPickerItemInfo::GraphicsPickerItemInfo() {

}

ot::GraphicsPickerItemInfo::GraphicsPickerItemInfo(const std::string& _name, const std::string& _title, const std::string& _previewIcon) 
	: m_name(_name), m_title(_title), m_previewIcon(_previewIcon)
{}

ot::GraphicsPickerItemInfo::~GraphicsPickerItemInfo() {}

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
