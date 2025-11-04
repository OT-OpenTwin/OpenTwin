// @otlicense
// File: Logging.cpp
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

// OToolkit header
#include "AppBase.h"
#include "Logging.h"
#include "LogModeSetter.h"
#include "QuickLogExport.h"
#include "LoggingFilterView.h"
#include "LogServiceDebugInfo.h"
#include "FileLogImporterDialog.h"
#include "ConnectToLoggerDialog.h"
#include "LogVisualizationItemViewDialog.h"
#include "LogVisualizerColumnWidthSaveDialog.h"

// OT header
#include "OTSystem/DateTime.h"
#include "OTCore/DebugHelper.h"
#include "OTCommunication/ActionTypes.h"
#include "OTCommunication/Msg.h"
#include "OTWidgets/Label.h"
#include "OTWidgets/Splitter.h"
#include "OTWidgets/ComboBox.h"
#include "OTWidgets/Positioning.h"

// Qt header
#include <QtCore/qtimer.h>
#include <QtCore/qsettings.h>
#include <QtCore/qjsondocument.h>
#include <QtCore/qjsonobject.h>
#include <QtCore/qjsonarray.h>
#include <QtWidgets/qlayout.h>
#include <QtWidgets/qpushbutton.h>
#include <QtWidgets/qtablewidget.h>
#include <QtWidgets/qlabel.h>
#include <QtWidgets/qgroupbox.h>
#include <QtWidgets/qstatusbar.h>
#include <QtWidgets/qheaderview.h>
#include <QtWidgets/qcheckbox.h>
#include <QtWidgets/qlistwidget.h>
#include <QtWidgets/qmessagebox.h>
#include <QtWidgets/qlineedit.h>
#include <QtWidgets/qmenubar.h>
#include <QtWidgets/qplaintextedit.h>
#include <QtWidgets/qshortcut.h>
#include <QtWidgets/qapplication.h>
#include <QtWidgets/qfiledialog.h>

#define TABLE_TXT_DETAILED "Detailed"
#define TABLE_TXT_INFO "Info"
#define TABLE_TXT_WARNING "Warning"
#define TABLE_TXT_ERROR "Error"
#define TABLE_TXT_MSG_IN "Msg In"
#define TABLE_TXT_MSG_QUEUE "Msg Queue"
#define TABLE_TXT_MSG_OW_TLS "Msg one-way TLS"
#define TABLE_TXT_MSG_OUT "Msg Out"
#define TABLE_TXT_TEST "Test"

#define LOGVIS_LOG(___message) OTOOLKIT_LOG("Log Visualization", ___message)
#define LOGVIS_LOGW(___message) OTOOLKIT_LOGW("Log Visualization", ___message)
#define LOGVIS_LOGE(___message) OTOOLKIT_LOGE("Log Visualization", ___message)

enum tableColumns {
	tIcon,
	tTimeGlobal,
	tTimeLocal,
	tUser,
	tProject,
	tService,
	tType,
	tFunction,
	tMessage,
	tCount
};

Logging::Logging()
	: m_warningCount(0), m_errorCount(0), m_filterView(nullptr), m_columnWidthTimer(nullptr), m_autoConnect(nullptr),
	m_autoScrollToBottom(nullptr), m_connectButton(nullptr), m_errorCountLabel(nullptr), m_exportButton(nullptr),
	m_ignoreNewMessages(nullptr), m_convertToLocalTime(nullptr), m_importButton(nullptr), m_messageCountLabel(nullptr), m_table(nullptr),
	m_warningCountLabel(nullptr), m_logModeSetter(nullptr), m_root(nullptr), m_serviceDebugInfo(nullptr), m_cellWidthMode(nullptr)
{
	m_columnWidthTimer = new QTimer(this);
	m_columnWidthTimer->setInterval(0);
	m_columnWidthTimer->setSingleShot(true);
	this->connect(m_columnWidthTimer, &QTimer::timeout, this, &Logging::slotUpdateColumnWidth);

	m_intervalTimer = new QTimer(this);
	m_intervalTimer->setInterval(1000);
	m_intervalTimer->setSingleShot(true);
	this->connect(m_intervalTimer, &QTimer::timeout, this, &Logging::slotIntervalTimeout);
}

Logging::~Logging() {

}

QString Logging::getToolName() const {
	return QString("Logging");
}

