/*****************************************************************//**
 * \file   TableRange.cpp
 * \brief
 *
 * \author Wagner
 * \date   February 2023
 *********************************************************************/

// OpenTwin header
#include "OTGui/TableRange.h"

ot::TableRange::TableRange() {

}

ot::TableRange::TableRange(int topRow, int bottomRow, int leftColumn, int rightColumn) :
	m_topRow(topRow), m_bottomRow(bottomRow), m_leftColumn(leftColumn), m_rightColumn(rightColumn) {
}

ot::TableRange::TableRange(const TableRange& other)
	: m_topRow(other.m_topRow), m_bottomRow(other.m_bottomRow), m_leftColumn(other.m_leftColumn), m_rightColumn(other.m_rightColumn) {
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