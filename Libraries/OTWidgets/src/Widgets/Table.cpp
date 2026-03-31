// @otlicense
// File: Table.cpp
// 
// License:
// Copyright 2025 by OpenTwin
//  
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
//  
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// @otlicense-end

// OpenTwin header
#include "OTCore/Logging/LogDispatcher.h"
#include "OTCore/RuntimeTests.h"
#include "OTWidgets/QtFactory.h"
#include "OTWidgets/Delegate/TableItemDelegate.h"
#include "OTWidgets/Header/TableHeader.h"
#include "OTWidgets/Widgets/Table.h"
#include "OTWidgets/Widgets/TableItem.h"
#include "OTCore/String.h"

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
	: QTableWidget(_parentWidget), m_contentChanged(false), m_resizeRequired(false),
	m_stopResizing(true), m_itemDelegate(nullptr), m_horizontalHeader(nullptr), m_verticalHeader(nullptr)
{
	this->ini();
}

ot::Table::Table(int _rows, int _columns, QWidget* _parentWidget) 
	: QTableWidget(_rows, _columns, _parentWidget), m_contentChanged(false), m_resizeRequired(false),
	m_stopResizing(true), m_itemDelegate(nullptr), m_horizontalHeader(nullptr), m_verticalHeader(nullptr)
{
	this->ini();
}

ot::Table::~Table() {
	delete m_itemDelegate;
	m_itemDelegate = nullptr;

	clearHeaderConfigs();
}

// ###########################################################################################################################################################################################################################################################################################################################

// Setter / Getter

void ot::Table::setupFromConfig(const TableCfg& _config) {
	OT_TEST_TABLE_Interval("Setup from config: Total");

	QSignalBlocker blocker(this);
	{
		OT_TEST_TABLE_Interval("Setup from config: Clear");
		this->clear();
		m_columnWidthBuffer.clear();
		m_rowHeightBuffer.clear();
		clearHeaderConfigs();
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
		m_verticalHeaderItemCfgs.reserve(_config.getRowCount());
		QHeaderView* header = this->verticalHeader();
		for (int r = 0; r < _config.getRowCount(); r++) {
			const TableHeaderItemCfg* headerItem = _config.getRowHeader(r);
			if (headerItem) {
				m_verticalHeaderItemCfgs.push_back(headerItem->createCopy());
				this->setVerticalHeaderItem(r, createHeaderItem(headerItem));
			}
			else
			{
				m_verticalHeaderItemCfgs.push_back(nullptr);
			}
		}
	}
	{
		OT_TEST_TABLE_Interval("Setup from config: Set horizontal header");
		m_horizontalHeaderItemCfgs.reserve(_config.getColumnCount());
		QHeaderView* header = this->horizontalHeader();
		for (int c = 0; c < _config.getColumnCount(); c++) {
			const TableHeaderItemCfg* headerItem = _config.getColumnHeader(c);
			if (headerItem) 
			{
				m_horizontalHeaderItemCfgs.push_back(headerItem->createCopy());
				this->setHorizontalHeaderItem(c, createHeaderItem(headerItem));
			}
			else
			{
				m_horizontalHeaderItemCfgs.push_back(nullptr);
			}
		}
	}

	// Initialize data
	{
		OT_TEST_TABLE_Interval("Setup from config: Set data");
		QAbstractItemModel* dataModel = this->model();
		QSignalBlocker blocker(dataModel);

		int rows = _config.getRowCount();
		int columns = _config.getColumnCount();

		for (int r = 0; r < rows; r++) {
			for (int c = 0; c < columns; c++) {
				dataModel->setData(dataModel->index(r, c), QString::fromStdString(_config.getCellText(r, c)));
			}
		}
	}

	m_filterColumnSortingEnabled = _config.getColumnSortingEnabled();
	m_filterRowSortingEnabled = _config.getRowSortingEnabled();

	this->setResizeRequired();
}

