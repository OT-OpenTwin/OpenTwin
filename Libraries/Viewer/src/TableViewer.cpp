#include "stdafx.h"

#include "TableViewer.h"
#include <exception>
#include <QList>
#include <QTableWidgetSelectionRange>
#include <memory>
#include <qheaderview.h>
#include "OpenTwinCore/Logger.h"

TableViewer::TableViewer(QWidget* parent) 
{
	_table = new Table(_defaultNumberOfRows, _defaultNumberOfColumns, parent);
}

std::vector<ot::TableRange> TableViewer::GetSelectedRanges()
{
	QList<QTableWidgetSelectionRange> ranges = _table->selectedRanges();
	std::vector<ot::TableRange> rangesInternalType;
	rangesInternalType.reserve(ranges.size());
	for (auto range : ranges)
	{
		rangesInternalType.push_back(ot::TableRange(range.topRow(), range.bottomRow(), range.leftColumn(), range.rightColumn()));
	}
	return rangesInternalType;
}


bool TableViewer::CreateNewTable(std::shared_ptr<EntityResultTable<std::string>> table, EntityParameterizedDataTable::HeaderOrientation orientation)
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
	QBrush backgroundBrush(QColor(background.rInt(), background.gInt(), background.bInt(), background.aInt()));

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
	_table->clearSelection();
	for (auto range : ranges)
	{
		_table->setRangeSelected(QTableWidgetSelectionRange(range.GetTopRow(), range.GetLeftColumn(), range.GetBottomRow(), range.GetRightColumn()),true);
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

		uint64_t numberOfColumns = tableData->getNumberOfColumns();
		uint64_t numberOfRows = tableData->getNumberOfRows();

		unsigned int rowStart(0), columnStart(0), rowEnd(0), columnEnd(0);
		if (_tableOrientation == EntityParameterizedDataTable::HeaderOrientation::horizontal)
		{
			for (unsigned int c = 0; c < numberOfColumns; c++)
			{
				QString text = QString::fromStdString(tableData->getValue(0, c));
				QTableWidgetItem* item = new QTableWidgetItem(text);
				_table->setHorizontalHeaderItem(c, item);
			}
			rowStart++;
		}
		else
		{
			for (unsigned int r = 0; r < numberOfRows; r++)
			{
				QString text = QString::fromStdString(tableData->getValue(r, 0));
				QTableWidgetItem* item = new QTableWidgetItem(text);
				_table->setVerticalHeaderItem(r, item);
			}
			columnStart++;
		}



		SetLoopRanges(rowStart, columnStart, rowEnd, columnEnd,numberOfColumns,numberOfRows);
		
		uint32_t numberOfVisibleRows(0), numberOfVisibleColumns(0), distance (0);

		distance = rowEnd - rowStart;

		if (distance <= 0)
		{
			numberOfVisibleRows = 1;
		}
		else
		{
			numberOfVisibleRows = distance + 1;
		}
		distance = columnEnd - columnStart;
		if (distance <= 0)
		{
			numberOfVisibleColumns = 1;
		}
		else
		{
			numberOfVisibleColumns = columnEnd - columnStart + 1;
		}
		
		
		_table->setColumnCount(numberOfVisibleColumns);
		_table->setRowCount(numberOfVisibleRows);

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
	_shownMinCol = _activeTable->getMinColumn();
	_shownMaxCol = _activeTable->getMaxColumn();
	_shownMinRow = _activeTable->getMinRow();
	_shownMaxRow = _activeTable->getMaxRow();

	if (numberOfRows < _shownMaxRow) //max Row lays within the possible index
	{
		outRowEnd = numberOfRows - 1;
	}
	else
	{
		outRowEnd = _shownMaxRow;
	}

	if (numberOfColumns < _shownMaxCol) //max Row lays within the possible index
	{
		outColumnEnd = numberOfColumns - 1;
	}
	else
	{
		outColumnEnd = _shownMaxCol;
	}


	if (_shownMinRow > outRowEnd || _shownMinRow < 0 || _shownMinRow < outRowStart)
	{

		//outRowStart remains the default; 0 respectively 1 if the header is in the first row
	}
	else
	{
		outRowStart = _shownMinRow;
	}
	
	if (_shownMinCol > outColumnEnd || _shownMinCol < 0 || _shownMinCol < outRowStart)
	{

		outColumnStart = 0;//outRowStart remains the default; 0 respectively 1 if the header is in the first row
	}
	else
	{
		outColumnStart = _shownMinCol;
	}

}
