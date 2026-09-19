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

ot::ColoredTableRange::ColoredTableRange()
	: m_range(std::make_shared<TableRangeSection>()), m_color()
{

}

ot::ColoredTableRange::ColoredTableRange(const TableRange& _range, const Color& _color)
	: m_range(_range.clone()), m_color(_color)
{

}

ot::ColoredTableRange::ColoredTableRange(std::shared_ptr<TableRange> _range, const Color& _color)
	: m_range(_range), m_color(_color)
{
	if (!m_range) {
		m_range = std::make_shared<TableRangeSection>();
	}
}

ot::ColoredTableRange::ColoredTableRange(const ColoredTableRange& _other)
	: m_range(_other.m_range ? _other.m_range->clone() : new TableRangeSection()), m_color(_other.m_color)
{

}

ot::ColoredTableRange::ColoredTableRange(ColoredTableRange&& _other) noexcept
	: m_range(std::move(_other.m_range)), m_color(std::move(_other.m_color))
{
	if (!m_range) {
		m_range = std::make_shared<TableRangeSection>();
	}
}

ot::ColoredTableRange& ot::ColoredTableRange::operator=(const ColoredTableRange& _other) {
	if (this != &_other) {
		m_range.reset(_other.m_range ? _other.m_range->clone() : new TableRangeSection());
		m_color = _other.m_color;
	}
	return *this;
}

ot::ColoredTableRange& ot::ColoredTableRange::operator=(ColoredTableRange&& _other) noexcept {
	if (this != &_other) {
		m_range = std::move(_other.m_range);
		m_color = std::move(_other.m_color);
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
	if (!m_range && !_other.m_range) {
		return m_color == _other.m_color;
	}
	if (!m_range || !_other.m_range) {
		return false;
	}
	return *m_range == *_other.m_range && m_color == _other.m_color;
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
}

