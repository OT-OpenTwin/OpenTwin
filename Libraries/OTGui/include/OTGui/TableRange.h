// @otlicense
// File: TableRange.h
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

 // OT header
#include "OTCore/Serializable.h"
#include "OTGui/OTGuiAPIExport.h"

// std header
#include <string>
#include <cstdint>

namespace ot {

	enum class TableRangeType {
		Table,
		Section,
		Column,
		Row,
		Cell
	};

	OT_GUI_API_EXPORT std::string toString(TableRangeType _type);
	OT_GUI_API_EXPORT TableRangeType stringToTableRangeType(const std::string& _type);

	// Serialization keys - no defines
	inline constexpr const char* c_tableRangeTypeKey = "Type";
	inline constexpr const char* c_tableRangeLeftColumnKey = "lC";
	inline constexpr const char* c_tableRangeTopRowKey = "tR";
	inline constexpr const char* c_tableRangeRightColumnKey = "rC";
	inline constexpr const char* c_tableRangeBottomRowKey = "bR";

	//! @class TableRange
	//! @brief Base class for table ranges. Concrete subclasses provide specialized behavior for specific range types.
	//! This class is also directly instantiable for full backward compatibility.
	class OT_GUI_API_EXPORT TableRange : public ot::Serializable {
	public:
		TableRange();
		TableRange(int _topRow, int _leftColumn, int _bottomRow, int _rightColumn);
		TableRange(TableRangeType _type);
		TableRange(TableRangeType _type, int _index);
		TableRange(TableRangeType _type, int _row, int _column);

		TableRange(const TableRange& other);
		TableRange& operator=(const TableRange& other);
		virtual ~TableRange() = default;

		bool operator==(const TableRange& other) const;
		bool operator!=(const TableRange& other) const;

		// Virtual methods for polymorphism
		virtual TableRangeType getRangeType() const { return m_rangeType; }
		virtual void setRangeType(TableRangeType _type) { m_rangeType = _type; }

		virtual bool isInRange(int _row, int _column) const;
		bool isInRange(uint64_t _column, uint64_t _row) const { return isInRange(static_cast<int>(_row), static_cast<int>(_column)); }

		virtual TableRange* clone() const;
		virtual bool isEqualTo(const TableRange& other) const;

		// Inherited via Serializable
		virtual void addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const override;
		virtual void setFromJsonObject(const ConstJsonObject& _object) override;

		virtual int getTopRow() const { return m_topRow; }
		virtual int getBottomRow() const { return m_bottomRow; }
		virtual int getLeftColumn() const { return m_leftColumn; }
		virtual int getRightColumn() const { return m_rightColumn; }

		virtual void setTopRow(int _topRow) { m_topRow = _topRow; }
		virtual void setBottomRow(int _bottomRow) { m_bottomRow = _bottomRow; }
		virtual void setLeftColumn(int _leftColumn) { m_leftColumn = _leftColumn; }
		virtual void setRightColumn(int _rightColumn) { m_rightColumn = _rightColumn; }

	protected:
		TableRangeType m_rangeType;
		int m_topRow;
		int m_bottomRow;
		int m_leftColumn;
		int m_rightColumn;
	};

	//! @class TableRangeTable
	//! @brief Represents the entire table. Covers all rows and columns.
	class OT_GUI_API_EXPORT TableRangeTable : public TableRange {
	public:
		TableRangeTable();

		virtual TableRangeType getRangeType() const override { return TableRangeType::Table; }
		virtual bool isInRange(int _row, int _column) const override { return true; }
		virtual TableRange* clone() const override { return new TableRangeTable(*this); }
		virtual bool isEqualTo(const TableRange& other) const override { return true; }

		virtual void addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const override;
		virtual void setFromJsonObject(const ConstJsonObject& _object) override;
	};

	//! @class TableRangeSection
	//! @brief Represents a rectangular section (box) of cells defined by top-left and bottom-right coordinates.
	class OT_GUI_API_EXPORT TableRangeSection : public TableRange {
	public:
		TableRangeSection();
		TableRangeSection(int _topRow, int _leftColumn, int _bottomRow, int _rightColumn);

		virtual TableRangeType getRangeType() const override { return TableRangeType::Section; }
		virtual bool isInRange(int _row, int _column) const override;
		virtual TableRange* clone() const override { return new TableRangeSection(*this); }
		virtual bool isEqualTo(const TableRange& other) const override;

		virtual void addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const override;
		virtual void setFromJsonObject(const ConstJsonObject& _object) override;
	};

	//! @class TableRangeColumn
	//! @brief Represents one or more entire columns.
	class OT_GUI_API_EXPORT TableRangeColumn : public TableRange {
	public:
		TableRangeColumn();
		TableRangeColumn(int _column);
		TableRangeColumn(int _leftColumn, int _rightColumn);

		int getColumn() const { return m_leftColumn; }
		void setColumn(int _column) { m_leftColumn = _column; m_rightColumn = _column; }

		virtual TableRangeType getRangeType() const override { return TableRangeType::Column; }
		virtual bool isInRange(int _row, int _column) const override;
		virtual TableRange* clone() const override { return new TableRangeColumn(*this); }
		virtual bool isEqualTo(const TableRange& other) const override;

		virtual void addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const override;
		virtual void setFromJsonObject(const ConstJsonObject& _object) override;
	};

	//! @class TableRangeRow
	//! @brief Represents one or more entire rows.
	class OT_GUI_API_EXPORT TableRangeRow : public TableRange {
	public:
		TableRangeRow();
		TableRangeRow(int _row);
		TableRangeRow(int _topRow, int _bottomRow);

		int getRow() const { return m_topRow; }
		void setRow(int _row) { m_topRow = _row; m_bottomRow = _row; }

		virtual TableRangeType getRangeType() const override { return TableRangeType::Row; }
		virtual bool isInRange(int _row, int _column) const override;
		virtual TableRange* clone() const override { return new TableRangeRow(*this); }
		virtual bool isEqualTo(const TableRange& other) const override;

		virtual void addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const override;
		virtual void setFromJsonObject(const ConstJsonObject& _object) override;
	};

	//! @class TableRangeCell
	//! @brief Represents a single table cell.
	class OT_GUI_API_EXPORT TableRangeCell : public TableRange {
	public:
		TableRangeCell();
		TableRangeCell(int _row, int _column);

		int getRow() const { return m_topRow; }
		int getColumn() const { return m_leftColumn; }
		void setRow(int _row) { m_topRow = _row; m_bottomRow = _row; }
		void setColumn(int _column) { m_leftColumn = _column; m_rightColumn = _column; }

		virtual TableRangeType getRangeType() const override { return TableRangeType::Cell; }
		virtual bool isInRange(int _row, int _column) const override;
		virtual TableRange* clone() const override { return new TableRangeCell(*this); }
		virtual bool isEqualTo(const TableRange& other) const override;

		virtual void addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const override;
		virtual void setFromJsonObject(const ConstJsonObject& _object) override;
	};
}