bool Logging::runTool(QMenu* _rootMenu, otoolkit::ToolWidgets& _content) {
	LOGVIS_LOG("Initializing Logging...");

	// Create layouts
	QWidget* centralLayoutW = new QWidget;
	QVBoxLayout* centralLayout = new QVBoxLayout(centralLayoutW);

	QHBoxLayout* buttonLayout = new QHBoxLayout;

	// Create controls	
	ot::Splitter* splitter = new ot::Splitter;
	m_root = this->createCentralWidgetView(splitter, "Log Visualization");
	_content.addView(m_root);

	m_logModeSetter = new LogModeSetter;
	_content.addView(this->createSideWidgetView(m_logModeSetter->getRootWidget(), "Log Switch"));

	m_filterView = new LoggingFilterView;
	_content.addView(this->createSideWidgetView(m_filterView->getRootWidget(), "Log Options"));

	m_serviceDebugInfo = new LogServiceDebugInfo;
	_content.addView(this->createSideWidgetView(m_serviceDebugInfo->getRootWidget(), "Service Info"));

	splitter->setOrientation(Qt::Orientation::Vertical);

	m_cellWidthMode = new ot::ComboBox;
	m_cellWidthMode->setEditable(false);
	m_cellWidthMode->setPlaceholderText("Restore Columns");

	m_convertToLocalTime = new QCheckBox("Convert to Local Time");
	m_ignoreNewMessages = new QCheckBox("Ignore new messages");
	m_autoScrollToBottom = new QCheckBox("Auto scroll to bottom");

	QPushButton* btnClear = new QPushButton("Clear");
	btnClear->setToolTip("Will clear the Log Messages");
	this->connect(btnClear, &QPushButton::clicked, this, &Logging::slotClear);

	QPushButton* btnClearAll = new QPushButton("Clear All");
	btnClearAll->setToolTip("Will clear the Log Messages and the Service Filter list");
	this->connect(btnClearAll, &QPushButton::clicked, this, &Logging::slotClearAll);

	m_table = new QTableWidget(0, tCount);

	m_messageCountLabel = new QLabel;
	m_errorCountLabel = new QLabel;
	m_warningCountLabel = new QLabel;

	_content.addStatusWidget(m_errorCountLabel);
	_content.addStatusWidget(m_warningCountLabel);
	_content.addStatusWidget(m_messageCountLabel);

	// Setup layouts
	centralLayout->addLayout(buttonLayout);

	buttonLayout->addStretch(1);
	buttonLayout->addWidget(m_cellWidthMode, 0);
	buttonLayout->addWidget(m_convertToLocalTime, 0);
	buttonLayout->addWidget(m_ignoreNewMessages, 0);
	buttonLayout->addWidget(m_autoScrollToBottom, 0);
	buttonLayout->addWidget(btnClear, 0);
	buttonLayout->addWidget(btnClearAll, 0);

	splitter->addWidget(centralLayoutW);
	splitter->addWidget(m_table);

	// Setup controls
	m_table->setHorizontalHeaderItem(tIcon, new QTableWidgetItem());
	m_table->setHorizontalHeaderItem(tTimeGlobal, new QTableWidgetItem("Global Time"));
	m_table->setHorizontalHeaderItem(tTimeLocal, new QTableWidgetItem("Local Time"));
	m_table->setHorizontalHeaderItem(tUser, new QTableWidgetItem("User"));
	m_table->setHorizontalHeaderItem(tProject, new QTableWidgetItem("Project"));
	m_table->setHorizontalHeaderItem(tService, new QTableWidgetItem("Service"));
	m_table->setHorizontalHeaderItem(tType, new QTableWidgetItem("Type"));
	m_table->setHorizontalHeaderItem(tFunction, new QTableWidgetItem("Function"));
	m_table->setHorizontalHeaderItem(tMessage, new QTableWidgetItem("Message"));

	m_table->setAlternatingRowColors(true);
	m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
	m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
	m_table->setSelectionMode(QAbstractItemView::SingleSelection);
	m_table->installEventFilter(this);

	splitter->setStretchFactor(1, 1);
	
	// Initialize colors and text
	this->slotUpdateCheckboxColors();
	this->updateCountLabels();

	this->connect(m_filterView, &LoggingFilterView::filterChanged, this, &Logging::slotFilterChanged);
	this->connect(m_filterView, &LoggingFilterView::messageLimitChanged, this, &Logging::slotMessageLimitChanged);
	this->connect(m_filterView, &LoggingFilterView::useIntervalChaged, this, &Logging::slotUseIntervalChanged);

	// Connect checkbox color signals
	connect(m_convertToLocalTime, &QCheckBox::stateChanged, this, &Logging::slotUpdateCheckboxColors);
	connect(m_convertToLocalTime, &QCheckBox::stateChanged, this, &Logging::slotRefillData);
	connect(m_ignoreNewMessages, &QCheckBox::stateChanged, this, &Logging::slotUpdateCheckboxColors);
	connect(m_autoScrollToBottom, &QCheckBox::stateChanged, this, &Logging::slotUpdateCheckboxColors);

	// Connect signals
	connect(m_autoScrollToBottom, &QCheckBox::stateChanged, this, &Logging::slotAutoScrollToBottomChanged);
	connect(m_cellWidthMode, &QComboBox::currentTextChanged, this, &Logging::slotColumnWidthModeChanged);
	connect(m_table, &QTableWidget::itemDoubleClicked, this, &Logging::slotViewCellContent);

	// Connection actions
	connectAction(OT_ACTION_CMD_Log, this, &Logging::handleNewLog);

	// Create menu bar
	m_connectButton = _rootMenu->addAction(QIcon(":/images/Disconnected.png"), "Connect");
	m_autoConnect = _rootMenu->addAction("Auto-Connect");

	_rootMenu->addSeparator();
	m_importButton = _rootMenu->addAction(QIcon(":images/Import.png"), "Import");
	QAction* importFromFileLog = _rootMenu->addAction(QIcon(":images/Import.png"), "Import File logs");
	m_exportButton = _rootMenu->addAction(QIcon(":images/Export.png"), "Export");

	connect(m_connectButton, &QAction::triggered, this, &Logging::slotConnect);
	connect(m_autoConnect, &QAction::triggered, this, &Logging::slotToggleAutoConnect);
	connect(m_importButton, &QAction::triggered, this, &Logging::slotImport);
	connect(importFromFileLog, &QAction::triggered, this, &Logging::slotImportFileLogs);
	connect(m_exportButton, &QAction::triggered, this, &Logging::slotExport);

	LOGVIS_LOG("Initialization completed");

	return true;
}

