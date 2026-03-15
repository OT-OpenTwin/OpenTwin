// @otlicense
// File: Grid.cpp
// 
// License:
// Copyright 2025 by OpenTwin
//  
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
//  
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// @otlicense-end

#pragma once

// OpenTwin header
#include "OTCore/Logging/LogDispatcher.h"
#include "OTGui/Painter/FillPainter2D.h"
#include "OTGui/Painter/StyleRefPainter2D.h"
#include "OTGui/Painter/Painter2DFactory.h"
#include "OTGui/Style/Grid.h"

// ###########################################################################################################################################################################################################################################################################################################################

// Static methods

std::string ot::Grid::toString(GridFlag _flag) {
	switch (_flag)
	{
	case Grid::NoGridFlags: return "null";
	case Grid::ShowNormalLines: return "ShowNormalLines";
	case Grid::ShowWideLines: return "ShowWideLines";
	case Grid::ShowCenterCross: return "ShowCenterCross";
	case Grid::AutoScaleGrid: return "AutoScaleGrid";
	default:
		OT_LOG_EAS("Invalid grid flag (" + std::to_string((int)_flag) + ")");
		return "null";
	}
}

ot::Grid::GridFlag ot::Grid::stringToGridFlag(const std::string& _flag) {
	if (_flag == Grid::toString(Grid::NoGridFlags)) return Grid::NoGridFlags;
	else if (_flag == Grid::toString(Grid::ShowNormalLines)) return Grid::ShowNormalLines;
	else if (_flag == Grid::toString(Grid::ShowWideLines)) return Grid::ShowWideLines;
	else if (_flag == Grid::toString(Grid::ShowCenterCross)) return Grid::ShowCenterCross;
	else if (_flag == Grid::toString(Grid::AutoScaleGrid)) return Grid::AutoScaleGrid;
	else {
		OT_LOG_E("Invalid grid flag \"" + _flag + "\"");
		return NoGridFlags;
	}
}

std::list<std::string> ot::Grid::toStringList(const GridFlags& _flags) {
	std::list<std::string> ret;

	if (_flags & Grid::ShowNormalLines) ret.push_back(Grid::toString(Grid::ShowNormalLines));
	if (_flags & Grid::ShowWideLines) ret.push_back(Grid::toString(Grid::ShowWideLines));
	if (_flags & Grid::ShowCenterCross) ret.push_back(Grid::toString(Grid::ShowCenterCross));
	if (_flags & Grid::AutoScaleGrid) ret.push_back(Grid::toString(Grid::AutoScaleGrid));

	return ret;
}

ot::Grid::GridFlags ot::Grid::stringListToGridFlags(const std::list<std::string>& _flags) {
	Grid::GridFlags ret = NoGridFlags;

	for (const std::string& flag : _flags) {
		ret |= Grid::stringToGridFlag(flag);
	}

	return ret;
}

std::string ot::Grid::toString(GridSnapMode _snapMode) {
	switch (_snapMode)
	{
	case Grid::NoGridSnap: return "NoGridSnap";
	case Grid::SnapTopLeft: return "SnapTopLeft";
	case Grid::SnapCenter: return "SnapCenter";
	default: 
		OT_LOG_EAS("Invalid GridSnapMode (" + std::to_string((int)_snapMode) + ")");
		return "NoGridSnap";
	}
}

ot::Grid::GridSnapMode ot::Grid::stringToGridSnapMode(const std::string& _snapMode) {
	if (_snapMode == Grid::toString(Grid::NoGridSnap)) return Grid::NoGridSnap;
	else if (_snapMode == Grid::toString(Grid::SnapTopLeft)) return Grid::SnapTopLeft;
	else if (_snapMode == Grid::toString(Grid::SnapCenter)) return Grid::SnapCenter;
	else {
		OT_LOG_EAS("Invalid GridSnapMode \"" + _snapMode + "\"");
		return Grid::NoGridSnap;
	}
}

// ###########################################################################################################################################################################################################################################################################################################################

// Constructor / Destructor

ot::Grid::Grid()
	: m_flags(NoGridFlags), m_gridStep(10, 10), m_gridWideEvery(10, 10), m_snapMode(NoGridSnap)
{}

ot::Grid::Grid(double _defaultGridLineWidth, const GridFlags& _flags, GridSnapMode _snapMode)
	: m_flags(_flags), m_gridStep(10, 10), m_gridWideEvery(10, 10), m_snapMode(_snapMode)
{}

ot::Grid::Grid(int _gridStep, int _gridWideEvery, const PenFCfg & _gridLineStyle, const GridFlags & _flags, GridSnapMode _snapMode)
	: m_flags(_flags), m_gridStep(_gridStep, _gridStep), m_gridWideEvery(_gridWideEvery, _gridWideEvery), m_snapMode(_snapMode), m_lineStyle(_gridLineStyle)
{

}

ot::Grid::Grid(const Point2D& _gridStep, const Point2D& _gridWideEvery, const PenFCfg& _gridLineStyle, const GridFlags& _flags, GridSnapMode _snapMode)
	: m_flags(_flags), m_gridStep(_gridStep), m_gridWideEvery(_gridWideEvery), m_snapMode(_snapMode), m_lineStyle(_gridLineStyle)
{}

ot::Grid::Grid(const Grid& _other) 
	: m_flags(_other.m_flags), m_gridStep(_other.m_gridStep), m_gridWideEvery(_other.m_gridWideEvery), m_snapMode(_other.m_snapMode), m_lineStyle(_other.m_lineStyle)
{}

