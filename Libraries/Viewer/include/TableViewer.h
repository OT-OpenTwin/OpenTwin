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

#include "OTCore/Color.h"
#include "OTServiceFoundation/TableRange.h"

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
	void clearActiveTableEntity() { _activeTable.reset(); _activeTable = nullptr; };
	ot::UID getTableID() const { return _activeTable->getEntityID(); }
	ot::UID getTableVersion() const { return _activeTable->getEntityStorageVersion(); }
	std::string getTableName() const { return _activeTable->getName(); }

	/**
	 * Takes a table entity changes the size and filling of the table member variable.
	 * 
	 * \param table
	 */
	bool CreateNewTable(std::shared_ptr<EntityResultTable<std::string>> table, EntityParameterizedDataTable::HeaderOrientation orientation = EntityParameterizedDataTable::HeaderOrientation::horizontal);
	std::vector <ot::TableRange> GetSelectedRanges();
	void ChangeColorOfSelection(ot::Color& background);
	void SelectRanges(std::vector <ot::TableRange>& ranges);
	void DeleteSelectedRow();
	void DeleteSelectedColumn();
	void AddRow(bool insertAbove);
	void AddColumn(bool insertLeft);

private:
	const int _defaultNumberOfRows = 60;
	const int _defaultNumberOfColumns = 30;
	std::shared_ptr <EntityResultTable<std::string>> _activeTable = nullptr;
	Table* _table = nullptr;

	EntityParameterizedDataTable::HeaderOrientation	_tableOrientation = EntityParameterizedDataTable::HeaderOrientation::horizontal;
	uint32_t _shownMinCol = 0;
	uint32_t _shownMaxCol = 0;
	uint32_t _shownMinRow = 0;
	uint32_t _shownMaxRow = 0;

	void SetTableData();
	void SetLoopRanges(uint32_t& outRowStart, uint32_t& outColumnStart, uint32_t& outRowEnd, uint32_t& outColumnEnd, uint64_t numberOfColumns, uint64_t numberOfRows);
};