void Logging::restoreToolSettings(QSettings& _settings) {
	m_convertToLocalTime->setChecked(_settings.value("Logging.ConvertToLocal", false).toBool());
	m_autoScrollToBottom->setChecked(_settings.value("Logging.AutoScrollToBottom", true).toBool());
	m_columnWidthTmp = _settings.value("Logging.Table.ColumnWidth", "").toString();
	
	m_columnWidthData.clear();
	QByteArray columnWidthData = _settings.value("Logging.Table.ColumnWidthData", "").toByteArray();
	if (!columnWidthData.isEmpty()) {
		QJsonDocument doc = QJsonDocument::fromJson(columnWidthData);
		if (doc.isArray()) {
			QJsonArray arr = doc.array();
			for (int i = 0; i < arr.count(); i++) {
				if (arr.at(i).isObject()) {
					QJsonObject it = arr.at(i).toObject();
					if (it.isEmpty() || !it.contains("Name") || !it.value("Name").isString() || !it.contains("Widths") || !it.value("Widths").isArray()) {
						continue;
					}
					QJsonArray wArr = it.value("Widths").toArray();
					std::list<int> lst;
					for (int j = 0; j < wArr.count(); j++) {
						if (wArr.at(j).isDouble()) {
							lst.push_back(wArr.at(j).toInt());
						}
					}
					if (lst.empty()) {
						continue;
					}

					m_columnWidthData.insert_or_assign(it.value("Name").toString(), std::move(lst));
				}
			}
		}
	}

	this->rebuildColumnWidthData();

	m_logModeSetter->restoreSettings(_settings);
	m_filterView->restoreSettings(_settings);

	bool needAutoConnect = _settings.value("Logging.AutoConnect", false).toBool();
	m_autoConnect->setIcon(needAutoConnect ? QIcon(":/images/True.png") : QIcon(":/images/False.png"));

	if (needAutoConnect) {
		LOGVIS_LOG("Auto connect requested, request queued");
		QMetaObject::invokeMethod(this, &Logging::slotAutoConnect, Qt::QueuedConnection);
	}
}

bool Logging::prepareToolShutdown(QSettings& _settings) {
	_settings.setValue("Logging.ConvertToLocal", m_convertToLocalTime->isChecked());
	_settings.setValue("Logging.AutoScrollToBottom", m_autoScrollToBottom->isChecked());

	QString tableColumnWidths;
	for (int i = 0; i < m_table->columnCount(); i++) {
		tableColumnWidths.append(QString::number(m_table->columnWidth(i)) + ";");
	}
	_settings.setValue("Logging.Table.ColumnWidth", tableColumnWidths);

	m_logModeSetter->saveSettings(_settings);
	m_filterView->saveSettings(_settings);

	QJsonArray arr;
	for (const auto& it : m_columnWidthData) {
		QJsonObject obj;
		obj.insert("Name", it.first);
		QJsonArray wArr;
		for (int w : it.second) {
			wArr.append(w);
		}
		obj.insert("Widths", wArr);
		arr.append(obj);
	}
	_settings.setValue("Logging.Table.ColumnWidthData", QJsonDocument(arr).toJson(QJsonDocument::Compact));

	if (this->disconnectFromLogger()) {
		return true;
	}
	else {
		LOGVIS_LOGE("Failed to send deregistration request to LoggerService \"" + QString::fromStdString(m_loggerUrl) + "\"");
		return false;
	}
}

