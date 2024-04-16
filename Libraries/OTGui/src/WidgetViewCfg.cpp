//! @file WidgetViewCfg.cpp
//! @author Alexander Kuester (alexk95)
//! @date March 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTGui/WidgetViewCfg.h"

std::string ot::WidgetViewCfg::toString(ViewFlag _flag) {
	switch (_flag)
	{
	case WidgetViewCfg::ViewIsCloseable: return "ViewIsCloseable";
	default:
		OT_LOG_EAS("Unknown view flag (" + std::to_string(_flag) + ")");
		return "<null>";
	}
}

ot::WidgetViewCfg::ViewFlag ot::WidgetViewCfg::stringToViewFlag(const std::string& _flag) {
	if (_flag == WidgetViewCfg::toString(WidgetViewCfg::ViewIsCloseable)) return WidgetViewCfg::ViewIsCloseable;
	else {
		OT_LOG_EAS("Unknown view flag \"" + _flag + "\"");
		return NoViewFlags;
	}
}

std::list<std::string> ot::WidgetViewCfg::toStringList(ViewFlags _flags) {
	std::list<std::string> ret;
	if (_flags & WidgetViewCfg::ViewIsCloseable) ret.push_back(toString(WidgetViewCfg::ViewIsCloseable));

	return ret;
}

ot::WidgetViewCfg::ViewFlags ot::WidgetViewCfg::stringListToViewFlags(const std::list<std::string>& _flags) {
	WidgetViewCfg::ViewFlags ret = NoViewFlags;
	for (const std::string& flag : _flags) {
		ret |= WidgetViewCfg::stringToViewFlag(flag);
	}

	return ret;
}

std::string ot::WidgetViewCfg::dockLocationToString(ViewDockLocation _dockLocation) {
	switch (_dockLocation)
	{
	case ot::WidgetViewCfg::Default: return "Default";
	case ot::WidgetViewCfg::Left: return "Left";
	case ot::WidgetViewCfg::Top: return "Top";
	case ot::WidgetViewCfg::Right: return "Right";
	case ot::WidgetViewCfg::Bottom: return "Bottom";
	default:
		OT_LOG_EAS("Unknown view dock location (" + std::to_string(_dockLocation) + ")");
		return "Default";
	}
}

ot::WidgetViewCfg::ViewDockLocation ot::WidgetViewCfg::stringToDockLocation(const std::string& _dockLocation) {
	if (_dockLocation == WidgetViewCfg::dockLocationToString(WidgetViewCfg::Default)) return WidgetViewCfg::Default;
	else if (_dockLocation == WidgetViewCfg::dockLocationToString(WidgetViewCfg::Left)) return WidgetViewCfg::Left;
	else if (_dockLocation == WidgetViewCfg::dockLocationToString(WidgetViewCfg::Top)) return WidgetViewCfg::Top;
	else if (_dockLocation == WidgetViewCfg::dockLocationToString(WidgetViewCfg::Right)) return WidgetViewCfg::Right;
	else if (_dockLocation == WidgetViewCfg::dockLocationToString(WidgetViewCfg::Bottom)) return WidgetViewCfg::Bottom;
	else {
		OT_LOG_EAS("Unknown view dock location \"" + _dockLocation + "\"");
		return WidgetViewCfg::Default;
	}
}

// ###########################################################################################################################################################################################################################################################################################################################

ot::WidgetViewCfg::WidgetViewCfg() 
	: m_flags(WidgetViewCfg::NoViewFlags), m_dockLocation(WidgetViewCfg::Default)
{

}

ot::WidgetViewCfg::~WidgetViewCfg() {

}

// ###########################################################################################################################################################################################################################################################################################################################

// Base class functions

void ot::WidgetViewCfg::addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const {
	_object.AddMember("Name", JsonString(m_name, _allocator), _allocator);
	_object.AddMember("Parent", JsonString(m_parentViewName, _allocator), _allocator);
	_object.AddMember("Title", JsonString(m_title, _allocator), _allocator);
	_object.AddMember("Flags", JsonArray(this->toStringList(m_flags), _allocator), _allocator);
	_object.AddMember("DockLocation", JsonString(this->dockLocationToString(m_dockLocation), _allocator), _allocator);
}

void ot::WidgetViewCfg::setFromJsonObject(const ot::ConstJsonObject& _object) {
	m_name = json::getString(_object, "Name");
	m_parentViewName = json::getString(_object, "Parent");
	m_title = json::getString(_object, "Title");
	m_flags = this->stringListToViewFlags(json::getStringList(_object, "Flags"));
	m_dockLocation = this->stringToDockLocation(json::getString(_object, "DockLocation"));
}