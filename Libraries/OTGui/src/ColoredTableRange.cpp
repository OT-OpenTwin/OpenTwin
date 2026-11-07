// @otlicense
// File: ColoredTableRange.cpp
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

// OpenTwin header
#include "OTGui/ColoredTableRange.h"
#include "OTGui/TableRangeFactory.h"
#include "OTCore/Logging/Logger.h"

// std header
#include <cmath>
#include <algorithm>

ot::ColoredTableRange::ColoredTableRange()
	: m_range(std::make_shared<TableRangeSection>()),
	  m_color(),
	  m_colorTo(),
	  m_hasValueRange(false),
	  m_minValue(0.0),
	  m_maxValue(0.0),
	  m_hasColorMap(false),
	  m_colorMap()
{

}

ot::ColoredTableRange::ColoredTableRange(const TableRange& _range, const Color& _color)
	: m_range(_range.clone()),
	  m_color(_color),
	  m_colorTo(_color),
	  m_hasValueRange(false),
	  m_minValue(0.0),
	  m_maxValue(0.0),
	  m_hasColorMap(false),
	  m_colorMap()
{

}

ot::ColoredTableRange::ColoredTableRange(std::shared_ptr<TableRange> _range, const Color& _color)
	: m_range(_range),
	  m_color(_color),
	  m_colorTo(_color),
	  m_hasValueRange(false),
	  m_minValue(0.0),
	  m_maxValue(0.0),
	  m_hasColorMap(false),
	  m_colorMap()
{
	if (!m_range) {
		m_range = std::make_shared<TableRangeSection>();
	}
}

ot::ColoredTableRange::ColoredTableRange(const TableRange& _range, const Color& _color, double _min, double _max)
	: m_range(_range.clone()),
	  m_color(_color),
	  m_colorTo(_color),
	  m_hasValueRange(true),
	  m_minValue(_min),
	  m_maxValue(_max),
	  m_hasColorMap(false),
	  m_colorMap()
{

}

ot::ColoredTableRange::ColoredTableRange(std::shared_ptr<TableRange> _range, const Color& _color, double _min, double _max)
	: m_range(_range),
	  m_color(_color),
	  m_colorTo(_color),
	  m_hasValueRange(true),
	  m_minValue(_min),
	  m_maxValue(_max),
	  m_hasColorMap(false),
	  m_colorMap()
{
	if (!m_range) {
		m_range = std::make_shared<TableRangeSection>();
	}
}

ot::ColoredTableRange::ColoredTableRange(const TableRange& _range, const Color& _minColor, const Color& _maxColor, double _min, double _max)
	: m_range(_range.clone()),
	  m_color(_minColor),
	  m_colorTo(_maxColor),
	  m_hasValueRange(true),
	  m_minValue(_min),
	  m_maxValue(_max),
	  m_hasColorMap(false),
	  m_colorMap()
{

}

ot::ColoredTableRange::ColoredTableRange(std::shared_ptr<TableRange> _range, const Color& _minColor, const Color& _maxColor, double _min, double _max)
	: m_range(_range),
	  m_color(_minColor),
	  m_colorTo(_maxColor),
	  m_hasValueRange(true),
	  m_minValue(_min),
	  m_maxValue(_max),
	  m_hasColorMap(false),
	  m_colorMap()
{
	if (!m_range) {
		m_range = std::make_shared<TableRangeSection>();
	}
}

ot::ColoredTableRange::ColoredTableRange(const TableRange& _range, const ColorMap& _colorMap, double _min, double _max)
	: m_range(_range.clone()),
	  m_color(),
	  m_colorTo(),
	  m_hasValueRange(true),
	  m_minValue(_min),
	  m_maxValue(_max),
	  m_hasColorMap(true),
	  m_colorMap(_colorMap)
{
	if (!m_colorMap.getStops().empty()) {
		m_color = m_colorMap.getColor(0.0f);
		m_colorTo = m_colorMap.getColor(1.0f);
	}
}

ot::ColoredTableRange::ColoredTableRange(std::shared_ptr<TableRange> _range, const ColorMap& _colorMap, double _min, double _max)
	: m_range(_range),
	  m_color(),
	  m_colorTo(),
	  m_hasValueRange(true),
	  m_minValue(_min),
	  m_maxValue(_max),
	  m_hasColorMap(true),
	  m_colorMap(_colorMap)
{
	if (!m_range) {
		m_range = std::make_shared<TableRangeSection>();
	}
	if (!m_colorMap.getStops().empty()) {
		m_color = m_colorMap.getColor(0.0f);
		m_colorTo = m_colorMap.getColor(1.0f);
	}
}