bool Logging::eventFilter(QObject* _obj, QEvent* _event) {
	if (_obj == m_table && _event->type() == QEvent::KeyPress) {
		QKeyEvent* keyEvent = dynamic_cast<QKeyEvent*>(_event);
		if (!keyEvent) {
			LOGVIS_LOGE("Key event is null");
			return false;
		}
		if (keyEvent->key() == Qt::Key_Return) {
			auto items = m_table->selectedItems();
			while (!items.isEmpty()) {
				QTableWidgetItem* item = items.front();
				items.pop_front();
				if (item) {
					this->slotViewCellContent(item);
					return true;
				}
			}
		}
	}
	return false;
}

void Logging::slotConnect() {
	this->connectToLogger(false);
}

void Logging::slotAutoConnect() {
	this->connectToLogger(true);
}

void Logging::slotImport() {
	otoolkit::SettingsRef settings = AppBase::instance()->createSettingsInstance();
	QString fn = QFileDialog::getOpenFileName(m_table, "Import Log Messages", settings->value("Logging.LastExportedFile", "").toString(), "OpenTwin Log (*.otlog.json)");
	if (fn.isEmpty()) return;

	QFile f(fn);
	if (!f.open(QIODevice::ReadOnly)) {
		LOGVIS_LOGE("Failed to open file for reading. File: \"" + fn + "\"");
		return;
	}

	std::string data = f.readAll().toStdString();
	f.close();

	std::list<ot::LogMessage> messages;

	if (!ot::importLogMessagesFromString(data, messages)) {
		return;
	}

	if (messages.empty()) {
		LOGVIS_LOGW("No data imported");
		return;
	}

	QSignalBlocker blocker(m_ignoreNewMessages);
	bool isIgnoring = m_ignoreNewMessages->isChecked();
	m_ignoreNewMessages->setChecked(false);

	this->slotClear();
	this->appendLogMessages(std::move(messages));

	m_ignoreNewMessages->setChecked(isIgnoring);

	settings->setValue("Logging.LastExportedFile", fn);
	LOGVIS_LOG("Log Messages successfully import from file \"" + fn + "\"");
}

void Logging::slotExport() {
	if (m_messages.empty()) {
		LOGVIS_LOGW("No messages to export");
		return;
	}

	otoolkit::SettingsRef settings = AppBase::instance()->createSettingsInstance();
	QString fn = QFileDialog::getSaveFileName(m_table, "Export Log Messages", settings->value("Logging.LastExportedFile", "").toString(), "OpenTwin Log (*.otlog.json)");
	if (fn.isEmpty()) return;

	QFile f(fn);
	if (!f.open(QIODevice::WriteOnly)) {
		LOGVIS_LOGE("Failed to open file for writing. File: \"" + fn + "\"");
		return;
	}

	std::string data = ot::exportLogMessagesToString(m_messages);

	f.write(QByteArray::fromStdString(data));
	f.close();

	settings->setValue("Logging.LastExportedFile", fn);
	LOGVIS_LOG("Log Messages successfully exported to file \"" + fn + "\"");
}

void Logging::slotRefillData() {
	std::list<ot::LogMessage> messages = std::move(m_messages);
	this->slotClear();

	// Temporarily disable ignoring new messages to allow appending
	QSignalBlocker blocker(m_ignoreNewMessages);
	bool isIgnoring = m_ignoreNewMessages->isChecked();
	m_ignoreNewMessages->setChecked(false);

	this->appendLogMessages(std::move(messages));

	m_ignoreNewMessages->setChecked(isIgnoring);
}

void Logging::slotClear() {
	m_table->setRowCount(0);
	m_messages.clear();
	m_errorCount = 0;
	m_warningCount = 0;
	this->updateCountLabels();
}

void Logging::slotClearAll() {
	this->slotClear();
	m_filterView->reset();
	LOGVIS_LOG("Logging Clear All completed");
}

void Logging::slotFilterChanged() {
	int r = 0;
	QList<QTableWidgetItem*> lst = m_table->selectedItems();
	int focusedRow = -1;
	for (const QTableWidgetItem* item : lst) {
		if (item->row() != focusedRow && focusedRow != -1) {
			OT_LOG_E("Invalid row selection");
			focusedRow = -1;
			break;
		}
		focusedRow = item->row();
	}

	for (const ot::LogMessage& msg : m_messages) {
		bool messageVisible = m_filterView->filterMessage(msg);
		m_table->setRowHidden(r++, !messageVisible);
	}

	if (focusedRow >= 0) {
		QMetaObject::invokeMethod(this, &Logging::slotScrollToItem, Qt::QueuedConnection, focusedRow);
	}
}

void Logging::slotAutoScrollToBottomChanged() {
	if (m_autoScrollToBottom->isChecked()) m_table->scrollToBottom();
}

