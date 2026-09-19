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
#include "OTCore/Logging/Logger.h"

// std header
#include <stdexcept>

std::string ot::toString(TableRangeType _type) {
	switch (_type) {
	case TableRangeType::Table: return "Table";
	case TableRangeType::Section: return "Section";
	case TableRangeType::Column: return "Column";
	case TableRangeType::Row: return "Row";
	case TableRangeType::Cell: return "Cell";
	default:
		OT_LOG_E("Unknown table range type (" + std::to_string(static_cast<int>(_type)) + ")");
		return "Section";
	}
}

ot::TableRangeType ot::stringToTableRangeType(const std::string& _type) {
	if (_type == "Table") return TableRangeType::Table;
	else if (_type == "Section") return TableRangeType::Section;
	else if (_type == "Column") return TableRangeType::Column;
	else if (_type == "Row") return TableRangeType::Row;
	else if (_type == "Cell") return TableRangeType::Cell;
	else {
		OT_LOG_E("Unknown table range type (\"" + _type + "\")");
		return TableRangeType::Section;
	}
}

ot::TableRange::TableRange()
	: m_rangeType(TableRangeType::Section), m_topRow(-1), m_leftColumn(-1), m_bottomRow(-1), m_rightColumn(-1)
{

}

ot::TableRange::TableRange(int _topRow, int _leftColumn, int _bottomRow, int _rightColumn)
	: m_rangeType(TableRangeType::Section), m_topRow(_topRow), m_leftColumn(_leftColumn), m_bottomRow(_bottomRow), m_rightColumn(_rightColumn)
{

}

ot::TableRange::TableRange(TableRangeType _type)
	: m_rangeType(_type), m_topRow(-1), m_leftColumn(-1), m_bottomRow(-1), m_rightColumn(-1)
{

}

ot::TableRange::TableRange(TableRangeType _type, int _index)
	: m_rangeType(_type), m_topRow(-1), m_leftColumn(-1), m_bottomRow(-1), m_rightColumn(-1)
{
	if (_type == TableRangeType::Row) {
		m_topRow = _index;
		m_bottomRow = _index;
	}
	else if (_type == TableRangeType::Column) {
		m_leftColumn = _index;
		m_rightColumn = _index;
	}
}

ot::TableRange::TableRange(TableRangeType _type, int _row, int _column)
	: m_rangeType(_type), m_topRow(_row), m_leftColumn(_column), m_bottomRow(_row), m_rightColumn(_column)
{

}

ot::TableRange::TableRange(const TableRange& other)
	: m_rangeType(other.m_rangeType), m_topRow(other.m_topRow), m_leftColumn(other.m_leftColumn), m_bottomRow(other.m_bottomRow), m_rightColumn(other.m_rightColumn)
{

}

ot::TableRange& ot::TableRange::operator=(const TableRange& other) {
	if (this != &other) {
		m_rangeType = other.m_rangeType;
		m_topRow = other.m_topRow;
		m_bottomRow = other.m_bottomRow;
		m_leftColumn = other.m_leftColumn;
		m_rightColumn = other.m_rightColumn;
	}
	return *this;
}

bool ot::TableRange::operator==(const TableRange& other) const {
	if (getRangeType() != other.getRangeType()) {
		return false;
	}
	return isEqualTo(other);
}

bool ot::TableRange::operator!=(const TableRange& other) const {
	return !(*this == other);
}

ot::TableRange* ot::TableRange::clone() const {
	return new TableRange(*this);
}

bool ot::TableRange::isEqualTo(const TableRange& other) const {
	return m_topRow == other.m_topRow &&
	       m_bottomRow == other.m_bottomRow &&
	       m_leftColumn == other.m_leftColumn &&
	       m_rightColumn == other.m_rightColumn;
}

bool ot::TableRange::isInRange(int _row, int _column) const {
	switch (m_rangeType) {
	case TableRangeType::Table:
		return true;
	case TableRangeType::Section:
		return (_row >= m_topRow && _row <= m_bottomRow && _column >= m_leftColumn && _column <= m_rightColumn);
	case TableRangeType::Column:
		return (_column >= m_leftColumn && _column <= m_rightColumn);
	case TableRangeType::Row:
		return (_row >= m_topRow && _row <= m_bottomRow);
	case TableRangeType::Cell:
		return (_row == m_topRow && _column == m_leftColumn);
	default:
		OT_LOG_E("Unknown table range type (" + std::to_string(static_cast<int>(m_rangeType)) + ")");
		return false;
	}
}