ot::ColoredTableRange::ColoredTableRange(const ColoredTableRange& _other)
	: m_range(_other.m_range ? _other.m_range->clone() : new TableRangeSection()),
	  m_color(_other.m_color),
	  m_colorTo(_other.m_colorTo),
	  m_hasValueRange(_other.m_hasValueRange),
	  m_minValue(_other.m_minValue),
	  m_maxValue(_other.m_maxValue),
	  m_hasColorMap(_other.m_hasColorMap),
	  m_colorMap(_other.m_colorMap)
{

}

ot::ColoredTableRange::ColoredTableRange(ColoredTableRange&& _other) noexcept
	: m_range(std::move(_other.m_range)),
	  m_color(std::move(_other.m_color)),
	  m_colorTo(std::move(_other.m_colorTo)),
	  m_hasValueRange(_other.m_hasValueRange),
	  m_minValue(_other.m_minValue),
	  m_maxValue(_other.m_maxValue),
	  m_hasColorMap(_other.m_hasColorMap),
	  m_colorMap(std::move(_other.m_colorMap))
{
	if (!m_range) {
		m_range = std::make_shared<TableRangeSection>();
	}
}

ot::ColoredTableRange& ot::ColoredTableRange::operator=(const ColoredTableRange& _other) {
	if (this != &_other) {
		m_range.reset(_other.m_range ? _other.m_range->clone() : new TableRangeSection());
		m_color = _other.m_color;
		m_colorTo = _other.m_colorTo;
		m_hasValueRange = _other.m_hasValueRange;
		m_minValue = _other.m_minValue;
		m_maxValue = _other.m_maxValue;
		m_hasColorMap = _other.m_hasColorMap;
		m_colorMap = _other.m_colorMap;
	}
	return *this;
}

ot::ColoredTableRange& ot::ColoredTableRange::operator=(ColoredTableRange&& _other) noexcept {
	if (this != &_other) {
		m_range = std::move(_other.m_range);
		m_color = std::move(_other.m_color);
		m_colorTo = std::move(_other.m_colorTo);
		m_hasValueRange = _other.m_hasValueRange;
		m_minValue = _other.m_minValue;
		m_maxValue = _other.m_maxValue;
		m_hasColorMap = _other.m_hasColorMap;
		m_colorMap = std::move(_other.m_colorMap);
		if (!m_range) {
			m_range = std::make_shared<TableRangeSection>();
		}
	}
	return *this;
}

void ot::ColoredTableRange::setRange(const TableRange& _range) {
	m_range.reset(_range.clone());
}

void ot::ColoredTableRange::setRange(std::shared_ptr<TableRange> _range) {
	m_range = _range;
	if (!m_range) {
		m_range = std::make_shared<TableRangeSection>();
	}
}

bool ot::ColoredTableRange::operator==(const ColoredTableRange& _other) const {
	if (m_hasValueRange != _other.m_hasValueRange) {
		return false;
	}
	if (m_hasValueRange) {
		if (m_minValue != _other.m_minValue || m_maxValue != _other.m_maxValue) {
			return false;
		}
	}
	if (m_hasColorMap != _other.m_hasColorMap) {
		return false;
	}
	if (m_hasColorMap) {
		if (m_colorMap != _other.m_colorMap) {
			return false;
		}
	}
	else {
		if (m_color != _other.m_color || m_colorTo != _other.m_colorTo) {
			return false;
		}
	}
	if (!m_range && !_other.m_range) {
		return true;
	}
	if (!m_range || !_other.m_range) {
		return false;
	}
	return *m_range == *_other.m_range;
}

