// @otlicense
// File: TableCfg.h
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
#include "OTCore/DataStruct/GenericDataStructMatrix.h"
#include "OTGui/Widgets/WidgetViewBase.h"
#include "OTGui/Widgets/TableHeaderItemCfg.h"

// std header
#include <string>
#include <vector>

namespace ot {

	class OT_GUI_API_EXPORT TableCfg : public WidgetViewBase {
	public:
		enum class TableHeaderMode {
			NoHeader,
			Horizontal,
			Vertical
		};

		static std::string toString(TableHeaderMode _headerMode);
		static TableHeaderMode stringToHeaderMode(const std::string& _headerMode);

		TableCfg(int _rows = 0, int _columns = 0, WidgetViewBase _baseInfo = WidgetViewBase(WidgetViewBase::ViewTable, WidgetViewBase::ViewIsCentral | WidgetViewBase::ViewIsCloseable | WidgetViewBase::ViewIsPinnable | WidgetViewBase::ViewNameAsTitle));
		TableCfg(const ot::GenericDataStructMatrix& _matrix, TableCfg::TableHeaderMode _headerMode);
		TableCfg(const TableCfg& _other);
		TableCfg(TableCfg&& _other) noexcept;
		virtual ~TableCfg();

		TableCfg& operator = (const TableCfg& _other);
		TableCfg& operator = (TableCfg&& _other) noexcept;

		//! @brief Add the object contents to the provided JSON object.
		//! @param _object Json object reference to write the data to.
		//! @param _allocator Allocator.
		virtual void addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const override;

		//! @brief Set the object contents from the provided JSON object.
		//! @param _object The JSON object containing the information.
		//! @throw May throw an exception if the provided object is not valid (members missing or invalid types).
		virtual void setFromJsonObject(const ot::ConstJsonObject& _object) override;

		GenericDataStructMatrix createMatrix() const;

		//! @brief Clears all the data.
		//! Resets the row and column count back to 0.
		void clear();

		int getRowCount() const { return m_rows; };
		int getColumnCount() const { return m_columns; };

		const std::vector<std::vector<std::string>>& getRows() const { return m_data; };
		const std::vector<std::string>& getRow(int _row) const;

		void setCellText(int _row, int _column, const std::string& _text);
		const std::string& getCellText(int _row, int _column) const;
		
		void setRowHeader(int _row, const std::string& _headerText);
		void setRowHeader(int _row, TableHeaderItemCfg* _item);
		const TableHeaderItemCfg* getRowHeader(int _row) const;

		void setColumnHeader(int _column, TableHeaderItemCfg* _item);
		void setColumnHeaderText(int _column, const std::string& _headerText);
		void setColumnHeaderFilterBehavior(int _column, TableHeaderItemCfg::FilterBehavior _behavior);
		const TableHeaderItemCfg* getColumnHeader(int _column) const;

		void setColumnSortingEnabled(bool _enable = true) { m_columnsSortable = _enable; };
		bool getColumnSortingEnabled() const { return m_columnsSortable; };

		void setRowSortingEnabled(bool _enable = true) { m_rowsSortable = _enable; };
		bool getRowSortingEnabled() const { return m_rowsSortable; };

		void setReadOnly(bool _readOnly = true) { m_readOnly = _readOnly; };
		bool getReadOnly() const { return m_readOnly; };

	private:
		void initialize();
		void initialize(int _rows, int _columns);
		inline TableHeaderItemCfg* getOrCreateColumnHeaderItem(int _column);

		int m_rows = 0;
		int m_columns = 0;

		bool m_rowsSortable = false;
		std::vector<TableHeaderItemCfg*> m_rowHeader;

		bool m_columnsSortable = false;
		std::vector<TableHeaderItemCfg*> m_columnHeader;

		std::vector<std::vector<std::string>> m_data;

		bool m_readOnly = false;
	};

}