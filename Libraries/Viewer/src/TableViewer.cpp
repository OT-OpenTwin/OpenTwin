#include "stdafx.h"

#include "TableViewer.h"
#include <exception>
#include <QList>
#include <QTableWidgetSelectionRange>
#include <memory>
#include <qheaderview.h>
#include "OTCore/Logger.h"

TableViewer::TableViewer(QWidget* parent) 
{
	_table = new Table(_defaultNumberOfRows, _defaultNumberOfColumns, parent);
}

std::vector<ot::TableRange> TableViewer::GetSelectedRanges()
{
	QList<QTableWidgetSelectionRange> ranges = _table->selectedRanges();
	std::vector<ot::TableRange> rangesInternalType;
	rangesInternalType.reserve(ranges.size());
	int colOffset =	_activeTable->getMinColumn();
	int rowOffset = _activeTable->getMinRow();
	for (auto range : ranges)
	{
		rangesInternalType.push_back(ot::TableRange(range.topRow() + rowOffset, range.bottomRow() + rowOffset, range.leftColumn() + colOffset, range.rightColumn() + colOffset));
	}
	return rangesInternalType;
}


bool TableViewer::CreateNewTable(std::shared_ptr<EntityResultTable<std::string>> table, ot::TableHeaderOrientation orientation)
{
	if (_activeTable == nullptr || _activeTable->getEntityID() != table->getEntityID() || _activeTable->getEntityStorageVersion() != table->getEntityStorageVersion() || orientation != _tableOrientation)
	{
		_activeTable = table;
		_tableOrientation = orientation;
		SetTableData();
		return true;
	}
	else
	{
		return false;
	}
}



void TableViewer::ChangeColorOfSelection(ot::Color & background)
{
	auto selectedRanges = _table->selectedRanges();
	QBrush backgroundBrush(QColor(background.r(), background.g(), background.b(), background.a()));

	for (auto range : selectedRanges)
	{
		int bottomRow = range.bottomRow();
		int topRow = range.topRow();
		int leftCol = range.leftColumn();
		int rightCol = range.rightColumn();
		for (int row = topRow; row <= bottomRow; row++)
		{
			for (int column = leftCol; column <= rightCol; column++)
			{
				QTableWidgetItem* item= _table->item(row, column);
				item->setBackground(backgroundBrush);
			}
		}
	}
}

void TableViewer::SelectRanges(std::vector<ot::TableRange>& ranges)
{

	if(_activeTable != nullptr)
	{
		_table->clearSelection();

		int minCol = _activeTable->getMinColumn() < 0 ? 0 : _activeTable->getMinColumn();
		int maxCol = _activeTable->getMaxColumn() < 0 ? 0 : _activeTable->getMaxColumn();
		int minRow = _activeTable->getMinRow() < 0 ? 0 : _activeTable->getMinRow();
		int maxRow = _activeTable->getMaxRow() < 0 ? 0 : _activeTable->getMaxRow();
		auto paramTable = dynamic_cast<EntityParameterizedDataTable*>(_activeTable.get());
		if (paramTable == nullptr || paramTable->getSelectedHeaderOrientation() == ot::TableHeaderOrientation::horizontal)
		{
			if (minRow == 0) { minRow = 1; }
		}
		else
		{
			if (minCol == 0) { minCol = 1; }
		}
		for (auto range : ranges)
		{
			int displayedTableTopRowIndx = range.getTopRow() - minRow;
			int displayedTableBottomRowIndx = range.getBottomRow() - minRow;
			if (displayedTableBottomRowIndx >= 0 || displayedTableTopRowIndx >= 0) //&& (same for column)
			{
				if (displayedTableTopRowIndx < 0)
				{
					displayedTableTopRowIndx = 0;
				}
				if (range.getBottomRow() > maxRow)
				{
					displayedTableBottomRowIndx = maxRow - minRow;
				}
				if (displayedTableBottomRowIndx >= _activeTable->getTableData()->getNumberOfRows())
				{
					displayedTableBottomRowIndx = _activeTable->getTableData()->getNumberOfRows() - 1;
				}
			}

			int displayedTableLeftColIndx = range.getLeftColumn() - minCol;
			int displayedTableRightColIndx = range.getRightColumn() - minCol;
			if (displayedTableLeftColIndx >= 0 || displayedTableRightColIndx >= 0)
			{
				if (displayedTableLeftColIndx < 0)
				{
					displayedTableLeftColIndx = 0;
				}
				if(range.getRightColumn()> maxCol)
				{
					displayedTableRightColIndx = maxCol - minCol;
				}
				if (displayedTableRightColIndx >= _activeTable->getTableData()->getNumberOfColumns())
				{
					displayedTableRightColIndx = _activeTable->getTableData()->getNumberOfColumns() - 1;
				}
			}
			_table->setRangeSelected(QTableWidgetSelectionRange(displayedTableTopRowIndx, displayedTableLeftColIndx, displayedTableBottomRowIndx, displayedTableRightColIndx), true);
		}
	}
}

