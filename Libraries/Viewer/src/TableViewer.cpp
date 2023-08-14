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
	if (_tableID != table->getEntityID() || _tableVersion != table->getEntityStorageVersion() || orientation != _tableOrientation)
	{

		std::chrono::system_clock::time_point t1 = std::chrono::system_clock::now();
		auto tableData = table->getTableData();
		std::chrono::system_clock::time_point t2 = std::chrono::system_clock::now();
		if (tableData != nullptr)
		{
			std::chrono::system_clock::time_point t3 = std::chrono::system_clock::now();
			_table->clear();
			std::chrono::system_clock::time_point t4 = std::chrono::system_clock::now();
			_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
			unsigned int numberOfColumns = tableData->getNumberOfColumns();
			unsigned int numberOfRows = tableData->getNumberOfRows();
			int minCol = table->getMinColumn();
			int maxCol = table->getMaxColumn();
			int minRow = table->getMinRow();
			int maxRow = table->getMaxRow();
			
			unsigned int rowStart,columnStart, rowEnd, columnEnd;
			rowEnd= numberOfRows < maxRow ? numberOfRows: maxRow;
			columnEnd= numberOfColumns < maxCol ? numberOfColumns : maxCol;

			if (minRow <= rowEnd)
			{
				if (minRow < 0)
				{
					rowStart = 0;
				}
				else
				{
					rowStart = minRow;
				}
			}
			else
			{
				if (rowEnd== 0)
				{
					rowStart = 0;
				}
				else
				{
					rowStart = rowEnd- 1;
				}
			}
			
			if (minCol<= columnEnd)
			{
				if (minCol< 0)
				{
					columnStart = 0;
				}
				else
				{
					columnStart = minCol;
				}
			}
			else
			{
				if (columnEnd== 0)
				{
					columnStart = 0;
				}
				else
				{
					columnStart = columnEnd - 1;
				}
			}


			if (orientation == EntityParameterizedDataTable::HeaderOrientation::horizontal)
			{
				_table->setColumnCount(columnEnd - columnStart);
				_table->setRowCount(rowEnd - rowStart -1 );
				for (unsigned int c = 0; c < numberOfColumns; c++)
				{
					QString text = QString::fromStdString(tableData->getValue(0, c));
					QTableWidgetItem *item = new QTableWidgetItem(text);
					_table->setHorizontalHeaderItem(c, item);
				}
				rowStart++;
			}
			else
			{
				_table->setColumnCount(columnEnd - columnStart -1);
				_table->setRowCount(rowEnd - rowStart);

				for (unsigned int r = 0; r < numberOfRows; r++)
				{
					QString text = QString::fromStdString(tableData->getValue(r, 0));
					QTableWidgetItem *item = new QTableWidgetItem(text);
					_table->setVerticalHeaderItem(r, item);
				}
				columnStart++;
			}
			std::chrono::system_clock::time_point t5 = std::chrono::system_clock::now();
			unsigned int itemRowIndex(0);
			for (unsigned int r = rowStart; r < rowEnd; r++)
			{
				unsigned int itemColumnIndex = 0;
				for (unsigned int c = columnStart; c < columnEnd; c++)
				{
					QString text = QString::fromStdString(tableData->getValue(r, c));
					QTableWidgetItem *item = new QTableWidgetItem(text);
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
			
			_tableName = table->getName();
			_tableID = table->getEntityID();
			_tableVersion = table->getEntityStorageVersion();
			_tableOrientation = orientation;
		}
		else
		{
			throw std::exception("Table data could not be loaded.");
		}
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
