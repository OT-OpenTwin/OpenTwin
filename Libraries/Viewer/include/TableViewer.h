/*****************************************************************//**
 * \file   TableViewer.h
 * \brief  Handler for all table related functionality. After respective Viewer is being selected in ViewerAPI, its table shall be passed to this 
 * class which does whatever needs to  be done with the table.
 * 
 * \author Wagner
 * \date   February 2023
 *********************************************************************/

#pragma once
#include <vector>

#include "Table.h"
#include "EntityParameterizedDataTable.h"
#include "EntityResultTable.h"
#include "EntityTableSelectedRanges.h"

#include "OpenTwinCore/Color.h"
#include "OpenTwinFoundation/TableRange.h"

class TableViewer
{
public:
	/**
	 * Takes table that shall be changed. Nullptr is not allowed.
	 * 
	 * \param table
	 */
	TableViewer(QWidget* parent = nullptr);
	TableViewer(const TableViewer& other) = delete;
	TableViewer& operator=(TableViewer& other) = delete;

	Table* getTable() const { return _table; }
	ot::UID getTableID() const { return _tableID; }
	ot::UID getTableVersion() const { return _tableVersion; }
	std::string getTableName() const { return _tableName; }

	/**
	 * Takes a table entity changes the size and filling of the table member variable.
	 * 
	 * \param table
	 */
	bool CreateNewTable(EntityResultTable<std::string>* table, EntityParameterizedDataTable::HeaderOrientation orientation = EntityParameterizedDataTable::HeaderOrientation::horizontal);
	std::vector <ot::TableRange> GetSelectedRanges();
	void ChangeColorOfSelection(ot::Color& background);
	void SelectRanges(std::vector <ot::TableRange>& ranges);

private:
	Table* _table = nullptr;
	const int _defaultNumberOfRows = 60;
	const int _defaultNumberOfColumns = 30;
	ot::UID _tableID = -1;
	ot::UID _tableVersion = -1;
	std::string _tableName;
	EntityParameterizedDataTable::HeaderOrientation	_tableOrientation;
};
