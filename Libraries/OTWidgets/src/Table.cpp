//! @file Table.cpp
//! @author Alexander Kuester (alexk95)
//! @date August 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTCore/RuntimeTests.h"
#include "OTWidgets/Table.h"
#include "OTWidgets/QtFactory.h"
#include "OTWidgets/TableItemDelegate.h"
#include "OTWidgets/SignalBlockWrapper.h"

// Qt header
#include <QtCore/qtimer.h>
#include <QtWidgets/qshortcut.h>
#include <QtWidgets/qheaderview.h>

#if OT_TESTING_GLOBAL_AllTestsEnabled==true
#define OT_TESTING_LOCAL_TABLE_PERFORMANCETEST_ENABLED OT_TESTING_GLOBAL_AllTestsEnabled
#elif OT_TESTING_GLOBAL_RuntimeTestingEnabled==true
#define OT_TESTING_LOCAL_TABLE_PERFORMANCETEST_ENABLED OT_TESTING_GLOBAL_RuntimeTestingEnabled
#else
#define OT_TESTING_LOCAL_TABLE_PERFORMANCETEST_ENABLED false
#endif 

#if OT_TESTING_LOCAL_TABLE_PERFORMANCETEST_ENABLED==true
#define OT_TEST_TABLE_Interval(___testText) OT_TEST_Interval(ot_intern_table_lcl_performancetest, "Table", ___testText)
#else
#define OT_TEST_TABLE_Interval(___testText)
#endif

QRect ot::Table::getSelectionBoundingRect(const QList<QTableWidgetSelectionRange>& _selections) {
	if (_selections.empty()) {
		return QRect();
	}
	else {
		QPoint minPt(std::numeric_limits<int>::max(), std::numeric_limits<int>::max());
		QPoint maxPt(std::numeric_limits<int>::lowest(), std::numeric_limits<int>::lowest());

		for (const QTableWidgetSelectionRange& range : _selections) {
			if (range.leftColumn() < minPt.x()) {
				minPt.setX(range.leftColumn());
			}
			if (range.rightColumn() > maxPt.x()) {
				maxPt.setX(range.rightColumn());
			}
			if (range.topRow() < minPt.y()) {
				minPt.setY(range.topRow());
			}
			if (range.bottomRow() > maxPt.y()) {
				maxPt.setY(range.bottomRow());
			}
		}

		return QRect(minPt, maxPt);
	}
}

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
	OT_TEST_TABLE_Interval("Setup from config: Total");

	SignalBlockWrapper blocker(this);

	{
		OT_TEST_TABLE_Interval("Setup from config: Clear");
		this->clear();
		m_columnWidthBuffer.clear();
		m_rowHeightBuffer.clear();
	}

	// Initialize dimensions
	{
		OT_TEST_TABLE_Interval("Setup from config: Ini");
		this->setRowCount(_config.getRowCount());
		this->setColumnCount(_config.getColumnCount());
	}

	// Initialize Header
	{
		OT_TEST_TABLE_Interval("Setup from config: Set vertical header");
		QHeaderView* header = this->verticalHeader();
		for (int r = 0; r < _config.getRowCount(); r++) {
			const TableHeaderItemCfg* headerItem = _config.getRowHeader(r);
			if (headerItem) {
				this->setVerticalHeaderItem(r, new QTableWidgetItem(QString::fromStdString(headerItem->getText())));
			}
		}
	}
	{
		OT_TEST_TABLE_Interval("Setup from config: Set horizontal header");
		QHeaderView* header = this->horizontalHeader();
		for (int c = 0; c < _config.getColumnCount(); c++) {
			const TableHeaderItemCfg* headerItem = _config.getColumnHeader(c);
			if (headerItem) {
				this->setHorizontalHeaderItem(c, new QTableWidgetItem(QString::fromStdString(headerItem->getText())));
			}
		}
	}

	// Initialize data
	{
		OT_TEST_TABLE_Interval("Setup from config: Set data");
		QAbstractItemModel* dataModel = this->model();
		SignalBlockWrapper blocker(dataModel);

		int rows = _config.getRowCount();
		int columns = _config.getColumnCount();

		for (int r = 0; r < rows; r++) {
			for (int c = 0; c < columns; c++) {
				dataModel->setData(dataModel->index(r, c), QString::fromStdString(_config.getCellText(r, c)));
			}
		}
	}

	this->horizontalHeader()->setSortIndicatorClearable(_config.getSortingClearable());
	this->setSortingEnabled(_config.getSortingEnabled());

	this->setResizeRequired();
}

