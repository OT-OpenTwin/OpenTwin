//! @file Table.cpp
//! @author Alexander Kuester (alexk95)
//! @date August 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTWidgets/Table.h"

// Qt header
#include <QtWidgets/qshortcut.h>
#include <QtWidgets/qheaderview.h>

ot::Table::Table(QWidget* _parentWidget)
	: QTableWidget(_parentWidget), m_contentChanged(false)
{
	this->ini();
}

ot::Table::Table(int _rows, int _columns, QWidget* _parentWidget) 
	: QTableWidget(_rows, _columns, _parentWidget), m_contentChanged(false)
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

	this->resizeColumnsToContents();
	this->resizeRowsToContents();

	this->blockSignals(tmp);
}

ot::TableCfg ot::Table::createConfig(void) const {
	TableCfg cfg(this->rowCount(), this->columnCount());
	cfg.setName(m_tableName);
	cfg.setTitle(m_tableTitle.toStdString());

	for (int r = 0; r < this->rowCount(); r++) {
		for (int c = 0; c < this->columnCount(); c++) {
			OTAssertNullptr(this->item(r, c));
			cfg.setCellText(r, c, this->item(r, c)->text().toStdString());
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

// ###########################################################################################################################################################################################################################################################################################################################

// Private slots

void ot::Table::slotSaveRequested(void) {
	Q_EMIT saveRequested();
}

void ot::Table::slotCellDataChanged(int _row, int _column) {
	m_contentChanged = true;
	this->contentChanged();
}

// ###########################################################################################################################################################################################################################################################################################################################

// Private helper

void ot::Table::ini(void) {
	QShortcut* saveShortcut = new QShortcut(QKeySequence("Ctrl+S"), this);
	saveShortcut->setContext(Qt::WidgetWithChildrenShortcut);

	this->connect(this, &Table::cellChanged, this, &Table::slotCellDataChanged);
	this->connect(saveShortcut, &QShortcut::activated, this, &Table::slotSaveRequested);
}