void Logging::slotUpdateCheckboxColors() {
	QString red("QCheckBox { color: #c02020; }");
	QString green("QCheckBox { color: #20c020; }");
	QString def("");

	m_convertToLocalTime->setStyleSheet(m_convertToLocalTime->isChecked() ? green : def);
	m_ignoreNewMessages->setStyleSheet(m_ignoreNewMessages->isChecked() ? red : def);
	m_autoScrollToBottom->setStyleSheet(m_autoScrollToBottom->isChecked() ? green : def);
}

void Logging::slotToggleAutoConnect() {
	otoolkit::SettingsRef settings = AppBase::instance()->createSettingsInstance();
	bool is = !settings->value("Logging.AutoConnect", false).toBool();
	m_autoConnect->setIcon(is ? QIcon(":/images/True.png") : QIcon(":/images/False.png"));
	settings->setValue("Logging.AutoConnect", is);
}

void Logging::slotViewCellContent(QTableWidgetItem* _itm) {
	size_t i = 0;
	for (auto itm : m_messages) {
		if (i++ == _itm->row()) {
			LogVisualizationItemViewDialog dia(itm, i, m_table);
			dia.showDialog();
			return;
		}
	}
}

void Logging::slotScrollToItem(int _row) {
	if (_row < m_table->rowCount()) {
		QTableWidgetItem* item = m_table->item(_row, tableColumns::tMessage);
		if (!m_table->isRowHidden(_row)) {
			m_table->scrollToItem(item);
		}
	}
	
}

void Logging::slotColumnWidthModeChanged(const QString& _text) {
	if (_text.isEmpty() || _text == "> Restore Columns <") {
		return;
	}

	if (_text == "< Save >") {
		QStringList existingNames;
		for (const auto& it : m_columnWidthData) {
			existingNames.push_back(it.first);
		}
		LogVisualizerColumnWidthSaveDialog dia(existingNames);
		ot::Positioning::centerWidgetOnParent(m_root->getViewWidget(), &dia);
		if (dia.showDialog() != ot::Dialog::Ok) {
			m_cellWidthMode->setCurrentIndex(0);
			return;
		}

		existingNames = dia.getExistingNames();

		for (auto it = m_columnWidthData.begin(); it != m_columnWidthData.end(); ) {
			bool exists = false;
			for (const QString& n : existingNames) {
				if (n == it->first) {
					exists = true;
					break;
				}
			}
			if (!exists) {
				it = m_columnWidthData.erase(it);
			}
			else {
				it++;
			}
		}

		std::list<int> widths;
		for (int i = 0; i < m_table->columnCount(); i++) {
			widths.push_back(m_table->columnWidth(i));
		}
		m_columnWidthData.insert_or_assign(dia.getNewName(), std::move(widths));
	}
	else if (_text == "< Default >") {
		std::list<int> widths;
		for (int i = 0; i < m_table->columnCount(); i++) {
			switch (i) {
			case tIcon: widths.push_back(30); break;
			case tTimeGlobal: widths.push_back(140); break;
			case tTimeLocal: widths.push_back(140); break;
			case tUser: widths.push_back(100); break;
			case tProject: widths.push_back(100); break;
			case tService: widths.push_back(150); break;
			case tType: widths.push_back(80); break;
			case tFunction: widths.push_back(150); break;
			case tMessage: widths.push_back(600); break;
			default:
				widths.push_back(50);
				break;
			}
		}
		this->applyColumnWidthData(widths);
	}
	else {
		auto it = m_columnWidthData.find(_text);
		if (it == m_columnWidthData.end()) {
			LOGVIS_LOGE("Failed to find column width data for name \"" + _text + "\"");
			return;
		}
		else {
			if (!this->applyColumnWidthData(it->second)) {
				m_columnWidthData.erase(it);
			}
		}
	}

	this->rebuildColumnWidthData();
}

void Logging::slotMessageLimitChanged(int _limit) {
	_limit = std::max(10, _limit);
	
	size_t limit = static_cast<size_t>(_limit);

	while (m_messages.size() > limit) {
		if (m_table->rowCount() > 0) {
			m_table->removeRow(0);
		}
		
		const ot::LogMessage& msg = m_messages.front();
		if (msg.getFlags() & ot::WARNING_LOG) {
			m_warningCount--;
		}
		else if (msg.getFlags() & ot::ERROR_LOG) {
			m_errorCount--;
		}
		m_messages.pop_front();
	}
	this->updateCountLabels();
}

void Logging::slotUseIntervalChanged() {
	m_intervalTimer->stop();

	if (m_filterView->getUseInterval()) {	
		m_intervalTimer->setInterval(m_filterView->getIntervalMilliseconds());
		m_intervalTimer->start();
	}
	else {
		if (!m_newMessages.empty()) {
			this->resizeNewMessageBuffer();
			this->appendLogMessages(std::move(m_newMessages));
			m_newMessages.clear();
		}
	}
}

