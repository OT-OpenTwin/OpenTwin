#include "stdafx.h"

#include "TableViewer.h"
#include <exception>
#include <QList>
#include <QTableWidgetSelectionRange>
#include <memory>
#include <qheaderview.h>

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


bool TableViewer::CreateNewTable(EntityResultTable<std::string> * table, EntityParameterizedDataTable::HeaderOrientation orientation)
{
	if (_tableID != table->getEntityID() || _tableVersion != table->getEntityStorageVersion() || orientation != _tableOrientation)
	{
		auto tableData = table->getTableData();
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
			//_table->resizeColumnsToContents();
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
