/*
 *	File:		aTableWidget.cpp
 *	Package:	akWidgets
 *
 *  Created on: March 19, 2020
 *	Author: Alexander Kuester
 *  Copyright (c) 2022 Alexander Kuester
 *	This file is part of the uiCore component.
 *	This file is subject to the terms and conditions defined in
 *	file 'LICENSE', which is part of this source code package.
 */

// AK header
#include <akWidgets/aTableWidget.h>

#include <qevent.h>

ak::aTableWidget::aTableWidget(QWidget * _parent)
: QTableWidget(_parent), aWidget(otTable) {}
ak::aTableWidget::aTableWidget(int _rows, int _columns, QWidget * _parent)
: QTableWidget(_rows, _columns, _parent), aWidget(otTable) {}

ak::aTableWidget::~aTableWidget() { A_OBJECT_DESTROYING }

// #######################################################################################################
// Event handling

void ak::aTableWidget::keyPressEvent(QKeyEvent *_event)
{
	QTableWidget::keyPressEvent(_event);
	Q_EMIT keyPressed(_event);
}

void ak::aTableWidget::keyReleaseEvent(QKeyEvent * _event) {
	QTableWidget::keyReleaseEvent(_event);
	Q_EMIT keyReleased(_event);
}

void ak::aTableWidget::focusInEvent(QFocusEvent * _event) {
	QTableWidget::focusInEvent(_event);
	Q_EMIT focused();
}

void ak::aTableWidget::focusOutEvent(QFocusEvent * _event) {
	QTableWidget::focusOutEvent(_event);
	Q_EMIT focusLost();
}

// #######################################################################################################

QWidget * ak::aTableWidget::widget(void) { return this; }

// ##############################################################################################################

// Table manipulation

void ak::aTableWidget::addRow(void) {
	insertRow(rowCount());
}

void ak::aTableWidget::addColumn(void) {
	insertColumn(columnCount());
}

void ak::aTableWidget::setCellText(
	int																_row,
	int																_column,
	const QString &													_text
) {
	QTableWidgetItem * itm = getCell(_row, _column);
	if (itm == nullptr) { itm = new QTableWidgetItem; }
	assert(itm != nullptr); // item is nullptr

	itm->setText(_text);
	setItem(_row, _column, itm);
}

void ak::aTableWidget::setCellEditable(
	int																_row,
	int																_column,
	bool															_ediaTableWidget
) {
	if (_ediaTableWidget && !isCellEditable(_row, _column) ||
		!_ediaTableWidget && isCellEditable(_row, _column))
	{
		// Get the cell
		QTableWidgetItem * itm = getCell(_row, _column);
		if (itm == nullptr) {
			// Create a new item
			itm = new QTableWidgetItem();
			setItem(_row, _column, itm);
		}
		// Set the cell ediaTableWidget flag
		itm->setFlags(itm->flags().setFlag(Qt::ItemIsEditable, _ediaTableWidget));
	}
}

void ak::aTableWidget::setCellSelectable(
	int																_row,
	int																_column,
	bool															_selecaTableWidget
) {
	if (_selecaTableWidget && !isCellSelectable(_row, _column) ||
		!_selecaTableWidget && isCellSelectable(_row, _column))
	{
		// Get the cell
		QTableWidgetItem * itm = getCell(_row, _column);
		if (itm == nullptr) {
			// Create a new item
			itm = new QTableWidgetItem();
			setItem(_row, _column, itm);
		}
		// Set item flags
		itm->setFlags(itm->flags().setFlag(Qt::ItemIsSelectable, _selecaTableWidget));
		//itm->setFlags(itm->flags().setFlag(Qt::ItemFlag::ItemIsAutoTristate, _selecaTableWidget));
	}
}

void ak::aTableWidget::setCellForecolor (
	int																_row,
	int																_column,
	const QColor &													_color
) {
	// Get the cell
	QTableWidgetItem * itm = getCell(_row, _column);
	if (itm == nullptr) {
		// Create a new item
		itm = new QTableWidgetItem();
		assert(itm != nullptr); // item is nullptr
		setItem(_row, _column, itm);
	}
	// Set item text color
	itm->setForeground(_color);
}

void ak::aTableWidget::setColumnHeader(
	int																_column,
	const QString &													_text
) {
	// Check arguments
	assert(_column >= 0 && _column < columnCount()); // Column index out of range
	// Set header item text
	setHorizontalHeaderItem(_column, new QTableWidgetItem(_text));
}