ot::TableCfg ot::Table::createConfig(void) const {
	OT_TEST_TABLE_Interval("Create config");

	TableCfg cfg(this->rowCount(), this->columnCount());
	
	bool hasColumnHeader = false;
	for (int column = 0; column < columnCount(); column++) {
		const auto item = horizontalHeaderItem(column);
		if (item != nullptr) {
			hasColumnHeader = true;

			const std::string text = item->text().toStdString();
			cfg.setColumnHeader(column, text);
		}
	}

	bool hasRowHeader = false;
	for (int row = 0; row < this->rowCount(); row++) {
		const auto item = verticalHeaderItem(row);
		if (item != nullptr) {
			hasRowHeader = true;

			const std::string text = item->text().toStdString();
			cfg.setRowHeader(row, text);
		}

		for (int column = 0; column < this->columnCount(); column++) {
			QTableWidgetItem* itm = this->item(row, column);
			if (itm) {
				cfg.setCellText(row, column, itm->text().toStdString());
			}
		}
	}

	if (hasRowHeader) {
		for (int row = 0; row < cfg.getRowCount(); row++) {
			if (!cfg.getRowHeader(row)) {
				cfg.setRowHeader(row, new TableHeaderItemCfg());
			}
		}
	}
	if (hasColumnHeader) {
		for (int column = 0; column < cfg.getColumnCount(); column++) {
			if (!cfg.getColumnHeader(column)) {
				cfg.setColumnHeader(column, new TableHeaderItemCfg());
			}
		}
	}
	
	cfg.setSortingClearable(this->horizontalHeader()->isSortIndicatorClearable());
	cfg.setSortingEnabled(this->isSortingEnabled());

	return cfg;
}

void ot::Table::setContentChanged(bool _changed) {
	if (m_contentChanged == _changed) return;
	m_contentChanged = _changed;
	Q_EMIT modifiedChanged(m_contentChanged);
}

void ot::Table::setSelectedCellsBackground(const ot::Color& _color) {
	this->setSelectedCellsBackground(QtFactory::toQColor(_color));
}

void ot::Table::setSelectedCellsBackground(const QColor& _color) {
	OT_TEST_TABLE_Interval("Set selected cells background");

	SignalBlockWrapper blocker(this);
	
	for (QTableWidgetItem* item : this->selectedItems()) {
		item->setBackground(QBrush(_color));
	}
}

void ot::Table::prepareForDataChange(void) {
	m_stopResizing = true;
}

// ###########################################################################################################################################################################################################################################################################################################################

// Public slots

void ot::Table::slotSaveRequested(void) {
	Q_EMIT saveRequested();
}

// ###########################################################################################################################################################################################################################################################################################################################

// Protected

void ot::Table::showEvent(QShowEvent* _event) {
	OT_TEST_TABLE_Interval("Show event");
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

void ot::Table::slotCellDataChanged(int _row, int _column) {
	if (_row >= 0 && _row < this->rowCount()) {
		SignalBlockWrapper sigBlock(this);
		this->resizeRowToContents(_row);
	}

	if (m_contentChanged) {
		return;
	}

	m_contentChanged = true;
	Q_EMIT modifiedChanged(m_contentChanged);
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
		SignalBlockWrapper sigBlock(this);

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
		SignalBlockWrapper sigBlock(this);

		this->resizeColumnToContents(_column++);
		QTimer::singleShot(0, [=]() { Table::slotResizeColumnToContent(_column); });
	}
}

void ot::Table::slotRestoreRowSize(int _row) {
	if (m_stopResizing || _row >= this->rowCount()) { // Stop
		return;
	}
	else { // Resize column and queue next
		SignalBlockWrapper sigBlock(this);

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
		SignalBlockWrapper sigBlock(this);

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

void ot::Table::setResizeRequired(void) {
	if (this->isVisible()) {
		this->resizeColumnsToContentIfNeeded();
	}
	else {
		m_resizeRequired = true;
	}
}