void ot::TableRange::addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const {
	_object.AddMember(JsonString(c_tableRangeTypeKey, _allocator), JsonString(toString(m_rangeType), _allocator), _allocator);
	_object.AddMember(JsonString(c_tableRangeLeftColumnKey, _allocator), JsonValue(m_leftColumn), _allocator);
	_object.AddMember(JsonString(c_tableRangeTopRowKey, _allocator), JsonValue(m_topRow), _allocator);
	_object.AddMember(JsonString(c_tableRangeRightColumnKey, _allocator), JsonValue(m_rightColumn), _allocator);
	_object.AddMember(JsonString(c_tableRangeBottomRowKey, _allocator), JsonValue(m_bottomRow), _allocator);
}

void ot::TableRange::setFromJsonObject(const ConstJsonObject& _object) {
	if (_object.HasMember(c_tableRangeTypeKey)) {
		m_rangeType = stringToTableRangeType(ot::json::getString(_object, c_tableRangeTypeKey));
	}
	else {
		m_rangeType = TableRangeType::Section;
	}
	m_topRow = ot::json::getInt(_object, c_tableRangeTopRowKey);
	m_bottomRow = ot::json::getInt(_object, c_tableRangeBottomRowKey);
	m_leftColumn = ot::json::getInt(_object, c_tableRangeLeftColumnKey);
	m_rightColumn = ot::json::getInt(_object, c_tableRangeRightColumnKey);
}

// =========================================================================================
// TableRangeTable
// =========================================================================================

ot::TableRangeTable::TableRangeTable()
	: TableRange(TableRangeType::Table)
{

}

void ot::TableRangeTable::addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const {
	_object.AddMember(JsonString(c_tableRangeTypeKey, _allocator), JsonString(toString(TableRangeType::Table), _allocator), _allocator);
}

void ot::TableRangeTable::setFromJsonObject(const ConstJsonObject& _object) {
	// Entire table range has no coordinates
	m_rangeType = TableRangeType::Table;
	m_topRow = -1;
	m_bottomRow = -1;
	m_leftColumn = -1;
	m_rightColumn = -1;
}

// =========================================================================================
// TableRangeSection
// =========================================================================================

ot::TableRangeSection::TableRangeSection()
	: TableRange(TableRangeType::Section)
{

}

ot::TableRangeSection::TableRangeSection(int _topRow, int _leftColumn, int _bottomRow, int _rightColumn)
	: TableRange(_topRow, _leftColumn, _bottomRow, _rightColumn)
{

}

bool ot::TableRangeSection::isInRange(int _row, int _column) const {
	return (_row >= m_topRow && _row <= m_bottomRow && _column >= m_leftColumn && _column <= m_rightColumn);
}

bool ot::TableRangeSection::isEqualTo(const TableRange& other) const {
	return m_topRow == other.getTopRow() &&
	       m_bottomRow == other.getBottomRow() &&
	       m_leftColumn == other.getLeftColumn() &&
	       m_rightColumn == other.getRightColumn();
}

void ot::TableRangeSection::addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const {
	_object.AddMember(JsonString(c_tableRangeTypeKey, _allocator), JsonString(toString(TableRangeType::Section), _allocator), _allocator);
	_object.AddMember(JsonString(c_tableRangeLeftColumnKey, _allocator), JsonValue(m_leftColumn), _allocator);
	_object.AddMember(JsonString(c_tableRangeTopRowKey, _allocator), JsonValue(m_topRow), _allocator);
	_object.AddMember(JsonString(c_tableRangeRightColumnKey, _allocator), JsonValue(m_rightColumn), _allocator);
	_object.AddMember(JsonString(c_tableRangeBottomRowKey, _allocator), JsonValue(m_bottomRow), _allocator);
}

void ot::TableRangeSection::setFromJsonObject(const ConstJsonObject& _object) {
	m_rangeType = TableRangeType::Section;
	m_topRow = ot::json::getInt(_object, c_tableRangeTopRowKey);
	m_bottomRow = ot::json::getInt(_object, c_tableRangeBottomRowKey);
	m_leftColumn = ot::json::getInt(_object, c_tableRangeLeftColumnKey);
	m_rightColumn = ot::json::getInt(_object, c_tableRangeRightColumnKey);
}

// =========================================================================================
// TableRangeColumn
// =========================================================================================

ot::TableRangeColumn::TableRangeColumn()
	: TableRange(TableRangeType::Column)
{

}

ot::TableRangeColumn::TableRangeColumn(int _column)
	: TableRange(TableRangeType::Column, _column)
{

}

ot::TableRangeColumn::TableRangeColumn(int _leftColumn, int _rightColumn)
	: TableRange(TableRangeType::Column)
{
	m_leftColumn = _leftColumn;
	m_rightColumn = _rightColumn;
}