void ak::aTableWidget::setColumnWidthInPercent(
	int																_column,
	int																_percent
) {
	// Check arguments
	assert(_column >= 0 && _column < columnCount()); // Column index out of range
	assert(_percent > 0 && _percent <= 100); // Invalid percent value

	// Calculate width and set it
	double mul = (double)_percent * 0.01;
	int newWidth = (double)width() * mul;
	setColumnWidth(_column, newWidth);
}

void ak::aTableWidget::setColumnResizeMode(
	int																_column,
	QHeaderView::ResizeMode											_mode
) {
	// Check arguments
	assert(_column >= 0 && _column < columnCount()); // Column index out of range
	// Set column resize mode
	horizontalHeader()->setSectionResizeMode(_column, _mode);
}

void ak::aTableWidget::setRowHeaderIsVisible(
	bool															_visible
) { verticalHeader()->setVisible(_visible); }

void ak::aTableWidget::setRowHeaderIsEnabled(
	bool															_enabled
) { verticalHeader()->setEnabled(_enabled); }

void ak::aTableWidget::setColumnHeaderIsVisible(
	bool															_visible
) { horizontalHeader()->setVisible(_visible); }

void ak::aTableWidget::setColumnHeaderIsEnabled(
	bool															_enabled
) { horizontalHeader()->setEnabled(_enabled); }

void ak::aTableWidget::setCellIsSelected(
	int																_row,
	int																_column,
	bool															_selected
) {
	// Get cell
	QTableWidgetItem * itm = getCell(_row, _column);
	if (itm == nullptr) {
		// Create new cell
		itm = new QTableWidgetItem();
		setItem(_row, _column, itm);
	}
	itm->setSelected(_selected);
	if (_selected) {
		// Set cell flags
		QItemSelectionModel::SelectionFlags flags = QItemSelectionModel::ClearAndSelect;
		QModelIndex id = model()->index(_row, _column);
		selectionModel()->select(id, flags);
		scrollToItem(itm);
	}
	else {
		// Set cell flags
		QItemSelectionModel::SelectionFlags flags = QItemSelectionModel::Clear;
		QModelIndex id = model()->index(_row, _column);
		selectionModel()->select(id, flags);
	}
}

void ak::aTableWidget::clearRows(void) {
	setRowCount(0);
}

// ##############################################################################################################

// Information

bool ak::aTableWidget::isRowHeaderVisible(void) const { return verticalHeader()->isVisible(); }

bool ak::aTableWidget::isColumnHeaderVisible(void) const { return horizontalHeader()->isVisible(); }

bool ak::aTableWidget::isCellSelected(
	int																_row,
	int																_column
) {
	// Get cell
	QTableWidgetItem * itm = getCell(_row, _column);
	if (itm == nullptr) {
		// Create cell
		itm = new QTableWidgetItem;
		setItem(_row, _column, itm);
	}
	// Return cell is selected state
	return itm->isSelected();
}

QString ak::aTableWidget::getCellText(
	int																_row,
	int																_column
) const {
	// Get cell
	QTableWidgetItem * itm = getCell(_row, _column);
	if (itm == nullptr) { return QString(); }
	else { return itm->text(); }
}

bool ak::aTableWidget::isCellEditable(
	int																_row,
	int																_column
) {
	// Get cell
	QTableWidgetItem * itm = getCell(_row, _column);
	if (itm == nullptr) {
		// Create new cell
		itm = new QTableWidgetItem();
		setItem(_row, _column, itm);
	}
	// Return true if the item is ediaTableWidget flag is set
	if (itm->flags().testFlag(Qt::ItemIsEditable)) { return true; }
	else { return false; }
}

bool ak::aTableWidget::isCellSelectable(
	int																_row,
	int																_column
) {
	// Get cell
	QTableWidgetItem * itm = getCell(_row, _column);
	if (itm == nullptr) {
		// Create new cell
		itm = new QTableWidgetItem();
		setItem(_row, _column, itm);
	}
	// Return true if the item is selecaTableWidget flag is set
	if (itm->flags().testFlag(Qt::ItemIsSelectable)) { return true; }
	else { return false; }
}

// ##############################################################################################################

// Private functions

void ak::aTableWidget::checkIndex(
	int																_row,
	int																_column
) const {
	// Check the provided indizes
	assert(_row >= 0 && _row < rowCount()); // row index out of range
	assert(_column >= 0 && _column < columnCount()); // column index out of range
}

QTableWidgetItem * ak::aTableWidget::getCell(
	int																_row,
	int																_column
) const {
	// Check the indizes (throws exception of invalid)
	checkIndex(_row, _column);
	// Return the aTableWidget item
	return item(_row, _column);
}
