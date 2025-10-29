// @otlicense

//! @file WidgetViewBase.cpp
//! @author Alexander Kuester (alexk95)
//! @date April 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/LogDispatcher.h"
#include "OTGui/WidgetViewBase.h"

std::string ot::WidgetViewBase::toString(ViewFlag _flag) {
	switch (_flag)
	{
	case WidgetViewBase::ViewIsCloseable: return "ViewIsCloseable";
	case WidgetViewBase::ViewDefaultCloseHandling: return "ViewDefaultCloseHandling";
	case WidgetViewBase::ViewIsPinnable: return "ViewIsPinnable";
	case WidgetViewBase::ViewIsCentral: return "ViewIsCentral";
	case WidgetViewBase::ViewIsSide: return "ViewIsSide";
	case WidgetViewBase::ViewIsTool: return "ViewIsTool";
	case WidgetViewBase::ViewNameAsTitle: return "NameAsTitle";
	case WidgetViewBase::ViewCloseOnEmptySelection: return "ViewCloseOnEmptySelection";
	default:
		OT_LOG_EAS("Unknown view flag (" + std::to_string((int)_flag) + ")");
		return "<null>";
	}
}

ot::WidgetViewBase::ViewFlag ot::WidgetViewBase::stringToViewFlag(const std::string& _flag) {
	if (_flag == WidgetViewBase::toString(WidgetViewBase::ViewIsCloseable)) return WidgetViewBase::ViewIsCloseable;
	else if (_flag == WidgetViewBase::toString(WidgetViewBase::ViewDefaultCloseHandling)) return WidgetViewBase::ViewDefaultCloseHandling;
	else if (_flag == WidgetViewBase::toString(WidgetViewBase::ViewIsPinnable)) return WidgetViewBase::ViewIsPinnable;
	else if (_flag == WidgetViewBase::toString(WidgetViewBase::ViewIsCentral)) return WidgetViewBase::ViewIsCentral;
	else if (_flag == WidgetViewBase::toString(WidgetViewBase::ViewIsSide)) return WidgetViewBase::ViewIsSide;
	else if (_flag == WidgetViewBase::toString(WidgetViewBase::ViewIsTool)) return WidgetViewBase::ViewIsTool;
	else if (_flag == WidgetViewBase::toString(WidgetViewBase::ViewNameAsTitle)) return WidgetViewBase::ViewNameAsTitle;
	else if (_flag == WidgetViewBase::toString(WidgetViewBase::ViewCloseOnEmptySelection)) return WidgetViewBase::ViewCloseOnEmptySelection;
	else {
		OT_LOG_EAS("Unknown view flag \"" + _flag + "\"");
		return NoViewFlags;
	}
}

