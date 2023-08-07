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

#include "OpenTwinCore/PerformanceTests.h"

bool TableViewer::CreateNewTable(EntityResultTable<std::string> * table, EntityParameterizedDataTable::HeaderOrientation orientation)
{
	OT_LOG_D("Test");
	if (_tableID != table->getEntityID() || _tableVersion != table->getEntityStorageVersion() || orientation != _tableOrientation)
	{
		//TakeTime(t1);
		std::chrono::system_clock::time_point t1 = std::chrono::system_clock::now();
		auto tableData = table->getTableData();
		std::chrono::system_clock::time_point t2 = std::chrono::system_clock::now();
		if (tableData != nullptr)
		{
			_table->clear();
			_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
			unsigned int numberOfColumns = tableData->getNumberOfColumns();
			unsigned int numberOfRows = tableData->getNumberOfRows();

			unsigned int rowStart(0),columnStart(0);
			if (orientation == EntityParameterizedDataTable::HeaderOrientation::horizontal)
			{
				rowStart = 1;
				_table->setColumnCount(numberOfColumns);
				_table->setRowCount(numberOfRows-1);
				for (unsigned int c = 0; c < numberOfColumns; c++)
				{
					QString text = QString::fromStdString(tableData->getValue(0, c));
					QTableWidgetItem *item = new QTableWidgetItem(text);
					_table->setHorizontalHeaderItem(c, item);
				}
			}
			else
			{
				columnStart = 1;
				_table->setColumnCount(numberOfColumns-1);
				_table->setRowCount(numberOfRows);

				for (unsigned int r = 0; r < numberOfRows; r++)
				{
					QString text = QString::fromStdString(tableData->getValue(r, 0));
					QTableWidgetItem *item = new QTableWidgetItem(text);
					_table->setVerticalHeaderItem(r, item);
				}
			}

			unsigned int itemRowIndex(0);
			for (unsigned int r = rowStart; r < numberOfRows; r++)
			{
				unsigned int itemColumnIndex = 0;
				for (unsigned int c = columnStart; c < numberOfColumns; c++)
				{
					QString text = QString::fromStdString(tableData->getValue(r, c));
					QTableWidgetItem *item = new QTableWidgetItem(text);
					_table->setItem(itemRowIndex, itemColumnIndex, item);
					itemColumnIndex++;
				}
				itemRowIndex++;
			}
			std::chrono::system_clock::time_point t3 = std::chrono::system_clock::now();
			_table->resizeColumnsToContents();
			std::chrono::system_clock::time_point t4 = std::chrono::system_clock::now();
			
			
			OT_LOG_D("Loading table data: " + std::to_string(std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count()));
			OT_LOG_D("Build qtable: " + std::to_string(std::chrono::duration_cast<std::chrono::microseconds>(t3 - t2).count()));
			OT_LOG_D("Resized qtable: " + std::to_string(std::chrono::duration_cast<std::chrono::microseconds>(t4 - t3).count()));
			
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
