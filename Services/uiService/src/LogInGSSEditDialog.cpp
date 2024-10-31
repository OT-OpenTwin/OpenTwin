//! @file LogInGSSEditDialog.cpp
//! @author Alexander Kuester (alexk95)
//! @date June 2024
// ###########################################################################################################################################################################################################################################################################################################################

// Frontend header
#include "LogInGSSEditDialog.h"

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTCore/StringHelper.h"
#include "OTWidgets/Label.h"
#include "OTWidgets/LineEdit.h"
#include "OTWidgets/PushButton.h"
#include "OTWidgets/IconManager.h"

// Qt header
#include <QtWidgets/qlayout.h>
#include <QtWidgets/qheaderview.h>
#include <QtWidgets/qmessagebox.h>
#include <QtWidgets/qtablewidget.h>

LogInGSSEditDialogEntry::LogInGSSEditDialogEntry(const LogInGSSEntry& _entry, LogInGSSEditDialog* _dialog)
	: m_dialog(_dialog), m_dataChanged(false)
{
	OTAssertNullptr(m_dialog);

	// Create controls
	m_name = new ot::LineEdit(_entry.getName());
	m_name->setMinimumWidth(180);
	m_name->setPlaceholderText("Name (e.g. Local)");
	m_name->setToolTip("Name (e.g. Local)");
	m_url = new ot::LineEdit(_entry.getUrl());
	m_url->setMinimumWidth(150);
	m_url->setPlaceholderText("Session Service URL (e.g. 127.0.0.1)");
	m_url->setToolTip("Session Service URL (e.g. 127.0.0.1)");
	m_port = new ot::LineEdit(_entry.getPort());
	m_port->setMinimumWidth(150);
	m_port->setPlaceholderText("Session Service Port (e.g. 8091)");
	m_port->setToolTip("Session Service Port (e.g. 8091)");
	m_delete = new QTableWidgetItem;
	auto deleteFlags = m_delete->flags();
	deleteFlags.setFlag(Qt::ItemFlag::ItemIsEditable, false);
	deleteFlags.setFlag(Qt::ItemFlag::ItemIsSelectable, false);
	m_delete->setFlags(deleteFlags);
	m_delete->setIcon(ot::IconManager::getIcon("Table/Delete.png"));

	// Setup table
	m_row = m_dialog->getTable()->rowCount();
	m_dialog->getTable()->insertRow(m_row);
	m_dialog->getTable()->setCellWidget(m_row, (int)LogInGSSEditDialog::TableColumn::Name, m_name);
	m_dialog->getTable()->setCellWidget(m_row, (int)LogInGSSEditDialog::TableColumn::Url, m_url);
	m_dialog->getTable()->setCellWidget(m_row, (int)LogInGSSEditDialog::TableColumn::Port, m_port);
	m_dialog->getTable()->setItem(m_row, (int)LogInGSSEditDialog::TableColumn::Delete, m_delete);

	// Connect signals
	this->connect(m_name, &ot::LineEdit::textChanged, this, &LogInGSSEditDialogEntry::slotDataChanged);
	this->connect(m_url, &ot::LineEdit::textChanged, this, &LogInGSSEditDialogEntry::slotDataChanged);
	this->connect(m_port, &ot::LineEdit::textChanged, this, &LogInGSSEditDialogEntry::slotDataChanged);
	this->connect(m_dialog->getTable(), &QTableWidget::itemPressed, this, &LogInGSSEditDialogEntry::slotDeleteItem);
}

LogInGSSEditDialogEntry::~LogInGSSEditDialogEntry() {

}

LogInGSSEntry LogInGSSEditDialogEntry::createEntry(void) const {
	return LogInGSSEntry(m_name->text(), m_url->text(), m_port->text());
}

bool LogInGSSEditDialogEntry::isEmpty(void) const {
	return m_name->text().isEmpty() && m_url->text().isEmpty() && m_port->text().isEmpty();
}

bool LogInGSSEditDialogEntry::isValid(void) const {
	if (!this->isValidIpV4() && !this->isValidIpV6()) return false;
	if (!this->isValidPort()) return false;

	return !m_name->text().isEmpty();
}