void TableViewer::DeleteSelectedRow()
{
	QList<QTableWidgetSelectionRange> ranges = _table->selectedRanges();
	if (ranges.size() != 0)
	{
		for (auto range : ranges)
		{
			for (int row = range.bottomRow(); row <= range.topRow(); row++)
			{
				_table->removeRow(row);
			}
		}
	}
}

void TableViewer::DeleteSelectedColumn()
{
	QList<QTableWidgetSelectionRange> ranges = _table->selectedRanges();
	if (ranges.size() != 0)
	{
		for (auto range : ranges)
		{
			for (int column= range.leftColumn(); column <= range.rightColumn(); column++)
			{
				_table->removeColumn(column);
			}
		}
	}
}

void TableViewer::AddRow(bool insertAbove)
{
	QList<QTableWidgetSelectionRange> ranges = _table->selectedRanges();
	if (ranges.size() != 0)
	{
		for (auto range : ranges)
		{
			for (int row = range.bottomRow(); row <= range.topRow(); row++)
			{
				_table->insertRow(row + !insertAbove);
			}
		}
	}
}

void TableViewer::AddColumn(bool insertLeft)
{
	QList<QTableWidgetSelectionRange> ranges = _table->selectedRanges();
	if (ranges.size() != 0)
	{
		for (auto range : ranges)
		{
			for (int column = range.leftColumn(); column <= range.rightColumn(); column++)
			{
				_table->insertColumn(column + !insertLeft);
			}
		}
	}
}

