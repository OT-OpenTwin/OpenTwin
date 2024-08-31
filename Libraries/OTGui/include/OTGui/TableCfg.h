//! @file TableCfg.h
//! @author Alexander Kuester (alexk95)
//! @date August 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/Serializable.h"
#include "OTCore/OTClassHelper.h"
#include "OTGui/OTGuiAPIExport.h"
#include "OTGui/TableHeaderItemCfg.h"

// std header
#include <string>
#include <vector>

namespace ot {

	class OT_GUI_API_EXPORT TableCfg : public Serializable {
	public:
		TableCfg(int _rows = 0, int _columns = 0);
		TableCfg(const TableCfg& _other);
		virtual ~TableCfg();

		TableCfg& operator = (const TableCfg& _other);

		//! \brief Add the object contents to the provided JSON object.
		//! \param _object Json object reference to write the data to.
		//! \param _allocator Allocator.
		virtual void addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const override;

		//! \brief Set the object contents from the provided JSON object.
		//! \param _object The JSON object containing the information.
		//! \throw May throw an exception if the provided object is not valid (members missing or invalid types).
		virtual void setFromJsonObject(const ot::ConstJsonObject& _object) override;

		//! \brief Clears all the data.
		//! Resets the row and column count back to 0.
		void clear(void);

		void setName(const std::string& _name) { m_name = _name; };
		const std::string& getName(void) const { return m_name; };

		void setTitle(const std::string& _title) { m_title = _title; };
		const std::string& getTitle(void) const { return (m_title.empty() ? m_name : m_title); };

		int getRowCount(void) const { return m_rows; };
		int getColumnCount(void) const { return m_columns; };

		void setCellText(int _row, int _column, const std::string& _text);
		const std::string& getCellText(int _row, int _column) const;
		
		void setRowHeader(int _row, const std::string& _headerText);
		void setRowHeader(int _row, TableHeaderItemCfg* _item);
		const TableHeaderItemCfg* getRowHeader(int _row) const;

		void setColumnHeader(int _column, const std::string& _headerText);
		void setColumnHeader(int _column, TableHeaderItemCfg* _item);
		const TableHeaderItemCfg* getColumnHeader(int _column) const;

	private:
		void initialize(void);

		std::string m_name;
		std::string m_title;

		int m_rows;
		int m_columns;

		std::vector<TableHeaderItemCfg*> m_rowHeader;
		std::vector<TableHeaderItemCfg*> m_columnHeader;
		std::vector<std::vector<std::string>> m_data;

	};

}