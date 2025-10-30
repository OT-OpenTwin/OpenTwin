// @otlicense
// File: TableRange.cpp
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
#include "OTGui/TableRange.h"

ot::TableRange::TableRange()
	: m_topRow(-1), m_leftColumn(-1), m_bottomRow(-1), m_rightColumn(-1)
{

}

ot::TableRange::TableRange(int _topRow, int _leftColumn, int _bottomRow, int _rightColumn)
	: m_topRow(_topRow), m_leftColumn(_leftColumn), m_bottomRow(_bottomRow), m_rightColumn(_rightColumn) {
}

ot::TableRange::TableRange(const TableRange& other)
	: m_topRow(other.m_topRow), m_leftColumn(other.m_leftColumn), m_bottomRow(other.m_bottomRow), m_rightColumn(other.m_rightColumn) {
}

ot::TableRange& ot::TableRange::operator=(const TableRange& other) {
	m_topRow = other.m_topRow;
	m_bottomRow = other.m_bottomRow;
	m_leftColumn = other.m_leftColumn;
	m_rightColumn = other.m_rightColumn;
	return *this;
}

bool ot::TableRange::operator==(const TableRange& other) {
	return m_topRow == other.m_topRow && m_bottomRow == other.m_bottomRow && m_leftColumn == other.m_leftColumn && m_rightColumn == other.m_rightColumn;
}

bool ot::TableRange::operator!=(const TableRange& other) {
	return !(m_topRow == other.m_topRow && m_bottomRow == other.m_bottomRow && m_leftColumn == other.m_leftColumn && m_rightColumn == other.m_rightColumn);
}

void ot::TableRange::addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const {
	_object.AddMember("lC", m_leftColumn, _allocator);
	_object.AddMember("tR", m_topRow, _allocator);
	_object.AddMember("rC", m_rightColumn, _allocator);
	_object.AddMember("bR", m_bottomRow, _allocator);
}

void ot::TableRange::setFromJsonObject(const ConstJsonObject& _object) {
	m_topRow = ot::json::getInt(_object, "tR");
	m_bottomRow = ot::json::getInt(_object, "bR");
	m_leftColumn = ot::json::getInt(_object, "lC");
	m_rightColumn = ot::json::getInt(_object, "rC");
}