void TableViewer::SetTableData()
{
	std::chrono::system_clock::time_point t1 = std::chrono::system_clock::now();
	auto tableData = _activeTable->getTableData();
	std::chrono::system_clock::time_point t2 = std::chrono::system_clock::now();

	if (tableData != nullptr)
	{
		std::chrono::system_clock::time_point t3 = std::chrono::system_clock::now();
		_table->clear();
		std::chrono::system_clock::time_point t4 = std::chrono::system_clock::now();
		_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);

		//Get iterators for data access
		unsigned int rowStart(0), columnStart(0), rowEnd(0), columnEnd(0);
		if (_tableOrientation == ot::TableHeaderOrientation::horizontal)
		{
			rowStart = 1;
		}
		else
		{
			columnStart = 1;
		}
		uint64_t numberOfColumns = tableData->getNumberOfColumns();
		uint64_t numberOfRows = tableData->getNumberOfRows();
		SetLoopRanges(rowStart, columnStart, rowEnd, columnEnd, numberOfColumns, numberOfRows);

		//Set number of rows and columns at visible table
		uint32_t numberOfVisibleRows(0), numberOfVisibleColumns(0), distance(0);
		distance = rowEnd - rowStart;
		if (distance <= 0)
		{
			numberOfVisibleRows = 1;
		}
		else
		{
			numberOfVisibleRows = distance +1;
		}
		distance = columnEnd - columnStart;
		if (distance <= 0)
		{
			numberOfVisibleColumns = 1;
		}
		else
		{
			numberOfVisibleColumns = distance + 1;
		}
		_table->setRowCount(numberOfVisibleRows);
		_table->setColumnCount(numberOfVisibleColumns);

		//Setting both headers
		if (_tableOrientation == ot::TableHeaderOrientation::horizontal)
		{
			unsigned int columnPtr = 0;
			for (unsigned int c = columnStart; c <= columnEnd ; c++)
			{
				QString text = QString::fromStdString(tableData->getValue(0, c));
				QTableWidgetItem* item = new QTableWidgetItem(text);
				_table->setHorizontalHeaderItem(columnPtr, item);
				columnPtr++;
			}
			int rowPtr = 0;
			for (unsigned int r = rowStart; r <= rowEnd; r++)
			{
				QString text = QString::fromStdString(std::to_string(r));
				QTableWidgetItem* item = new QTableWidgetItem(text);
				_table->setVerticalHeaderItem(rowPtr, item);
				rowPtr++;
			}
		}
		else
		{
			int rowPtr = 0;
			for (unsigned int r = rowStart; r <= rowEnd ; r++)
			{
				QString text = QString::fromStdString(tableData->getValue(r, 0));
				QTableWidgetItem* item = new QTableWidgetItem(text);
				_table->setVerticalHeaderItem(rowPtr, item);
				rowPtr++;
			}

			int columnPtr = 0;
			for (unsigned int c = columnStart; c <= columnEnd; c++)
			{
				QString text = QString::fromStdString(std::to_string(c));
				QTableWidgetItem* item = new QTableWidgetItem(text);
				_table->setHorizontalHeaderItem(columnPtr, item);
				columnPtr++;
			}
		}

		//Filling data in visible table
		std::chrono::system_clock::time_point t5 = std::chrono::system_clock::now();
		unsigned int itemRowIndex(0);
		for (unsigned int r = rowStart; r <= rowEnd; r++)
		{
			unsigned int itemColumnIndex = 0;
			for (unsigned int c = columnStart; c <= columnEnd; c++)
			{
				QString text = QString::fromStdString(tableData->getValue(r, c));
				QTableWidgetItem* item = new QTableWidgetItem(text);
				_table->setItem(itemRowIndex, itemColumnIndex, item);
				itemColumnIndex++;
			}
			itemRowIndex++;
		}
		std::chrono::system_clock::time_point t6 = std::chrono::system_clock::now();
		_table->resizeColumnsToContents();
		std::chrono::system_clock::time_point t7 = std::chrono::system_clock::now();


		std::string durationTableLoad = std::to_string(std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count());
		std::string durationClearTable = std::to_string(std::chrono::duration_cast<std::chrono::microseconds>(t4 - t3).count());
		std::string durationSetTableItems = std::to_string(std::chrono::duration_cast<std::chrono::microseconds>(t6 - t5).count());
		std::string durationSetTableResize = std::to_string(std::chrono::duration_cast<std::chrono::microseconds>(t7 - t6).count());

		OT_LOG_D("Loading table data: " + durationTableLoad);
		OT_LOG_D("Clear table data: " + durationClearTable);
		OT_LOG_D("Build qtable: " + durationSetTableItems);
		OT_LOG_D("Resized qtable: " + durationSetTableResize);
	}
	else
	{
		throw std::exception("Table data could not be loaded.");
	}
}

void TableViewer::SetLoopRanges(uint32_t& outRowStart, uint32_t& outColumnStart, uint32_t& outRowEnd, uint32_t& outColumnEnd, uint64_t numberOfColumns, uint64_t numberOfRows)
{
	_shownMinCol = _activeTable->getMinColumn() -1; //The display index is 1-based and the access index is 0-based.
	_shownMaxCol = _activeTable->getMaxColumn() - 1;
	_shownMinRow = _activeTable->getMinRow() - 1;
	_shownMaxRow = _activeTable->getMaxRow() - 1;

	if (numberOfRows <= _shownMaxRow) //max Row lays within the possible index
	{
		outRowEnd = numberOfRows - 1;
	}
	else if (_shownMaxRow < outRowStart)
	{
		outRowEnd = outRowStart;
	}
	else
	{
		outRowEnd = _shownMaxRow;
	}

	if (numberOfColumns <= _shownMaxCol) //max Row lays within the possible index
	{
		outColumnEnd = numberOfColumns - 1;
	}
	else if (_shownMaxCol < outColumnStart)
	{
		outColumnEnd = outColumnStart;
	}
	else
	{
		outColumnEnd = _shownMaxCol;
	}


	if (_shownMinRow > outRowEnd || _shownMinRow < 0 || _shownMinRow < outRowStart)
	{

		//outRowStart remains the default; 0 respectively 1 if the header is in the zeroth row
	}
	else
	{
		outRowStart = _shownMinRow;
	}
	
	if (_shownMinCol > outColumnEnd || _shownMinCol < 0 || _shownMinCol < outColumnStart)
	{

		//outRowStart remains the default; 0 respectively 1 if the header is in the zeroth column
	}
	else
	{
		outColumnStart = _shownMinCol;
	}

}