void Logging::slotIntervalTimeout() {
	this->resizeNewMessageBuffer();
	this->appendLogMessages(std::move(m_newMessages));
	m_newMessages.clear();
	if (m_filterView->getUseInterval()) {
		m_intervalTimer->start();
	}
}

void Logging::appendLogMessage(ot::LogMessage&& _msg) {
	if (m_serviceDebugInfo && _msg.getText().find(ot::DebugHelper::getSetupCompletedMessagePrefix()) == 0) {
		ot::JsonDocument debugInfoDoc;
		if (debugInfoDoc.fromJson(_msg.getText().substr(ot::DebugHelper::getSetupCompletedMessagePrefix().length()))) {
			ot::ServiceDebugInformation debugInfo;
			debugInfo.setFromJsonObject(debugInfoDoc.getConstObject());

			m_serviceDebugInfo->appendServiceDebugInfo(debugInfo);
		}
	}

	if (m_ignoreNewMessages->isChecked()) {
		return;
	}

	m_filterView->setFilterLock(true);

	// Check if we reached the message limit
	if (m_messages.size() >= m_filterView->getMessageLimit()) {
		// Remove first entry
		m_table->removeRow(0);
		const ot::LogMessage& msg = m_messages.front();
		if (msg.getFlags() & ot::WARNING_LOG) {
			m_warningCount--;
		}
		else if (msg.getFlags() & ot::ERROR_LOG) {
			m_errorCount--;
		}
		m_messages.pop_front();
	}

	int r = m_table->rowCount();
	m_table->insertRow(r);

	QString serviceName = QString::fromStdString(_msg.getServiceName());
	QString lclTime;
	QString globalTime;
	if (m_convertToLocalTime->isChecked()) {
		lclTime = QString::fromStdString(ot::DateTime::timestampFromMsec(_msg.getLocalSystemTime(), ot::DateTime::Simple));
		globalTime = QString::fromStdString(ot::DateTime::timestampFromMsec(_msg.getGlobalSystemTime(), ot::DateTime::Simple));
	}
	else {
		lclTime = QString::fromStdString(ot::DateTime::timestampFromMsec(_msg.getLocalSystemTime(), ot::DateTime::SimpleUTC));
		globalTime = QString::fromStdString(ot::DateTime::timestampFromMsec(_msg.getGlobalSystemTime(), ot::DateTime::SimpleUTC));
	}
	
	QTableWidgetItem* iconItm = new QTableWidgetItem("");
	if (_msg.getFlags() & ot::WARNING_LOG) {
		iconItm->setIcon(QIcon(":/images/Warning.png"));
		m_warningCount++;
	}
	else if (_msg.getFlags() & ot::ERROR_LOG) {
		iconItm->setIcon(QIcon(":/images/Error.png"));
		m_errorCount++;
	}
	else if (_msg.getFlags() & ot::TEST_LOG) {
		iconItm->setIcon(QIcon(":/images/Test.png"));
		m_errorCount++;
	}
	else {
		iconItm->setIcon(QIcon(":/images/Info.png"));
	}
	this->iniTableItem(r, tIcon, iconItm);

	this->iniTableItem(r, tType, new QTableWidgetItem(logMessageTypeString(_msg)));
	this->iniTableItem(r, tTimeGlobal, new QTableWidgetItem(globalTime));
	this->iniTableItem(r, tTimeLocal, new QTableWidgetItem(lclTime));
	this->iniTableItem(r, tUser, new QTableWidgetItem(QString::fromStdString(_msg.getUserName())));
	this->iniTableItem(r, tProject, new QTableWidgetItem(QString::fromStdString(_msg.getProjectName())));
	this->iniTableItem(r, tService, new QTableWidgetItem(serviceName));
	this->iniTableItem(r, tFunction, new QTableWidgetItem(QString::fromStdString(_msg.getFunctionName())));
	this->iniTableItem(r, tMessage, new QTableWidgetItem(QString::fromStdString(_msg.getText())));

	// Store message
	m_messages.push_back(std::move(_msg));

	// If required scroll to the last entry in the table
	if (m_autoScrollToBottom->isChecked()) m_table->scrollToBottom();

	// Check if the service name is in the service name list
	m_filterView->setFilterLock(false);

	// Update status labels (warnings and errors were increased while checking the typeString)
	this->updateCountLabels();

	// Check the filter for the new entry
	m_table->setRowHidden(r, !m_filterView->filterMessage(m_messages.back()));
}

void Logging::appendLogMessages(std::list<ot::LogMessage>&& _messages) {
	if (!m_table) {
		return;
	}

	this->resizeMessageBuffer(_messages);

	bool actb = m_autoScrollToBottom->isChecked();
	m_autoScrollToBottom->setChecked(false);
	for (ot::LogMessage& msg : _messages) {
		this->appendLogMessage(std::move(msg));
	}
	m_autoScrollToBottom->setChecked(actb);
}

