// @otlicense

#pragma once

// OpenTwin header
#include "OTGui/WidgetViewBase.h"
#include "OTGui/TableHeaderItemCfg.h"
#include "OTCore/GenericDataStructMatrix.h"

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

		GenericDataStructMatrix createMatrix(void) const;

		//! @brief Clears all the data.
		//! Resets the row and column count back to 0.
		void clear(void);

		int getRowCount(void) const { return m_rows; };
		int getColumnCount(void) const { return m_columns; };

		const std::vector<std::vector<std::string>>& getRows(void) const { return m_data; };
		const std::vector<std::string>& getRow(int _row) const;

		void setCellText(int _row, int _column, const std::string& _text);
		const std::string& getCellText(int _row, int _column) const;
		
		void setRowHeader(int _row, const std::string& _headerText);
		void setRowHeader(int _row, TableHeaderItemCfg* _item);
		const TableHeaderItemCfg* getRowHeader(int _row) const;

		void setColumnHeader(int _column, const std::string& _headerText);
		void setColumnHeader(int _column, TableHeaderItemCfg* _item);
		const TableHeaderItemCfg* getColumnHeader(int _column) const;

		void setSortingEnabled(bool _enable = true) { m_sortingEnabled = _enable; };
		bool getSortingEnabled(void) const { return m_sortingEnabled; };

		//! @brief If set the sorting can be cleared when pressing the sort button repeatadly.
		//! This has no effect if sorting is not enabled.
		void setSortingClearable(bool _enable = true) { m_sortingClearable = _enable; };
		bool getSortingClearable(void) const { return m_sortingClearable; };

	private:
		void initialize(void);
		void initialize(int _rows, int _columns);

		int m_rows = 0;
		int m_columns = 0;
		bool m_sortingEnabled = false;
		bool m_sortingClearable = false;

		std::vector<TableHeaderItemCfg*> m_rowHeader;
		std::vector<TableHeaderItemCfg*> m_columnHeader;
		std::vector<std::vector<std::string>> m_data;
	};

}