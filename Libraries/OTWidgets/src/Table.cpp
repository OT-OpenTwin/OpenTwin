//! @file Table.cpp
//! @author Alexander Kuester (alexk95)
//! @date August 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTWidgets/Table.h"
#include "OTWidgets/QtFactory.h"
#include "OTWidgets/TableItemDelegate.h"

// Qt header
#include <QtCore/qtimer.h>
#include <QtWidgets/qshortcut.h>
#include <QtWidgets/qheaderview.h>

ot::Table::Table(QWidget* _parentWidget)
	: QTableWidget(_parentWidget), m_contentChanged(false), m_resizeRequired(false), m_stopResizing(true)
{
	this->ini();
}

ot::Table::Table(int _rows, int _columns, QWidget* _parentWidget) 
	: QTableWidget(_rows, _columns, _parentWidget), m_contentChanged(false), m_resizeRequired(false), m_stopResizing(true)
{
	this->ini();
}

ot::Table::~Table() {

}

// ###########################################################################################################################################################################################################################################################################################################################

// Setter / Getter

void ot::Table::setupFromConfig(const TableCfg& _config) {
	bool tmp = this->signalsBlocked();
	this->blockSignals(true);

	this->clear();
	m_columnWidthBuffer.clear();
	m_rowHeightBuffer.clear();

	// Initialize dimensions
	this->setRowCount(_config.getRowCount());
	this->setColumnCount(_config.getColumnCount());

	// Initialize data
	QAbstractItemModel* dataModel = this->model();

	for (int r = 0; r < _config.getRowCount(); r++) {
		for (int c = 0; c < _config.getColumnCount(); c++) {
			dataModel->setData(dataModel->index(r, c), QString::fromStdString(_config.getCellText(r, c)));
		}
	}

	// Initialize Header
	QHeaderView* header = this->verticalHeader();
	for (int r = 0; r < _config.getRowCount(); r++) {
		const TableHeaderItemCfg* headerItem = _config.getRowHeader(r);
		if (headerItem) {
			this->setVerticalHeaderItem(r, new QTableWidgetItem(QString::fromStdString(headerItem->getText())));
		}
	}

	header = this->horizontalHeader();
	for (int c = 0; c < _config.getColumnCount(); c++) {
		const TableHeaderItemCfg* headerItem = _config.getColumnHeader(c);
		if (headerItem) {
			this->setHorizontalHeaderItem(c, new QTableWidgetItem(QString::fromStdString(headerItem->getText())));
		}
	}

	if (this->isVisible()) {
		this->resizeColumnsToContentIfNeeded();
	}
	else {
		m_resizeRequired = true;
	}
	
	this->blockSignals(tmp);
}

ot::TableCfg ot::Table::createConfig(void) const {
	TableCfg cfg(this->rowCount(), this->columnCount());
	
	for (int r = 0; r < this->rowCount(); r++) {
		for (int c = 0; c < this->columnCount(); c++) {
			OTAssertNullptr(this->item(r, c));
			cfg.setCellText(r, c, this->item(r, c)->text().toStdString());
		}
	}

	for (int row = 0; row < rowCount(); row++)
	{
		const auto item =	verticalHeaderItem(row);
		if (item != nullptr)
		{
			const std::string text = item->text().toStdString();
			cfg.setRowHeader(row, text);
		}
	}
	
	for (int column = 0; column < columnCount(); column++)
	{
		const auto item = horizontalHeaderItem(column);
		if (item != nullptr)
		{
			const std::string text = item->text().toStdString();
			cfg.setColumnHeader(column, text);
		}
	}

	return cfg;
}

void ot::Table::setContentChanged(bool _changed) {
	if (m_contentChanged == _changed) return;
	m_contentChanged = _changed;
	if (m_contentChanged) this->contentChanged();
	else this->contentSaved();
}

void ot::Table::setSelectedCellsBackground(const ot::Color& _color) {
	this->setSelectedCellsBackground(QtFactory::toQColor(_color));
}

void ot::Table::setSelectedCellsBackground(const QColor& _color) {
	bool blocked = this->signalsBlocked();
	this->blockSignals(true);
	for (QTableWidgetItem* item : this->selectedItems()) {
		item->setBackground(QBrush(_color));
	}
	this->blockSignals(blocked);
}

void ot::Table::prepareForDataChange(void) {
	m_stopResizing = true;
}

// ###########################################################################################################################################################################################################################################################################################################################

// Protected

void ot::Table::showEvent(QShowEvent* _event) {
	QTableWidget::showEvent(_event);

	if (!m_columnWidthBuffer.empty() || !m_rowHeightBuffer.empty()) {
		if (this->columnCount() != m_columnWidthBuffer.size()) {
			OT_LOG_EA("Column width buffer is invalid");
		} else if (this->rowCount() != m_rowHeightBuffer.size()) {
			OT_LOG_EA("Row height buffer is invalid");
		}
		else if (!m_columnWidthBuffer.empty()) {
			m_stopResizing = false;
			this->slotRestoreColumnSize(0);
		}
		else if (!m_rowHeightBuffer.empty()) {
			m_stopResizing = false;
			this->slotRestoreRowSize(0);
		}
	}
	else if (m_resizeRequired) {
		this->resizeColumnsToContentIfNeeded();
		m_resizeRequired = false;
	}
}