void Logging::newMessages(std::list<ot::LogMessage>&& _messages) {
	if (_messages.empty() && m_ignoreNewMessages->isChecked()) {
		return;
	}

	if (m_filterView->getUseInterval()) {
		m_newMessages.splice(m_newMessages.end(), std::move(_messages));
	}
	else {
		this->appendLogMessages(std::move(_messages));
	}
}

void Logging::slotUpdateColumnWidth() {
	QStringList tableColumnWidthsList = m_columnWidthTmp.split(";", Qt::SkipEmptyParts);
	m_columnWidthTmp.clear();
	if (tableColumnWidthsList.count() == m_table->columnCount()) {
		int column = 0;
		for (auto w : tableColumnWidthsList) {
			m_table->setColumnWidth(column++, w.toInt());
		}
	}
}

void Logging::slotImportFileLogs() {
	FileLogImporterDialog dia(m_table);
	if (dia.showDialog() == ot::Dialog::Ok) {
		std::list<ot::LogMessage> messages;
		dia.grabLogMessages(messages);
		if (messages.empty()) {
			return;
		}

		QSignalBlocker blocker(m_ignoreNewMessages);
		bool isIgnoring = m_ignoreNewMessages->isChecked();
		m_ignoreNewMessages->setChecked(false);
		this->appendLogMessages(std::move(messages));
		m_ignoreNewMessages->setChecked(isIgnoring);
	}
}

void Logging::runQuickExport() {
	if (m_loggerUrl.empty()) {
		ConnectToLoggerDialog dia;
		dia.queueConnectRequest();
		dia.queueRecenterRequest();
		dia.exec();

		if (!dia.dialogResult() == ot::Dialog::Ok) {
			return;
		}

		m_loggerUrl = dia.loggerServiceUrl().toStdString();

		LOGVIS_LOG("Successfully connected to LoggerService. Refreshing log messages...");

		m_connectButton->setText("Disconnect");
		m_connectButton->setIcon(QIcon(":/images/Connected.png"));
		this->slotClear();

		std::list<ot::LogMessage> buffer;
		dia.grabMessageBuffer(buffer);
		this->appendLogMessages(std::move(buffer));
	}

	if (m_messages.empty()) {
		LOGVIS_LOGW("No messages to export");
		return;
	}

	QuickLogExport exportDia(m_messages);
	ot::Positioning::centerWidgetOnParent(m_root->getViewWidget(), &exportDia);

	if (exportDia.showDialog() == ot::Dialog::Ok) {
		if (exportDia.isAutoClose()) AppBase::instance()->close();
	}
}

QString Logging::logMessageTypeString(const ot::LogMessage& _msg) {
	QString typeString;
	if (_msg.getFlags() & ot::DETAILED_LOG) {
		if (!typeString.isEmpty()) typeString.append(" | ");
		typeString.append(TABLE_TXT_DETAILED);
	}
	if (_msg.getFlags() & ot::INFORMATION_LOG) {
		if (!typeString.isEmpty()) typeString.append(" | ");
		typeString.append(TABLE_TXT_INFO);
	}
	if (_msg.getFlags() & ot::ERROR_LOG) {
		if (!typeString.isEmpty()) typeString.append(" | ");
		typeString.append(TABLE_TXT_ERROR);
	}
	if (_msg.getFlags() & ot::WARNING_LOG) {
		if (!typeString.isEmpty()) typeString.append(" | ");
		typeString.append(TABLE_TXT_WARNING);
	}
	if (_msg.getFlags() & ot::INBOUND_MESSAGE_LOG) {
		if (!typeString.isEmpty()) typeString.append(" | ");
		typeString.append(TABLE_TXT_MSG_IN);
	}
	if (_msg.getFlags() & ot::QUEUED_INBOUND_MESSAGE_LOG) {
		if (!typeString.isEmpty()) typeString.append(" | ");
		typeString.append(TABLE_TXT_MSG_QUEUE);
	}
	if (_msg.getFlags() & ot::ONEWAY_TLS_INBOUND_MESSAGE_LOG) {
		if (!typeString.isEmpty()) typeString.append(" | ");
		typeString.append(TABLE_TXT_MSG_OW_TLS);
	}
	if (_msg.getFlags() & ot::OUTGOING_MESSAGE_LOG) {
		if (!typeString.isEmpty()) typeString.append(" | ");
		typeString.append(TABLE_TXT_MSG_OUT);
	}
	if (_msg.getFlags() & ot::TEST_LOG) {
		if (!typeString.isEmpty()) typeString.append(" | ");
		typeString.append(TABLE_TXT_TEST);
	}

	return typeString;
}