std::list<std::string> ot::WidgetViewBase::toStringList(ViewFlags _flags) {
	std::list<std::string> ret;
	if (_flags & WidgetViewBase::ViewIsCloseable) ret.push_back(toString(WidgetViewBase::ViewIsCloseable));
	if (_flags & WidgetViewBase::ViewDefaultCloseHandling) ret.push_back(toString(WidgetViewBase::ViewDefaultCloseHandling));
	if (_flags & WidgetViewBase::ViewIsPinnable) ret.push_back(toString(WidgetViewBase::ViewIsPinnable));
	if (_flags & WidgetViewBase::ViewIsCentral) ret.push_back(toString(WidgetViewBase::ViewIsCentral));
	if (_flags & WidgetViewBase::ViewIsSide) ret.push_back(toString(WidgetViewBase::ViewIsSide));
	if (_flags & WidgetViewBase::ViewIsTool) ret.push_back(toString(WidgetViewBase::ViewIsTool));
	if (_flags & WidgetViewBase::ViewNameAsTitle) ret.push_back(toString(WidgetViewBase::ViewNameAsTitle));
	if (_flags & WidgetViewBase::ViewCloseOnEmptySelection) ret.push_back(toString(WidgetViewBase::ViewCloseOnEmptySelection));
	
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
		OT_LOG_EAS("Unknown view dock location (" + std::to_string((int)_dockLocation) + ")");
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

std::string ot::WidgetViewBase::toString(ViewType _type) {
	switch (_type) {
	case ot::WidgetViewBase::View3D: return "3D";
	case ot::WidgetViewBase::View1D: return "1D";
	case ot::WidgetViewBase::ViewText: return "Text";
	case ot::WidgetViewBase::ViewTable: return "Table";
	case ot::WidgetViewBase::ViewVersion: return "Version";
	case ot::WidgetViewBase::ViewGraphics: return "Graphics";
	case ot::WidgetViewBase::ViewGraphicsPicker: return "GraphicsPicker";
	case ot::WidgetViewBase::ViewProperties: return "Properties";
	case ot::WidgetViewBase::ViewNavigation: return "Navigation";
	case ot::WidgetViewBase::ViewPDF: return "PDF";
	case ot::WidgetViewBase::CustomView: return "Custom";
	default:
		OT_LOG_EAS("Unknown view type (" + std::to_string((int)_type) + ")");
		return "Custom";
	}
}

ot::WidgetViewBase::ViewType ot::WidgetViewBase::stringToViewType(const std::string& _type) {
	if (_type == WidgetViewBase::toString(ViewType::View3D)) return ViewType::View3D;
	else if (_type == WidgetViewBase::toString(ViewType::View1D)) return ViewType::View1D;
	else if (_type == WidgetViewBase::toString(ViewType::ViewText)) return ViewType::ViewText;
	else if (_type == WidgetViewBase::toString(ViewType::ViewTable)) return ViewType::ViewTable;
	else if (_type == WidgetViewBase::toString(ViewType::ViewVersion)) return ViewType::ViewVersion;
	else if (_type == WidgetViewBase::toString(ViewType::ViewGraphics)) return ViewType::ViewGraphics;
	else if (_type == WidgetViewBase::toString(ViewType::ViewGraphicsPicker)) return ViewType::ViewGraphicsPicker;
	else if (_type == WidgetViewBase::toString(ViewType::ViewProperties)) return ViewType::ViewProperties;
	else if (_type == WidgetViewBase::toString(ViewType::ViewNavigation)) return ViewType::ViewNavigation;
	else if (_type == WidgetViewBase::toString(ViewType::ViewPDF)) return ViewType::ViewPDF;
	else if (_type == WidgetViewBase::toString(ViewType::CustomView)) return ViewType::CustomView;
	else {
		OT_LOG_EAS("Unknown view type \"" + _type + "\"");
		return WidgetViewBase::CustomView;
	}
}

// ###########################################################################################################################################################################################################################################################################################################################

ot::WidgetViewBase::WidgetViewBase(ViewType _type, ViewFlags _flags)
	: m_flags(_flags), m_dockLocation(WidgetViewBase::Default), m_type(_type)
{}

ot::WidgetViewBase::WidgetViewBase(const std::string& _entityName, const std::string& _title, ViewType _type, ViewFlags _flags)
	: BasicEntityInformation(_entityName, 0, 0), m_title(_title), m_dockLocation(WidgetViewBase::Default), m_flags(_flags), m_type(_type)
{}

ot::WidgetViewBase::WidgetViewBase(const std::string& _entityName, const std::string& _title, ViewDockLocation _dockLocation, ViewType _type, ViewFlags _flags)
	: BasicEntityInformation(_entityName, 0, 0), m_title(_title), m_dockLocation(_dockLocation), m_flags(_flags), m_type(_type)
{}

ot::WidgetViewBase::WidgetViewBase(const WidgetViewBase& _other) {
	*this = _other;
}

ot::WidgetViewBase::~WidgetViewBase() {

}

ot::WidgetViewBase& ot::WidgetViewBase::operator = (const WidgetViewBase& _other) {
	BasicEntityInformation::operator=(_other);

	if (this != &_other) {
		m_title = _other.m_title;
		m_flags = _other.m_flags;
		m_dockLocation = _other.m_dockLocation;
		m_type = _other.m_type;
	}
	return *this;
}

bool ot::WidgetViewBase::operator==(const WidgetViewBase& _other) const {
	return (BasicEntityInformation::operator==(_other)) &&
		(m_title == _other.m_title) &&
		(m_flags == _other.m_flags) &&
		(m_dockLocation == _other.m_dockLocation) &&
		(m_type == _other.m_type);
}

bool ot::WidgetViewBase::operator!=(const WidgetViewBase& _other) const {
	return !WidgetViewBase::operator==(_other);
}

// ###########################################################################################################################################################################################################################################################################################################################

// Base class functions

void ot::WidgetViewBase::addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const {
	BasicEntityInformation::addToJsonObject(_object, _allocator);

	_object.AddMember("ViewTitle", JsonString(m_title, _allocator), _allocator);
	_object.AddMember("ViewType", JsonString(this->toString(m_type), _allocator), _allocator);
	_object.AddMember("ViewFlags", JsonArray(this->toStringList(m_flags), _allocator), _allocator);
	_object.AddMember("DockLocation", JsonString(this->toString(m_dockLocation), _allocator), _allocator);
}

void ot::WidgetViewBase::setFromJsonObject(const ot::ConstJsonObject& _object) {
	BasicEntityInformation::setFromJsonObject(_object);

	m_title = json::getString(_object, "ViewTitle");
	m_type = this->stringToViewType(json::getString(_object, "ViewType"));
	m_flags = this->stringListToViewFlags(json::getStringList(_object, "ViewFlags"));
	m_dockLocation = this->stringToDockLocation(json::getString(_object, "DockLocation"));
}

void ot::WidgetViewBase::setEntityInformation(const BasicEntityInformation& _info) {
	BasicEntityInformation::operator=(_info);
}