bool ot::ColoredTableRange::calculateColor(double _value, Color& _outColor) const {
	if (!m_hasValueRange) {
		_outColor = m_color;
		return true;
	}

	if (_value < m_minValue || _value > m_maxValue) {
		return false;
	}

	if (m_hasColorMap) {
		if (std::abs(m_maxValue - m_minValue) < 1e-12) {
			_outColor = m_colorMap.getColor(0.0f);
			return true;
		}

		double t = (_value - m_minValue) / (m_maxValue - m_minValue);
		if (t < 0.0) t = 0.0;
		if (t > 1.0) t = 1.0;

		_outColor = m_colorMap.getColor(static_cast<float>(t));
		return true;
	}

	if (m_color == m_colorTo) {
		_outColor = m_color;
		return true;
	}

	if (std::abs(m_maxValue - m_minValue) < 1e-12) {
		_outColor = m_color;
		return true;
	}

	double t = (_value - m_minValue) / (m_maxValue - m_minValue);
	if (t < 0.0) t = 0.0;
	if (t > 1.0) t = 1.0;

	_outColor = m_color.interpolate(m_colorTo, static_cast<float>(t), ColorInterpolationSpace::Oklab);
	return true;
}

void ot::ColoredTableRange::addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const {
	JsonObject rangeObj;
	if (m_range) {
		m_range->addToJsonObject(rangeObj, _allocator);
	}
	_object.AddMember(JsonString(c_coloredTableRangeRangeKey, _allocator), rangeObj, _allocator);

	JsonObject colorObj;
	m_color.addToJsonObject(colorObj, _allocator);
	_object.AddMember(JsonString(c_coloredTableRangeColorKey, _allocator), colorObj, _allocator);

	JsonObject colorToObj;
	m_colorTo.addToJsonObject(colorToObj, _allocator);
	_object.AddMember(JsonString(c_coloredTableRangeColorToKey, _allocator), colorToObj, _allocator);

	_object.AddMember(JsonString(c_coloredTableRangeHasValueRangeKey, _allocator), JsonValue(m_hasValueRange), _allocator);
	_object.AddMember(JsonString(c_coloredTableRangeMinValueKey, _allocator), JsonValue(m_minValue), _allocator);
	_object.AddMember(JsonString(c_coloredTableRangeMaxValueKey, _allocator), JsonValue(m_maxValue), _allocator);

	_object.AddMember(JsonString(c_coloredTableRangeHasColorMapKey, _allocator), JsonValue(m_hasColorMap), _allocator);
	if (m_hasColorMap) {
		JsonObject colorMapObj;
		m_colorMap.addToJsonObject(colorMapObj, _allocator);
		_object.AddMember(JsonString(c_coloredTableRangeColorMapKey, _allocator), colorMapObj, _allocator);
	}
}

void ot::ColoredTableRange::setFromJsonObject(const ConstJsonObject& _object) {
	ConstJsonObject rangeObj = ot::json::getObject(_object, c_coloredTableRangeRangeKey);
	TableRange* createdRange = TableRangeFactory::create(rangeObj);
	if (createdRange) {
		m_range.reset(createdRange);
	}
	else {
		m_range = std::make_shared<TableRangeSection>();
	}

	ConstJsonObject colorObj = ot::json::getObject(_object, c_coloredTableRangeColorKey);
	m_color.setFromJsonObject(colorObj);

	if (_object.HasMember(c_coloredTableRangeColorToKey)) {
		ConstJsonObject colorToObj = ot::json::getObject(_object, c_coloredTableRangeColorToKey);
		m_colorTo.setFromJsonObject(colorToObj);
	}
	else {
		m_colorTo = m_color;
	}

	if (_object.HasMember(c_coloredTableRangeHasValueRangeKey)) {
		m_hasValueRange = ot::json::getBool(_object, c_coloredTableRangeHasValueRangeKey);
	}
	else {
		m_hasValueRange = false;
	}

	if (_object.HasMember(c_coloredTableRangeMinValueKey)) {
		m_minValue = ot::json::getDouble(_object, c_coloredTableRangeMinValueKey);
	}
	else {
		m_minValue = 0.0;
	}

	if (_object.HasMember(c_coloredTableRangeMaxValueKey)) {
		m_maxValue = ot::json::getDouble(_object, c_coloredTableRangeMaxValueKey);
	}
	else {
		m_maxValue = 0.0;
	}

	if (_object.HasMember(c_coloredTableRangeHasColorMapKey)) {
		m_hasColorMap = ot::json::getBool(_object, c_coloredTableRangeHasColorMapKey);
	}
	else {
		m_hasColorMap = false;
	}

	if (m_hasColorMap && _object.HasMember(c_coloredTableRangeColorMapKey)) {
		ConstJsonObject colorMapObj = ot::json::getObject(_object, c_coloredTableRangeColorMapKey);
		m_colorMap.setFromJsonObject(colorMapObj);
	}
}