ot::TableCfg ot::Table::createConfig() const {
	OT_TEST_TABLE_Interval("Create config");

	TableCfg cfg(this->rowCount(), this->columnCount());
	
	bool hasColumnHeader = false;
	for (int column = 0; column < columnCount(); column++) {
		if (column < m_horizontalHeaderItemCfgs.size() && m_horizontalHeaderItemCfgs[column]) {
			hasColumnHeader = true;
			cfg.setColumnHeader(column, m_horizontalHeaderItemCfgs[column]->createCopy());
		}
	}

	bool hasRowHeader = false;
	for (int row = 0; row < this->rowCount(); row++) {
		if (row < m_verticalHeaderItemCfgs.size() && m_verticalHeaderItemCfgs[row]) {
			hasRowHeader = true;
			cfg.setRowHeader(row, m_verticalHeaderItemCfgs[row]->createCopy());
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
	
	cfg.setRowSortingEnabled(m_filterRowSortingEnabled);
	cfg.setColumnSortingEnabled(m_filterColumnSortingEnabled);

	return cfg;
}

void ot::Table::setContentChanged(bool _changed) {
	if (m_contentChanged == _changed)
	{
		return;
	}
	m_contentChanged = _changed;
	Q_EMIT modifiedChanged(m_contentChanged);
}

void ot::Table::setSelectedCellsBackground(const ot::Color& _color) {
	this->setSelectedCellsBackground(QtFactory::toQColor(_color));
}

void ot::Table::setSelectedCellsBackground(const QColor& _color) {
	OT_TEST_TABLE_Interval("Set selected cells background");

	QSignalBlocker blocker(this);
	
	for (QTableWidgetItem* item : this->selectedItems()) {
		item->setBackground(QBrush(_color));
	}
}

void ot::Table::prepareForDataChange() {
	m_stopResizing = true;
}

ot::TableItem* ot::Table::addItem(int _row, int _column, const QString& _text, const QString& _sortHint) {
	TableItem* newItem = new TableItem(_text, _sortHint);
	this->setItem(_row, _column, newItem);
	return newItem;
}

ot::TableItem* ot::Table::addItem(int _row, int _column, const QIcon& _icon, const QString& _text, const QString& _sortHint) {
	TableItem* newItem = new TableItem(_text, _sortHint);
	newItem->setIcon(_icon);
	this->setItem(_row, _column, newItem);
	return newItem;
}

const ot::TableHeaderItemCfg* ot::Table::getHorizontalHeaderItemCfg(int _column) const
{
	if (_column >= 0 && _column < m_horizontalHeaderItemCfgs.size()) {
		return m_horizontalHeaderItemCfgs.at(_column);
	}
	else
	{
		return nullptr;
	}
}

const ot::TableHeaderItemCfg* ot::Table::getVerticalHeaderItemCfg(int _row) const
{
	if (_row >= 0 && _row < m_verticalHeaderItemCfgs.size()) {
		return m_verticalHeaderItemCfgs.at(_row);
	}
	{
		return nullptr;
	}
}

// ###########################################################################################################################################################################################################################################################################################################################

// Public slots

void ot::Table::slotSaveRequested() {
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
		QSignalBlocker sigBlock(this);
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
		QSignalBlocker sigBlock(this);

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
		QSignalBlocker sigBlock(this);

		this->resizeColumnToContents(_column++);
		QTimer::singleShot(0, [=]() { Table::slotResizeColumnToContent(_column); });
	}
}

void ot::Table::slotRestoreRowSize(int _row) {
	if (m_stopResizing || _row >= this->rowCount()) { // Stop
		return;
	}
	else { // Resize column and queue next
		QSignalBlocker sigBlock(this);

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
		QSignalBlocker sigBlock(this);

		this->resizeRowToContents(_row++);
		QTimer::singleShot(0, [=]() { Table::slotResizeRowToContent(_row); });
	}
}

// ###########################################################################################################################################################################################################################################################################################################################

// Private helper

void ot::Table::ini() {
	m_itemDelegate = new TableItemDelegate(this);

	m_horizontalHeader = new TableHeader(this, Qt::Horizontal);
	this->setHorizontalHeader(m_horizontalHeader);
	m_verticalHeader = new TableHeader(this, Qt::Vertical);
	this->setVerticalHeader(m_verticalHeader);

	QShortcut* saveShortcut = new QShortcut(QKeySequence("Ctrl+S"), this);
	saveShortcut->setContext(Qt::WidgetWithChildrenShortcut);
	
	this->connect(this, &Table::cellChanged, this, &Table::slotCellDataChanged);
	this->connect(saveShortcut, &QShortcut::activated, this, &Table::slotSaveRequested);
}

void ot::Table::resizeColumnsToContentIfNeeded() {
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

void ot::Table::resizeRowsToContentIfNeeded() {
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

void ot::Table::setResizeRequired() {
	if (this->isVisible()) {
		this->resizeColumnsToContentIfNeeded();
	}
	else {
		m_resizeRequired = true;
	}
}

void ot::Table::clearHeaderConfigs()
{
	for (auto* cfg : m_horizontalHeaderItemCfgs)
	{
		if (cfg)
		{
			delete cfg;
		}
	}
	m_horizontalHeaderItemCfgs.clear();
}

QTableWidgetItem* ot::Table::createHeaderItem(const TableHeaderItemCfg* _cfg) const
{
	QTableWidgetItem* item = new QTableWidgetItem;

	std::string headerContent = _cfg->getText();

	headerContent.erase(headerContent.begin(), std::find_if(headerContent.begin(), headerContent.end(), [](unsigned char ch) { return !std::isspace(ch); }));
	headerContent.erase(std::find_if(headerContent.rbegin(), headerContent.rend(), [](unsigned char ch) { return !std::isspace(ch); }).base(), headerContent.end());

	if (headerContent.size() > 1) {
		if (headerContent[0] == headerContent[headerContent.size() - 1] && headerContent[0] == '"')
		{
			headerContent = headerContent.substr(1, headerContent.size() - 2);
		}
	}

	item->setText(QString::fromStdString(headerContent));
	return item;
}