void LogInGSSEditDialogEntry::prepareDestroy(void) {
	m_dialog->getTable()->removeCellWidget(m_row, (int)LogInGSSEditDialog::TableColumn::Name);
	m_dialog->getTable()->removeCellWidget(m_row, (int)LogInGSSEditDialog::TableColumn::Url);
	m_dialog->getTable()->removeCellWidget(m_row, (int)LogInGSSEditDialog::TableColumn::Port);

	delete m_name;
	m_name = nullptr;

	delete m_url;
	m_url = nullptr;

	delete m_port;
	m_port = nullptr;

	delete m_delete;
	m_delete = nullptr;

	m_dialog->getTable()->removeRow(m_row);
}

void LogInGSSEditDialogEntry::slotDataChanged(void) {
	this->updateErrorState();
	m_dataChanged = true;
}

void LogInGSSEditDialogEntry::slotDeleteItem(QTableWidgetItem* _item) {
	if (_item != m_delete) return;
	m_dialog->requestDeleteEntry(m_row);
}

void LogInGSSEditDialogEntry::updateErrorState(void) {
	if (this->isValidIpV4() || this->isValidIpV6()) {
		m_url->unsetInputErrorStateProperty();
	}
	else {
		m_url->setInputErrorStateProperty();
	}

	if (this->isValidPort()) {
		m_port->unsetInputErrorStateProperty();
	}
	else {
		m_port->setInputErrorStateProperty();
	}
}

bool LogInGSSEditDialogEntry::isValidIpV4(void) const {
	if (m_url->text() == "localhost") return true;
	else if (m_url->text().length() > 6) {
		QStringList lst = m_url->text().split(".", Qt::SkipEmptyParts);
		if (lst.count() == 4) {
			bool failed = false;
			for (const QString& str : lst) {
				int number = ot::stringToNumber<int>(str.toStdString(), failed);
				if (failed) return false;
				if (number < 0 || number > 255) return false;
			}
			return true;
		}
	}
	return false;
}

bool LogInGSSEditDialogEntry::isValidIpV6(void) const {
	return false;
}

bool LogInGSSEditDialogEntry::isValidPort(void) const {
	if (m_port->text().isEmpty()) return false;
	
	bool failed = false;
	int number = ot::stringToNumber<int>(m_port->text().toStdString(), failed);
	if (failed) return false;
	
	if (number < 0 || number > 65535) return false;
	else return true;
}

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

LogInGSSEditDialog::LogInGSSEditDialog(const std::vector<LogInGSSEntry>& _entries) 
	: m_dataChanged(false)
{
	using namespace ot;

	// Create layouts
	QVBoxLayout* centralLayout = new QVBoxLayout(this);
	QHBoxLayout* topLayout = new QHBoxLayout;
	QHBoxLayout* tableLayout = new QHBoxLayout;
	QHBoxLayout* buttonLayout = new QHBoxLayout;

	// Create controls
	PushButton* addButton = new PushButton("Add");
	m_table = new QTableWidget(0, (int)TableColumn::TableColumnCount);
	PushButton* saveButton = new PushButton("Save");
	PushButton* cancelButton = new PushButton("Cancel");

	// Setup data
	{
		QTableWidgetItem* headerItem = new QTableWidgetItem("Name");
		auto flags = headerItem->flags();
		flags.setFlag(Qt::ItemFlag::ItemIsEditable, false);
		flags.setFlag(Qt::ItemFlag::ItemIsSelectable, false);
		headerItem->setFlags(flags);
		m_table->setHorizontalHeaderItem((int)TableColumn::Name, headerItem);
	}
	{
		QTableWidgetItem* headerItem = new QTableWidgetItem("URL");
		auto flags = headerItem->flags();
		flags.setFlag(Qt::ItemFlag::ItemIsEditable, false);
		flags.setFlag(Qt::ItemFlag::ItemIsSelectable, false);
		headerItem->setFlags(flags);
		m_table->setHorizontalHeaderItem((int)TableColumn::Url, headerItem);
	}
	{
		QTableWidgetItem* headerItem = new QTableWidgetItem("Port");
		auto flags = headerItem->flags();
		flags.setFlag(Qt::ItemFlag::ItemIsEditable, false);
		flags.setFlag(Qt::ItemFlag::ItemIsSelectable, false);
		headerItem->setFlags(flags);
		m_table->setHorizontalHeaderItem((int)TableColumn::Port, headerItem);
	}
	{
		QTableWidgetItem* headerItem = new QTableWidgetItem;
		auto flags = headerItem->flags();
		flags.setFlag(Qt::ItemFlag::ItemIsEditable, false);
		flags.setFlag(Qt::ItemFlag::ItemIsSelectable, false);
		headerItem->setFlags(flags);
		m_table->setHorizontalHeaderItem((int)TableColumn::Delete, headerItem);
	}

	m_table->horizontalHeader()->setSectionResizeMode((int)TableColumn::Name, QHeaderView::Stretch);
	m_table->horizontalHeader()->setSectionResizeMode((int)TableColumn::Url, QHeaderView::ResizeToContents);
	m_table->horizontalHeader()->setSectionResizeMode((int)TableColumn::Port, QHeaderView::ResizeToContents);
	m_table->horizontalHeader()->setSectionResizeMode((int)TableColumn::Delete, QHeaderView::ResizeToContents);

	for (const LogInGSSEntry& entry : _entries) {
		this->addEntry(entry);
	}

	if (m_entries.empty()) {
		this->addEntry(LogInGSSEntry());
	}

	// Setup layouts
	topLayout->addStretch(1);
	topLayout->addWidget(addButton);

	tableLayout->addWidget(m_table);

	buttonLayout->addStretch(1);
	buttonLayout->addWidget(saveButton);
	buttonLayout->addWidget(cancelButton);

	centralLayout->addLayout(topLayout);
	centralLayout->addLayout(tableLayout);
	centralLayout->addLayout(buttonLayout);

	// Setup window
	this->setWindowTitle("Edit Global Session Services");
	this->setWindowIcon(ot::IconManager::getApplicationIcon());
	this->setMinimumSize(550, 300);

	// Connect signals
	this->connect(addButton, &PushButton::clicked, this, &LogInGSSEditDialog::slotAdd);
	this->connect(saveButton, &PushButton::clicked, this, &LogInGSSEditDialog::slotSave);
	this->connect(cancelButton, &PushButton::clicked, this, &LogInGSSEditDialog::slotCancel);

}