void Logging::handleNewLog(ot::JsonDocument& _doc) {
	std::list<ot::LogMessage> messages;
	if (_doc.HasMember(OT_ACTION_PARAM_LOGS)) {
		for (const ot::ConstJsonObject& logObj : ot::json::getObjectList(_doc, OT_ACTION_PARAM_LOGS)) {
			ot::LogMessage msg;
			msg.setFromJsonObject(logObj);
			msg.setCurrentTimeAsGlobalSystemTime();
			messages.push_back(msg);
		}
	}
	else if (_doc.HasMember(OT_ACTION_PARAM_LOG)) {
		ot::LogMessage msg;
		msg.setFromJsonObject(ot::json::getObject(_doc, OT_ACTION_PARAM_LOG));
		messages.push_back(msg);
	}
	else {
		LOGVIS_LOGE("Invalid log message received");
		return;
	}
	if (messages.empty()) {
		return;
	}
	this->newMessages(std::move(messages));
}

void Logging::iniTableItem(int _row, int _column, QTableWidgetItem* _itm) {
	//_itm->setFlags(_itm->flags() & (~Qt::ItemIsEditable) & (~Qt::ItemIsSelectable));
	m_table->setItem(_row, _column, _itm);
}

void Logging::updateCountLabels() {
	//m_errorCountLabel->setHidden(m_errorCount == 0);
	//m_warningCountLabel->setHidden(m_warningCount == 0);

	m_messageCountLabel->setText("Log count: " + QString::number(m_messages.size()));
	m_warningCountLabel->setText("Warnings: " + QString::number(m_warningCount));
	m_errorCountLabel->setText("Errors: " + QString::number(m_errorCount));
}

void Logging::connectToLogger(bool _isAutoConnect) {
	if (!m_loggerUrl.empty()) {
		this->disconnectFromLogger();
		return;
	}

	ConnectToLoggerDialog dia;
	if (_isAutoConnect) {
		dia.queueConnectRequest();
	}
	dia.queueRecenterRequest();
	dia.exec();

	if (!dia.dialogResult() == ot::Dialog::Ok) {
		return;
	}

	m_loggerUrl = dia.loggerServiceUrl().toStdString();

	LOGVIS_LOG("Successfully connected to LoggerService. Refreshing log messages...");

	m_connectButton->setText("Disconnect");
	m_connectButton->setIcon(QIcon(":/images/Connected.png"));
	this->slotClear();


	std::list<ot::LogMessage> buffer;
	dia.grabMessageBuffer(buffer);

	if (!buffer.empty()) {
		this->appendLogMessages(std::move(buffer));
	}
	
	if (!m_columnWidthTmp.isEmpty()) {
		m_columnWidthTimer->start();
	}
	
	LOGVIS_LOG("Done.");
}

bool Logging::disconnectFromLogger() {
	if (m_loggerUrl.empty()) return true;

	std::string response;
	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_RemoveService, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_SERVICE_URL, ot::JsonString(AppBase::instance()->url().toStdString(), doc.GetAllocator()), doc.GetAllocator());

	if (!ot::msg::send("", m_loggerUrl, ot::EXECUTE, doc.toJson(), response, ot::msg::defaultTimeout, ot::msg::NoRequestFlags)) {
		LOGVIS_LOGE("Failed to send remove service request to logger service.");
		return false;
	}

	LOGVIS_LOG("Successfully deregistered at LoggerService \"" + QString::fromStdString(m_loggerUrl) + "\"");
	m_loggerUrl.clear();

	m_connectButton->setText("Connect");
	m_connectButton->setIcon(QIcon(":/images/Disconnected.png"));

	return true;
}

void Logging::rebuildColumnWidthData() {
	m_cellWidthMode->clear();
	m_cellWidthMode->addItem("> Restore Columns <");
	m_cellWidthMode->addItem("< Default >");
	for (const auto& it : m_columnWidthData) {
		m_cellWidthMode->addItem(it.first);
	}
	m_cellWidthMode->addItem("< Save >");
	m_cellWidthMode->setCurrentIndex(0);
}

bool Logging::applyColumnWidthData(const std::list<int>& _widths) {
	int ix = 0;
	if (_widths.empty()) {
		LOGVIS_LOGE("No column width data");
		return false;
	}

	for (int w : _widths) {
		if (w <= 0) {
			LOGVIS_LOGE("Invalid column width data");
			return false;
		}
		if (ix < m_table->columnCount()) {
			m_table->setColumnWidth(ix++, w);
		}
		else {
			return false;
		}
	}

	return ix == m_table->columnCount();
}

void Logging::resizeMessageBuffer(std::list<ot::LogMessage>& _buffer) {
	size_t limit = static_cast<size_t>(m_filterView->getMessageLimit());
	size_t size = _buffer.size();
	while (size > limit) {
		_buffer.pop_front();
		size--;
	}
}

void Logging::resizeNewMessageBuffer() {
	this->resizeMessageBuffer(m_newMessages);
}
