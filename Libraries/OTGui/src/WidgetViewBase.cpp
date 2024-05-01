//! @file WidgetViewBase.cpp
//! @author Alexander Kuester (alexk95)
//! @date April 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTGui/WidgetViewBase.h"

std::string ot::WidgetViewBase::toString(ViewFlag _flag) {
	switch (_flag)
	{
	case WidgetViewBase::ViewIsCloseable: return "ViewIsCloseable";
	case WidgetViewBase::ViewIsPinnable: return "ViewIsPinnable";
	case WidgetViewBase::ViewIsCentral: return "ViewIsCentral";
	case WidgetViewBase::ViewIsSide: return "ViewIsSide";
	case WidgetViewBase::ViewIsTool: return "ViewIsTool";
	default:
		OT_LOG_EAS("Unknown view flag (" + std::to_string(_flag) + ")");
		return "<null>";
	}
}

ot::WidgetViewBase::ViewFlag ot::WidgetViewBase::stringToViewFlag(const std::string& _flag) {
	if (_flag == WidgetViewBase::toString(WidgetViewBase::ViewIsCloseable)) return WidgetViewBase::ViewIsCloseable;
	else if (_flag == WidgetViewBase::toString(WidgetViewBase::ViewIsPinnable)) return WidgetViewBase::ViewIsPinnable;
	else if (_flag == WidgetViewBase::toString(WidgetViewBase::ViewIsCentral)) return WidgetViewBase::ViewIsCentral;
	else if (_flag == WidgetViewBase::toString(WidgetViewBase::ViewIsSide)) return WidgetViewBase::ViewIsSide;
	else if (_flag == WidgetViewBase::toString(WidgetViewBase::ViewIsTool)) return WidgetViewBase::ViewIsTool;
	else {
		OT_LOG_EAS("Unknown view flag \"" + _flag + "\"");
		return NoViewFlags;
	}
}

std::list<std::string> ot::WidgetViewBase::toStringList(ViewFlags _flags) {
	std::list<std::string> ret;
	if (_flags & WidgetViewBase::ViewIsCloseable) ret.push_back(toString(WidgetViewBase::ViewIsCloseable));
	if (_flags & WidgetViewBase::ViewIsPinnable) ret.push_back(toString(WidgetViewBase::ViewIsPinnable));
	if (_flags & WidgetViewBase::ViewIsCentral) ret.push_back(toString(WidgetViewBase::ViewIsCentral));
	if (_flags & WidgetViewBase::ViewIsSide) ret.push_back(toString(WidgetViewBase::ViewIsSide));
	if (_flags & WidgetViewBase::ViewIsTool) ret.push_back(toString(WidgetViewBase::ViewIsTool));

	return ret;
}

ot::WidgetViewBase::ViewFlags ot::WidgetViewBase::stringListToViewFlags(const std::list<std::string>& _flags) {
	WidgetViewBase::ViewFlags ret = NoViewFlags;
	for (const std::string& flag : _flags) {
		ret |= WidgetViewBase::stringToViewFlag(flag);
	}

	return ret;
}

std::string ot::WidgetViewBase::toString(ViewDockLocation _dockLocation) {
	switch (_dockLocation)
	{
	case ot::WidgetViewBase::Default: return "Default";
	case ot::WidgetViewBase::Left: return "Left";
	case ot::WidgetViewBase::Top: return "Top";
	case ot::WidgetViewBase::Right: return "Right";
	case ot::WidgetViewBase::Bottom: return "Bottom";
	default:
		OT_LOG_EAS("Unknown view dock location (" + std::to_string(_dockLocation) + ")");
		return "Default";
	}
}

ot::WidgetViewBase::ViewDockLocation ot::WidgetViewBase::stringToDockLocation(const std::string& _dockLocation) {
	if (_dockLocation == WidgetViewBase::toString(WidgetViewBase::Default)) return WidgetViewBase::Default;
	else if (_dockLocation == WidgetViewBase::toString(WidgetViewBase::Left)) return WidgetViewBase::Left;
	else if (_dockLocation == WidgetViewBase::toString(WidgetViewBase::Top)) return WidgetViewBase::Top;
	else if (_dockLocation == WidgetViewBase::toString(WidgetViewBase::Right)) return WidgetViewBase::Right;
	else if (_dockLocation == WidgetViewBase::toString(WidgetViewBase::Bottom)) return WidgetViewBase::Bottom;
	else {
		OT_LOG_EAS("Unknown view dock location \"" + _dockLocation + "\"");
		return WidgetViewBase::Default;
	}
}

// ###########################################################################################################################################################################################################################################################################################################################

ot::WidgetViewBase::WidgetViewBase()
	: m_flags(WidgetViewBase::NoViewFlags), m_dockLocation(WidgetViewBase::Default)
{}

ot::WidgetViewBase::WidgetViewBase(const std::string& _nameAndTitle, ViewFlags _flags)
	: m_name(_nameAndTitle), m_title(_nameAndTitle), m_dockLocation(WidgetViewBase::Default), m_flags(_flags)
{}

ot::WidgetViewBase::WidgetViewBase(const std::string& _name, const std::string& _title, ViewFlags _flags) 
	: m_name(_name), m_title(_title), m_dockLocation(WidgetViewBase::Default), m_flags(_flags)
{}

ot::WidgetViewBase::WidgetViewBase(const std::string& _name, const std::string& _title, ViewDockLocation _dockLocation, ViewFlags _flags) 
	: m_name(_name), m_title(_title), m_dockLocation(_dockLocation), m_flags(_flags)
{}

ot::WidgetViewBase::WidgetViewBase(const WidgetViewBase& _other) {
	*this = _other;
}

ot::WidgetViewBase::~WidgetViewBase() {

}

ot::WidgetViewBase& ot::WidgetViewBase::operator = (const WidgetViewBase& _other) {
	m_name = _other.m_name;
	m_title = _other.m_title;
	m_flags = _other.m_flags;
	m_dockLocation = _other.m_dockLocation;

	return *this;
}

// ###########################################################################################################################################################################################################################################################################################################################

// Base class functions

void ot::WidgetViewBase::addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const {
	_object.AddMember("Name", JsonString(m_name, _allocator), _allocator);
	_object.AddMember("Title", JsonString(m_title, _allocator), _allocator);
	_object.AddMember("Flags", JsonArray(this->toStringList(m_flags), _allocator), _allocator);
	_object.AddMember("DockLocation", JsonString(this->toString(m_dockLocation), _allocator), _allocator);
}

void ot::WidgetViewBase::setFromJsonObject(const ot::ConstJsonObject& _object) {
	m_name = json::getString(_object, "Name");
	m_title = json::getString(_object, "Title");
	m_flags = this->stringListToViewFlags(json::getStringList(_object, "Flags"));
	m_dockLocation = this->stringToDockLocation(json::getString(_object, "DockLocation"));
}