ot::Grid::~Grid() {}

// ###########################################################################################################################################################################################################################################################################################################################

// Operators

ot::Grid& ot::Grid::operator = (const Grid& _other) {
	m_flags = _other.m_flags;
	m_snapMode = _other.m_snapMode;
	m_gridStep = _other.m_gridStep;
	m_gridWideEvery = _other.m_gridWideEvery;
	m_lineStyle = _other.m_lineStyle;

	return *this;
}

// ###########################################################################################################################################################################################################################################################################################################################

// Base class functions

void ot::Grid::addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const {
	_object.AddMember("Flags", JsonArray(Grid::toStringList(m_flags), _allocator), _allocator);
	_object.AddMember("SnapMode", JsonString(Grid::toString(m_snapMode), _allocator), _allocator);
	
	JsonObject gridStepObj;
	m_gridStep.addToJsonObject(gridStepObj, _allocator);
	_object.AddMember("GridStep", gridStepObj, _allocator);

	JsonObject gridWideObj;
	m_gridWideEvery.addToJsonObject(gridWideObj, _allocator);
	_object.AddMember("WideLineCounter", gridWideObj, _allocator);

	JsonObject lineStyleObj;
	m_lineStyle.addToJsonObject(lineStyleObj, _allocator);
	_object.AddMember("GridLines", lineStyleObj, _allocator);
}

void ot::Grid::setFromJsonObject(const ot::ConstJsonObject& _object) {
	m_flags = stringListToGridFlags(json::getStringList(_object, "Flags"));
	m_snapMode = stringToGridSnapMode(json::getString(_object, "SnapMode"));
	
	m_gridStep.setFromJsonObject(json::getObject(_object, "GridStep"));
	m_gridWideEvery.setFromJsonObject(json::getObject(_object, "WideLineCounter"));
	m_lineStyle.setFromJsonObject(json::getObject(_object, "GridLines"));
}

// ###########################################################################################################################################################################################################################################################################################################################

// Helper

ot::Point2D ot::Grid::snapToGrid(const Point2D& _pt) const {
	if (m_snapMode != Grid::NoGridSnap) return Grid::snapToGrid(_pt, m_gridStep);
	else return _pt;
}

ot::Point2DF ot::Grid::snapToGrid(const Point2DF& _pt) const {
	if (m_snapMode != Grid::NoGridSnap) return Grid::snapToGrid(_pt, m_gridStep);
	else return _pt;
}

ot::Point2DD ot::Grid::snapToGrid(const Point2DD& _pt) const {
	if (m_snapMode != Grid::NoGridSnap) return Grid::snapToGrid(_pt, m_gridStep);
	else return _pt;
}

ot::Point2D ot::Grid::snapToGrid(const Point2D& _pt, const Point2D& _gridStep) {
	Point2D pt = _pt;
	if (_gridStep.getX() > 0) {
		if (pt.getX() < 0.) {
			pt.setX(pt.getX() + (((int)pt.getX() * (-1)) % _gridStep.getX()));
		}
		else {
			pt.setX(pt.getX() - (((int)pt.getX()) % _gridStep.getX()));
		}
	}

	if (_gridStep.getY() > 0) {
		if (pt.getY() < 0.) {
			pt.setY(pt.getY() + (((int)pt.getY() * (-1)) % _gridStep.getY()));
		}
		else {
			pt.setY(pt.getY() - (((int)pt.getY()) % _gridStep.getY()));
		}
	}

	return pt;
}

ot::Point2DF ot::Grid::snapToGrid(const Point2DF& _pt, const Point2D& _gridStep) {
	Point2DF pt = _pt;
	if (_gridStep.getX() > 0) {
		pt.setX(std::round(pt.getX()));

		if (pt.getX() < 0.) {
			pt.setX(pt.getX() + (float)(((int)pt.getX() * (-1)) % _gridStep.getX()));
		}
		else {
			pt.setX(pt.getX() - (float)(((int)pt.getX()) % _gridStep.getX()));
		}
	}

	if (_gridStep.getY() > 0) {
		pt.setY(std::round(pt.getY()));

		if (pt.getY() < 0.) {
			pt.setY(pt.getY() + (float)(((int)pt.getY() * (-1)) % _gridStep.getY()));
		}
		else {
			pt.setY(pt.getY() - (float)(((int)pt.getY()) % _gridStep.getY()));
		}
	}

	return pt;
}

ot::Point2DD ot::Grid::snapToGrid(const Point2DD& _pt, const Point2D& _gridStep) {
	Point2DD pt = _pt;
	if (_gridStep.getX() > 0) {
		pt.setX(std::round(pt.getX()));

		if (pt.getX() < 0.) {
			pt.setX(pt.getX() + (double)(((int)pt.getX() * (-1)) % _gridStep.getX()));
		}
		else {
			pt.setX(pt.getX() - (double)(((int)pt.getX()) % _gridStep.getX()));
		}
	}

	if (_gridStep.getY() > 0) {
		pt.setY(std::round(pt.getY()));

		if (pt.getY() < 0.) {
			pt.setY(pt.getY() + (double)(((int)pt.getY() * (-1)) % _gridStep.getY()));
		}
		else {
			pt.setY(pt.getY() - (double)(((int)pt.getY()) % _gridStep.getY()));
		}
	}

	return pt;
}

// ###########################################################################################################################################################################################################################################################################################################################

// Setter / Getter
