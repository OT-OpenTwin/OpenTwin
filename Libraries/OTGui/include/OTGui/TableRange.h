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

	class OT_GUI_API_EXPORT TableRange : public ot::Serializable {
	public:
		TableRange();
		TableRange(int _topRow, int _leftColumn, int _bottomRow, int _rightColumn);
		TableRange(TableRangeType _type);
		TableRange(TableRangeType _type, int _index);
		TableRange(TableRangeType _type, int _row, int _column);

		TableRange(const TableRange& other);
		TableRange& operator=(const TableRange& other);
		bool operator==(const TableRange& other) const;
		bool operator!=(const TableRange& other) const;

		// Inherited via Serializable

		virtual void addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const override;
		virtual void setFromJsonObject(const ConstJsonObject& _object) override;

		TableRangeType getRangeType() const { return m_rangeType; }
		void setRangeType(TableRangeType _type) { m_rangeType = _type; }

		bool isInRange(int _row, int _column) const;
		bool isInRange(uint64_t _column, uint64_t _row) const { return isInRange(static_cast<int>(_row), static_cast<int>(_column)); }

		int getTopRow() const { return m_topRow; }
		int getBottomRow() const { return m_bottomRow; }
		int getLeftColumn() const { return m_leftColumn; }
		int getRightColumn() const { return m_rightColumn; }

		void setTopRow(int _topRow) { m_topRow = _topRow; }
		void setBottomRow(int _bottomRow) { m_bottomRow = _bottomRow; }
		void setLeftColumn(int _leftColumn) { m_leftColumn = _leftColumn; }
		void setRightColumn(int _rightColumn) { m_rightColumn = _rightColumn; }

	private:
		TableRangeType m_rangeType;
		int m_topRow;
		int m_bottomRow;
		int m_leftColumn;
		int m_rightColumn;
	};
}