bool ot::TableRangeColumn::isInRange(int _row, int _column) const {
	return (_column >= m_leftColumn && _column <= m_rightColumn);
}

bool ot::TableRangeColumn::isEqualTo(const TableRange& other) const {
	return m_leftColumn == other.getLeftColumn() && m_rightColumn == other.getRightColumn();
}

void ot::TableRangeColumn::addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const {
	_object.AddMember(JsonString(c_tableRangeTypeKey, _allocator), JsonString(toString(TableRangeType::Column), _allocator), _allocator);
	_object.AddMember(JsonString(c_tableRangeLeftColumnKey, _allocator), JsonValue(m_leftColumn), _allocator);
	_object.AddMember(JsonString(c_tableRangeRightColumnKey, _allocator), JsonValue(m_rightColumn), _allocator);
}

void ot::TableRangeColumn::setFromJsonObject(const ConstJsonObject& _object) {
	m_rangeType = TableRangeType::Column;
	m_topRow = -1;
	m_bottomRow = -1;
	m_leftColumn = ot::json::getInt(_object, c_tableRangeLeftColumnKey);
	if (_object.HasMember(c_tableRangeRightColumnKey)) {
		m_rightColumn = ot::json::getInt(_object, c_tableRangeRightColumnKey);
	}
	else {
		m_rightColumn = m_leftColumn;
	}
}

// =========================================================================================
// TableRangeRow
// =========================================================================================

ot::TableRangeRow::TableRangeRow()
	: TableRange(TableRangeType::Row)
{

}

ot::TableRangeRow::TableRangeRow(int _row)
	: TableRange(TableRangeType::Row, _row)
{

}

ot::TableRangeRow::TableRangeRow(int _topRow, int _bottomRow)
	: TableRange(TableRangeType::Row)
{
	m_topRow = _topRow;
	m_bottomRow = _bottomRow;
}

bool ot::TableRangeRow::isInRange(int _row, int _column) const {
	return (_row >= m_topRow && _row <= m_bottomRow);
}

bool ot::TableRangeRow::isEqualTo(const TableRange& other) const {
	return m_topRow == other.getTopRow() && m_bottomRow == other.getBottomRow();
}

void ot::TableRangeRow::addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const {
	_object.AddMember(JsonString(c_tableRangeTypeKey, _allocator), JsonString(toString(TableRangeType::Row), _allocator), _allocator);
	_object.AddMember(JsonString(c_tableRangeTopRowKey, _allocator), JsonValue(m_topRow), _allocator);
	_object.AddMember(JsonString(c_tableRangeBottomRowKey, _allocator), JsonValue(m_bottomRow), _allocator);
}

void ot::TableRangeRow::setFromJsonObject(const ConstJsonObject& _object) {
	m_rangeType = TableRangeType::Row;
	m_leftColumn = -1;
	m_rightColumn = -1;
	m_topRow = ot::json::getInt(_object, c_tableRangeTopRowKey);
	if (_object.HasMember(c_tableRangeBottomRowKey)) {
		m_bottomRow = ot::json::getInt(_object, c_tableRangeBottomRowKey);
	}
	else {
		m_bottomRow = m_topRow;
	}
}

// =========================================================================================
// TableRangeCell
// =========================================================================================

ot::TableRangeCell::TableRangeCell()
	: TableRange(TableRangeType::Cell)
{

}

ot::TableRangeCell::TableRangeCell(int _row, int _column)
	: TableRange(TableRangeType::Cell, _row, _column)
{

}

bool ot::TableRangeCell::isInRange(int _row, int _column) const {
	return (_row == m_topRow && _column == m_leftColumn);
}

bool ot::TableRangeCell::isEqualTo(const TableRange& other) const {
	return m_topRow == other.getTopRow() && m_leftColumn == other.getLeftColumn();
}

void ot::TableRangeCell::addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const {
	_object.AddMember(JsonString(c_tableRangeTypeKey, _allocator), JsonString(toString(TableRangeType::Cell), _allocator), _allocator);
	_object.AddMember(JsonString(c_tableRangeLeftColumnKey, _allocator), JsonValue(m_leftColumn), _allocator);
	_object.AddMember(JsonString(c_tableRangeTopRowKey, _allocator), JsonValue(m_topRow), _allocator);
}

void ot::TableRangeCell::setFromJsonObject(const ConstJsonObject& _object) {
	m_rangeType = TableRangeType::Cell;
	m_topRow = ot::json::getInt(_object, c_tableRangeTopRowKey);
	m_bottomRow = m_topRow;
	m_leftColumn = ot::json::getInt(_object, c_tableRangeLeftColumnKey);
	m_rightColumn = m_leftColumn;
}