void ot::Table::hideEvent(QHideEvent* _event) {
	m_columnWidthBuffer.clear();
	m_rowHeightBuffer.clear();
	m_columnWidthBuffer.reserve(this->columnCount());
	m_rowHeightBuffer.reserve(this->rowCount());

	for (int i = 0; i < this->columnCount(); i++) {
		if (this->horizontalHeader()->sectionResizeMode(i) != QHeaderView::Interactive) {
			m_columnWidthBuffer.clear();
			m_rowHeightBuffer.clear();
			return;
		}
		m_columnWidthBuffer.push_back(this->columnWidth(i));
	}
	for (int i = 0; i < this->rowCount(); i++) {
		if (this->verticalHeader()->sectionResizeMode(i) != QHeaderView::Interactive) {
			m_columnWidthBuffer.clear();
			m_rowHeightBuffer.clear();
			return;
		}
		m_rowHeightBuffer.push_back(this->rowHeight(i));
	}

	m_stopResizing = true;
}

// ###########################################################################################################################################################################################################################################################################################################################

// Private slots

void ot::Table::slotSaveRequested(void) {
	Q_EMIT saveRequested();
}

void ot::Table::slotCellDataChanged(int _row, int _column) {
	m_contentChanged = true;
	this->contentChanged();
}

void ot::Table::slotRestoreColumnSize(int _column) {
	if (m_stopResizing) { // Stop
		return;
	}
	if (_column >= this->columnCount()) { // Last column reached, now go for rows
		if (!m_rowHeightBuffer.empty()) {
			this->slotRestoreRowSize(0);
		}
	}
	else { // Resize column and queue next
		OTAssert(m_columnWidthBuffer.size() == this->columnCount(), "Invalid data");
		this->setColumnWidth(_column, m_columnWidthBuffer[_column]);
		_column++;

		QTimer::singleShot(0, [=]() { Table::slotRestoreColumnSize(_column); });
	}
}

void ot::Table::slotResizeColumnToContent(int _column) {
	if (m_stopResizing) { // Stop
		return;
	}
	if (_column >= this->columnCount()) { // Last column reached, now go for rows
		this->resizeRowsToContentIfNeeded();
	}
	else { // Resize column and queue next
		this->resizeColumnToContents(_column++);

		QTimer::singleShot(0, [=]() { Table::slotResizeColumnToContent(_column); });
	}
}

void ot::Table::slotRestoreRowSize(int _row) {
	if (m_stopResizing || _row >= this->rowCount()) { // Stop
		return;
	}
	else { // Resize column and queue next
		OTAssert(m_rowHeightBuffer.size() == this->rowCount(), "Invalid data");
		this->setRowHeight(_row, m_rowHeightBuffer[_row]);
		_row++;

		QTimer::singleShot(0, [=]() { Table::slotRestoreRowSize(_row); });
	}
}

void ot::Table::slotResizeRowToContent(int _row) {
	if (m_stopResizing || _row >= this->rowCount()) { // Stop
		return;
	}
	else { // Resize column and queue next
		this->resizeRowToContents(_row++);

		QTimer::singleShot(0, [=]() { Table::slotResizeRowToContent(_row); });
	}
}

// ###########################################################################################################################################################################################################################################################################################################################

// Private helper

void ot::Table::ini(void) {
	new TableItemDelegate(this); // Will be destroyed via Qt

	QShortcut* saveShortcut = new QShortcut(QKeySequence("Ctrl+S"), this);
	saveShortcut->setContext(Qt::WidgetWithChildrenShortcut);

	this->connect(this, &Table::cellChanged, this, &Table::slotCellDataChanged);
	this->connect(saveShortcut, &QShortcut::activated, this, &Table::slotSaveRequested);
}

void ot::Table::resizeColumnsToContentIfNeeded(void) {
	// Ensure we have any content
	if (this->horizontalHeader()->count() == 0) {
		this->resizeRowsToContentIfNeeded();
		return;
	}

	// Ensure the content is interactive
	for (int i = 0; i < this->horizontalHeader()->count(); i++) {
		if (this->horizontalHeader()->sectionResizeMode(i) != QHeaderView::Interactive) {
			this->resizeRowsToContentIfNeeded();
			return;
		}
	}

	// Start resizing
	m_stopResizing = false;
	this->slotResizeColumnToContent(0);
}

void ot::Table::resizeRowsToContentIfNeeded(void) {
	// Ensure we have any content
	if (this->verticalHeader()->count() == 0) {
		return;
	}

	// Ensure the content is interactive
	for (int i = 0; i < this->verticalHeader()->count(); i++) {
		if (this->verticalHeader()->sectionResizeMode(i) != QHeaderView::Interactive) {
			return;
		}
	}

	// Start resizing
	m_stopResizing = false;
	this->slotResizeRowToContent(0);
}