std::vector<LogInGSSEntry> LogInGSSEditDialog::getEntries(void) const {
	std::vector<LogInGSSEntry> entries;

	for (LogInGSSEditDialogEntry* entry : m_entries) {
		if (entry->isValid()) {
			entries.push_back(entry->createEntry());
		}
	}

	return entries;
}

bool LogInGSSEditDialog::hasChangedData(void) const {
	if (m_dataChanged) return true;
	for (LogInGSSEditDialogEntry* entry : m_entries) {
		if (entry->getDataChanged()) return true;
	}
	return false;
}

void LogInGSSEditDialog::slotAdd(void) {
	m_dataChanged = true;
	this->addEntry(LogInGSSEntry());
}

void LogInGSSEditDialog::slotSave(void) {
	if (!this->hasChangedData()) {
		this->close(ot::Dialog::Cancel);
		return;
	}
	
	for (LogInGSSEditDialogEntry* entry : m_entries) {
		if (!entry->isEmpty() && !entry->isValid()) {
			QMessageBox msgBox(QMessageBox::Warning, "Invalid Input", "Invalid input at entry #" + QString::number(entry->getRow() + 1) + "."
				"Please adjust or remove the entry before saving.", QMessageBox::Ok);
			msgBox.exec();
			return;
		}
	}

	this->close(ot::Dialog::Ok);
}

void LogInGSSEditDialog::slotCancel(void) {
	this->close(ot::Dialog::Cancel);
}

void LogInGSSEditDialog::slotDeleteEntry(int _row) {
	OTAssert(_row >= 0 && _row < m_entries.size(), "Invalid index");

	m_dataChanged = true;

	m_entries[_row]->prepareDestroy();
	delete m_entries[_row];
	m_entries.erase(m_entries.begin() + _row);

	_row = 0;
	for (LogInGSSEditDialogEntry* entry : m_entries) {
		entry->setRow(_row++);
	}

	if (m_entries.empty()) {
		this->addEntry(LogInGSSEntry());
	}
}

void LogInGSSEditDialog::addEntry(const LogInGSSEntry& _entry) {
	m_entries.push_back(new LogInGSSEditDialogEntry(_entry, this));
}

void LogInGSSEditDialog::requestDeleteEntry(int _row) {
	QMetaObject::invokeMethod(this, "slotDeleteEntry", Qt::QueuedConnection, Q_ARG(int, _row));
}
