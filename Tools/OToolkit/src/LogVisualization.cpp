//! @file LogVisualization.cpp
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

#include "LogVisualization.h"
#include "AppBase.h"
#include "ConnectToLoggerDialog.h"

// OT header
#include "OTCommunication/ActionTypes.h"
#include "OTCommunication/Msg.h"

// Qt header
#include <QtCore/qtimer.h>
#include <QtCore/qsettings.h>
#include <QtCore/qjsondocument.h>
#include <QtCore/qjsonobject.h>
#include <QtCore/qjsonarray.h>
#include <QtWidgets/qsplitter.h>
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

#define LOGVIS_LOG(___message) OTOOLKIT_LOG("Log Visualization", ___message)
#define LOGVIS_LOGW(___message) OTOOLKIT_LOGW("Log Visualization", ___message)
#define LOGVIS_LOGE(___message) OTOOLKIT_LOGE("Log Visualization", ___message)

enum tableColumns {
	tIcon,
	tTimeGlobal,
	tTimeLocal,
	tService,
	tType,
	tFunction,
	tMessage,
	tCount
};

LogVisualization::LogVisualization()
	: m_filterLock(false), m_warningCount(0), m_errorCount(0)
{
	
}

LogVisualization::~LogVisualization() {
	otoolkit::SettingsRef settings = AppBase::instance()->createSettingsInstance();

	settings->setValue("LogVisualization.AutoScrollToBottom", m_autoScrollToBottom->isChecked());

	settings->setValue("LogVisualization.FilterActive.Detailed", m_msgTypeFilterDetailed->isChecked());
	settings->setValue("LogVisualization.FilterActive.Info", m_msgTypeFilterInfo->isChecked());
	settings->setValue("LogVisualization.FilterActive.Warning", m_msgTypeFilterWarning->isChecked());
	settings->setValue("LogVisualization.FilterActive.Error", m_msgTypeFilterError->isChecked());
	settings->setValue("LogVisualization.FilterActive.Message.In", m_msgTypeFilterMsgIn->isChecked());
	settings->setValue("LogVisualization.FilterActive.Message.Out", m_msgTypeFilterMsgOut->isChecked());

	QJsonArray serviceFilterArr;
	for (int i = 0; i < m_serviceFilter->count(); i++) {
		QJsonObject serviceObj;
		serviceObj["Name"] = m_serviceFilter->item(i)->text();
		serviceObj["Active"] = (m_serviceFilter->item(i)->checkState() == Qt::Checked);
		serviceFilterArr.push_back(serviceObj);
	}
	QJsonDocument serviceFilterDoc(serviceFilterArr);
	settings->setValue("LogVisualization.ServiceFilter.List", QVariant(serviceFilterDoc.toJson(QJsonDocument::Compact)));

	QString tableColumnWidths;
	for (int i = 0; i < m_table->columnCount(); i++) {
		tableColumnWidths.append(QString::number(m_table->columnWidth(i)) + ";");
	}
	settings->setValue("LogVisualization.Table.ColumnWidth", tableColumnWidths);
}

QString LogVisualization::toolName(void) const {
	return QString("Log Visualization");
}

QWidget* LogVisualization::runTool(QMenu* _rootMenu, std::list<QWidget*>& _statusWidgets, QSettings& _settings) {
	LOGVIS_LOG("Initializing LogVisualization...");

	// Create layouts
	QWidget* centralLayoutW = new QWidget;
	QVBoxLayout* centralLayout = new QVBoxLayout(centralLayoutW);

	QHBoxLayout* filterLayout = new QHBoxLayout;
	QGroupBox*   filterByMessageTypeBox = new QGroupBox("Type Filter");
	QVBoxLayout* filterByMessageTypeLayout = new QVBoxLayout(filterByMessageTypeBox);
	QGroupBox*   filterByServiceBox = new QGroupBox("Service Filter");
	QVBoxLayout* filterByServiceLayout = new QVBoxLayout(filterByServiceBox);
	QHBoxLayout* buttonLayout = new QHBoxLayout;

	// Create controls
	QSplitter* splitter = new QSplitter;
	splitter->setOrientation(Qt::Orientation::Vertical);

	QLabel* messageFilterL = new QLabel("Message contains:");
	m_messageFilter = new QLineEdit;
	m_messageFilter->setPlaceholderText("<Confirm by pressing return>");
	m_messageFilter->setMinimumWidth(250);

	m_ignoreNewMessages = new QCheckBox("Ignore new messages");
	m_autoScrollToBottom = new QCheckBox("Auto scroll to bottom");

	QPushButton* btnClear = new QPushButton("Clear");
	btnClear->setToolTip("Will clear the Log Messages");
	this->connect(btnClear, &QPushButton::clicked, this, &LogVisualization::slotClear);
	
	QPushButton* btnClearAll = new QPushButton("Clear All");
	btnClearAll->setToolTip("Will clear the Log Messages and the Service Filter list");
	this->connect(btnClearAll, &QPushButton::clicked, this, &LogVisualization::slotClearAll);

	m_table = new QTableWidget(0, tCount);

	m_msgTypeFilterDetailed = new QCheckBox("Detailed");
	m_msgTypeFilterInfo = new QCheckBox("Info");
	m_msgTypeFilterWarning = new QCheckBox("Warning");
	m_msgTypeFilterError = new QCheckBox("Error");
	m_msgTypeFilterMsgIn = new QCheckBox("Inbound Message");
	m_msgTypeFilterMsgOut = new QCheckBox("Outgoing Message");

	m_serviceFilter = new QListWidget;
	QPushButton* btnSelectAllServices = new QPushButton("Select All");
	this->connect(btnSelectAllServices, &QPushButton::clicked, this, &LogVisualization::slotSelectAllServices);
	
	QPushButton* btnDeselectAllServices = new QPushButton("Deselect All");
	this->connect(btnDeselectAllServices, &QPushButton::clicked, this, &LogVisualization::slotDeselectAllServices);

	m_messageCountLabel = new QLabel;
	m_errorCountLabel = new QLabel;
	m_warningCountLabel = new QLabel;

	_statusWidgets.push_back(m_errorCountLabel);
	_statusWidgets.push_back(m_warningCountLabel);
	_statusWidgets.push_back(m_messageCountLabel);

	// Setup layouts
	centralLayout->addLayout(filterLayout);
	centralLayout->addLayout(buttonLayout);

	filterLayout->addWidget(filterByMessageTypeBox, 0);
	filterLayout->addWidget(filterByServiceBox, 1);

	filterByMessageTypeLayout->addWidget(m_msgTypeFilterDetailed);
	filterByMessageTypeLayout->addWidget(m_msgTypeFilterInfo);
	filterByMessageTypeLayout->addWidget(m_msgTypeFilterWarning);
	filterByMessageTypeLayout->addWidget(m_msgTypeFilterError);
	filterByMessageTypeLayout->addWidget(m_msgTypeFilterMsgIn);
	filterByMessageTypeLayout->addWidget(m_msgTypeFilterMsgOut);

	filterByServiceLayout->addWidget(m_serviceFilter);
	filterByServiceLayout->addWidget(btnSelectAllServices);
	filterByServiceLayout->addWidget(btnDeselectAllServices);

	buttonLayout->addWidget(messageFilterL);
	buttonLayout->addWidget(m_messageFilter);
	buttonLayout->addStretch(1);
	buttonLayout->addWidget(m_ignoreNewMessages, 0);
	buttonLayout->addWidget(m_autoScrollToBottom, 0);
	buttonLayout->addWidget(btnClear, 0);
	buttonLayout->addWidget(btnClearAll, 0);

	splitter->addWidget(centralLayoutW);
	splitter->addWidget(m_table);

	// Setup controls
	m_msgTypeFilterDetailed->setChecked(true);
	m_msgTypeFilterInfo->setChecked(true);
	m_msgTypeFilterWarning->setChecked(true);
	m_msgTypeFilterError->setChecked(true);

	m_table->setHorizontalHeaderItem(tIcon, new QTableWidgetItem());
	m_table->setHorizontalHeaderItem(tTimeGlobal, new QTableWidgetItem("Global Time"));
	m_table->setHorizontalHeaderItem(tTimeLocal, new QTableWidgetItem("Local Time"));
	m_table->setHorizontalHeaderItem(tService, new QTableWidgetItem("Service"));
	m_table->setHorizontalHeaderItem(tType, new QTableWidgetItem("Type"));
	m_table->setHorizontalHeaderItem(tFunction, new QTableWidgetItem("Function"));
	m_table->setHorizontalHeaderItem(tMessage, new QTableWidgetItem("Message"));

	m_table->setAlternatingRowColors(true);
	m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
	m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
	m_table->setSelectionMode(QAbstractItemView::SingleSelection);

	splitter->setStretchFactor(1, 1);

	// Restore settings
	QString tableColumnWidths = _settings.value("LogVisualization.Table.ColumnWidth", "").toString();
	QStringList tableColumnWidthsList = tableColumnWidths.split(";", QString::SkipEmptyParts);
	if (tableColumnWidthsList.count() == m_table->columnCount()) {
		int column = 0;
		for (auto w : tableColumnWidthsList) {
			m_table->setColumnWidth(column++, w.toInt());
		}
	}

	m_autoScrollToBottom->setChecked(_settings.value("LogVisualization.AutoScrollToBottom", true).toBool());

	m_msgTypeFilterDetailed->setChecked(_settings.value("LogVisualization.FilterActive.Detailed", true).toBool());
	m_msgTypeFilterInfo->setChecked(_settings.value("LogVisualization.FilterActive.Info", true).toBool());
	m_msgTypeFilterWarning->setChecked(_settings.value("LogVisualization.FilterActive.Warning", true).toBool());
	m_msgTypeFilterError->setChecked(_settings.value("LogVisualization.FilterActive.Error", true).toBool());
	m_msgTypeFilterMsgIn->setChecked(_settings.value("LogVisualization.FilterActive.Message.In", false).toBool());
	m_msgTypeFilterMsgOut->setChecked(_settings.value("LogVisualization.FilterActive.Message.Out", false).toBool());

	QByteArray serviceFilter = _settings.value("LogVisualization.ServiceFilter.List", QByteArray()).toByteArray();
	if (!serviceFilter.isEmpty()) {
		QJsonDocument serviceFilterDoc = QJsonDocument::fromJson(serviceFilter);
		if (serviceFilterDoc.isArray()) {
			QJsonArray serviceFilterArr = serviceFilterDoc.array();

			for (int i = 0; i < serviceFilterArr.count(); i++) {
				if (serviceFilterArr[i].isObject()) {
					QJsonObject serviceObj = serviceFilterArr[i].toObject();

					if (serviceObj.contains("Name") && serviceObj.contains("Active")) {
						if (serviceObj["Name"].isString() && serviceObj["Active"].isBool()) {
							QListWidgetItem* nItm = new QListWidgetItem(serviceObj["Name"].toString());
							nItm->setFlags(nItm->flags() | Qt::ItemIsUserCheckable);
							nItm->setCheckState((serviceObj["Active"].toBool() ? Qt::Checked : Qt::Unchecked));
							m_serviceFilter->addItem(nItm);
						}
					}
				}
			}
			m_serviceFilter->sortItems(Qt::SortOrder::AscendingOrder);
		}
	}

	// Initialize colors and text
	this->slotUpdateCheckboxColors();
	this->updateCountLabels();

	// Connect checkbox color signals
	connect(m_msgTypeFilterDetailed, &QCheckBox::stateChanged, this, &LogVisualization::slotUpdateCheckboxColors);
	connect(m_msgTypeFilterInfo, &QCheckBox::stateChanged, this, &LogVisualization::slotUpdateCheckboxColors);
	connect(m_msgTypeFilterWarning, &QCheckBox::stateChanged, this, &LogVisualization::slotUpdateCheckboxColors);
	connect(m_msgTypeFilterError, &QCheckBox::stateChanged, this, &LogVisualization::slotUpdateCheckboxColors);
	connect(m_msgTypeFilterMsgIn, &QCheckBox::stateChanged, this, &LogVisualization::slotUpdateCheckboxColors);
	connect(m_msgTypeFilterMsgOut, &QCheckBox::stateChanged, this, &LogVisualization::slotUpdateCheckboxColors);
	connect(m_ignoreNewMessages, &QCheckBox::stateChanged, this, &LogVisualization::slotUpdateCheckboxColors);
	connect(m_autoScrollToBottom, &QCheckBox::stateChanged, this, &LogVisualization::slotUpdateCheckboxColors);

	// Connect signals
	connect(m_messageFilter, &QLineEdit::returnPressed, this, &LogVisualization::slotFilterChanged);

	connect(m_msgTypeFilterDetailed, &QCheckBox::stateChanged, this, &LogVisualization::slotFilterChanged);
	connect(m_msgTypeFilterInfo, &QCheckBox::stateChanged, this, &LogVisualization::slotFilterChanged);
	connect(m_msgTypeFilterWarning, &QCheckBox::stateChanged, this, &LogVisualization::slotFilterChanged);
	connect(m_msgTypeFilterError, &QCheckBox::stateChanged, this, &LogVisualization::slotFilterChanged);
	connect(m_msgTypeFilterMsgIn, &QCheckBox::stateChanged, this, &LogVisualization::slotFilterChanged);
	connect(m_msgTypeFilterMsgOut, &QCheckBox::stateChanged, this, &LogVisualization::slotFilterChanged);


	connect(m_serviceFilter, &QListWidget::itemChanged, this, &LogVisualization::slotFilterChanged);

	connect(m_autoScrollToBottom, &QCheckBox::stateChanged, this, &LogVisualization::slotAutoScrollToBottomChanged);

	connect(m_table, &QTableWidget::itemDoubleClicked, this, &LogVisualization::slotViewCellContent);

	// Create menu bar
	m_connectButton = _rootMenu->addAction(QIcon(":/images/Disconnected.png"), "Connect");
	m_autoConnect = _rootMenu->addAction("Auto-Connect");
	bool needAutoConnect = _settings.value("LogVisualization.AutoConnect", false).toBool();
	m_autoConnect->setIcon(needAutoConnect ? QIcon(":/images/True.png") : QIcon(":/images/False.png"));

	_rootMenu->addSeparator();
	m_importButton = _rootMenu->addAction(QIcon(":images/Import.png"), "Import");
	m_exportButton = _rootMenu->addAction(QIcon(":images/Export.png"), "Export");

	connect(m_connectButton, &QAction::triggered, this, &LogVisualization::slotConnect);
	connect(m_autoConnect, &QAction::triggered, this, &LogVisualization::slotToggleAutoConnect);
	connect(m_importButton, &QAction::triggered, this, &LogVisualization::slotImport);
	connect(m_exportButton, &QAction::triggered, this, &LogVisualization::slotExport);

	if (needAutoConnect) {
		LOGVIS_LOG("Auto connect requested, request queued");
		QMetaObject::invokeMethod(this, &LogVisualization::slotAutoConnect, Qt::QueuedConnection);
	}

	LOGVIS_LOG("Initialization completed");

	return splitter;
}

//QList<QWidget *> LogVisualization::statusBarWidgets(void) const {
//	QList<QWidget *> ret;
//	
//	ret.push_back(m_errorCountLabel);
//	ret.push_back(m_warningCountLabel);
//	ret.push_back(m_messageCountLabel);
//
//	return ret;
//}

bool LogVisualization::prepareToolShutdown(QSettings& _settings) {
	_settings.setValue("LogVisualization.AutoScrollToBottom", m_autoScrollToBottom->isChecked());

	_settings.setValue("LogVisualization.FilterActive.Detailed", m_msgTypeFilterDetailed->isChecked());
	_settings.setValue("LogVisualization.FilterActive.Info", m_msgTypeFilterInfo->isChecked());
	_settings.setValue("LogVisualization.FilterActive.Warning", m_msgTypeFilterWarning->isChecked());
	_settings.setValue("LogVisualization.FilterActive.Error", m_msgTypeFilterError->isChecked());
	_settings.setValue("LogVisualization.FilterActive.Message.In", m_msgTypeFilterMsgIn->isChecked());
	_settings.setValue("LogVisualization.FilterActive.Message.Out", m_msgTypeFilterMsgOut->isChecked());

	QJsonArray serviceFilterArr;
	for (int i = 0; i < m_serviceFilter->count(); i++) {
		QJsonObject serviceObj;
		serviceObj["Name"] = m_serviceFilter->item(i)->text();
		serviceObj["Active"] = (m_serviceFilter->item(i)->checkState() == Qt::Checked);
		serviceFilterArr.push_back(serviceObj);
	}
	QJsonDocument serviceFilterDoc(serviceFilterArr);
	_settings.setValue("LogVisualization.ServiceFilter.List", QVariant(serviceFilterDoc.toJson(QJsonDocument::Compact)));

	QString tableColumnWidths;
	for (int i = 0; i < m_table->columnCount(); i++) {
		tableColumnWidths.append(QString::number(m_table->columnWidth(i)) + ";");
	}
	_settings.setValue("LogVisualization.Table.ColumnWidth", tableColumnWidths);


	if (this->disconnectFromLogger()) {
		return true;
	}
	else {
		LOGVIS_LOGE("Failed to send deregistration request to LoggerService \"" + QString::fromStdString(m_loggerUrl) + "\"");
		return false;
	}
}

void LogVisualization::slotConnect(void) {
	this->connectToLogger(false);
}

void LogVisualization::slotAutoConnect(void) {
	this->connectToLogger(true);
}

void LogVisualization::slotImport(void) {
	otoolkit::SettingsRef settings = AppBase::instance()->createSettingsInstance();
	QString fn = QFileDialog::getOpenFileName(m_table, "Import Log Messages", settings->value("LogVisualization.LastExportedFile", "").toString(), "OpenTwin Log (*.otlog.json)");
	if (fn.isEmpty()) return;

	QFile f(fn);
	if (!f.open(QIODevice::ReadOnly)) {
		LOGVIS_LOGE("Failed to open file for reading. File: \"" + fn + "\"");
		return;
	}

	OT_rJSON_parseDOC(doc, f.readAll().toStdString().c_str());
	f.close();

	if (!doc.IsArray()) {
		LOGVIS_LOGE("Import failed: Document is not an array");
	}

	QList<ot::LogMessage> msg;

	for (rapidjson::SizeType i = 0; i < doc.Size(); i++) {
		try {
			if (!doc[i].IsObject()) {
				throw std::exception("Array entry is not an object");
			}
			OT_rJSON_val obj = doc[i].GetObject();
			ot::LogMessage m;
			m.setFromJsonObject(obj);
			msg.push_back(m);
		}
		catch (const std::exception& _e) {
			LOGVIS_LOGE(_e.what());
		}
		catch (...) {
			LOGVIS_LOGE("[FATAL] Unknown error");
		}
	}

	if (msg.empty()) {
		LOGVIS_LOGW("No data imported");
		return;
	}

	this->slotClear();
	this->appendLogMessages(msg);

	settings->setValue("LogVisualization.LastExportedFile", fn);
	LOGVIS_LOG("Log Messages successfully import from file \"" + fn + "\"");
}

void LogVisualization::slotExport(void) {
	if (m_messages.empty()) {
		LOGVIS_LOGW("No messages to export");
		return;
	}

	otoolkit::SettingsRef settings = AppBase::instance()->createSettingsInstance();
	QString fn = QFileDialog::getSaveFileName(m_table, "Export Log Messages", settings->value("LogVisualization.LastExportedFile", "").toString(), "OpenTwin Log (*.otlog.json)");
	if (fn.isEmpty()) return;
	
	QFile f(fn);
	if (!f.open(QIODevice::WriteOnly)) {
		LOGVIS_LOGE("Failed to open file for writing. File: \"" + fn + "\"");
		return;
	}

	OT_rJSON_doc doc;
	doc.SetArray();

	for (auto m : m_messages) {
		OT_rJSON_createValueObject(msgObj);
		m.addToJsonObject(doc, msgObj);
		doc.PushBack(msgObj, doc.GetAllocator());
	}

	f.write(QByteArray::fromStdString(ot::rJSON::toJSON(doc)));
	f.close();

	settings->setValue("LogVisualization.LastExportedFile", fn);
	LOGVIS_LOG("Log Messages successfully exported to file \"" + fn + "\"");
}

void LogVisualization::slotClear(void) {
	m_table->setRowCount(0);
	m_messages.clear();
	m_errorCount = 0;
	m_warningCount = 0;
	this->updateCountLabels();
}

void LogVisualization::slotClearAll(void) {
	this->slotClear();
	m_serviceFilter->clear();
	LOGVIS_LOG("LogVisualization Clear All completed");
}

void LogVisualization::slotFilterChanged(void) {
	if (m_filterLock) return;
	m_filterLock = true;
	for (int i = 0; i < m_table->rowCount(); i++) checkEntryFilter(i);
	m_filterLock = false;
}

void LogVisualization::slotSelectAllServices(void) {
	m_filterLock = true;
	for (int i = 0; i < m_serviceFilter->count(); i++) m_serviceFilter->item(i)->setCheckState(Qt::Checked);
	m_filterLock = false;
	this->slotFilterChanged();
}

void LogVisualization::slotDeselectAllServices(void) {
	m_filterLock = true;
	for (int i = 0; i < m_serviceFilter->count(); i++) m_serviceFilter->item(i)->setCheckState(Qt::Unchecked);
	m_filterLock = false;
	this->slotFilterChanged();
}

void LogVisualization::slotAutoScrollToBottomChanged(void) {
	if (m_autoScrollToBottom->isChecked()) m_table->scrollToBottom();
}

void LogVisualization::slotUpdateCheckboxColors(void) {
	QString red("QCheckBox { color: #c02020; }");
	QString green("QCheckBox { color: #20c020; }");
	QString def("");

	m_msgTypeFilterDetailed->setStyleSheet(m_msgTypeFilterDetailed->isChecked() ? green : red);
	m_msgTypeFilterInfo->setStyleSheet(m_msgTypeFilterInfo->isChecked() ? green : red);
	m_msgTypeFilterWarning->setStyleSheet(m_msgTypeFilterWarning->isChecked() ? green : red);
	m_msgTypeFilterError->setStyleSheet(m_msgTypeFilterError->isChecked() ? green : red);
	m_msgTypeFilterMsgIn->setStyleSheet(m_msgTypeFilterMsgIn->isChecked() ? green : red);
	m_msgTypeFilterMsgOut->setStyleSheet(m_msgTypeFilterMsgOut->isChecked() ? green : red);

	m_ignoreNewMessages->setStyleSheet(m_ignoreNewMessages->isChecked() ? red : def);
	m_autoScrollToBottom->setStyleSheet(m_autoScrollToBottom->isChecked() ? red : def);
}

void LogVisualization::slotToggleAutoConnect(void) {
	otoolkit::SettingsRef settings = AppBase::instance()->createSettingsInstance();
	bool is = !settings->value("LogVisualization.AutoConnect", false).toBool();
	m_autoConnect->setIcon(is ? QIcon(":/images/True.png") : QIcon(":/images/False.png"));
	settings->setValue("LogVisualization.AutoConnect", is);
}

void LogVisualization::slotViewCellContent(QTableWidgetItem* _itm) {
	size_t i = 0;
	for (auto itm : m_messages) {
		if (i++ == _itm->row()) {
			LogVisualizationItemViewDialog dia(itm, i, m_table);
			dia.exec();
			return;
		}
	}
}

void LogVisualization::appendLogMessage(const ot::LogMessage& _msg) {
	if (m_ignoreNewMessages->isChecked()) { return; };

	m_filterLock = true;

	int r = m_table->rowCount();
	m_table->insertRow(r);

	QString serviceName = QString::fromStdString(_msg.serviceName());

	QTableWidgetItem * iconItm = new QTableWidgetItem("");
	if (_msg.flags() & ot::WARNING_LOG) {
		iconItm->setIcon(QIcon(":/images/Warning.png"));
		m_warningCount++;
	}
	else if (_msg.flags() & ot::ERROR_LOG) {
		iconItm->setIcon(QIcon(":/images/Error.png"));
		m_errorCount++;
	}
	else {
		iconItm->setIcon(QIcon(":/images/Info.png"));
	}
	this->iniTableItem(r, tIcon, iconItm);

	
	this->iniTableItem(r, tType, new QTableWidgetItem(logMessageTypeString(_msg)));
	this->iniTableItem(r, tTimeGlobal, new QTableWidgetItem(QString::fromStdString(_msg.globalSystemTime())));
	this->iniTableItem(r, tTimeLocal, new QTableWidgetItem(QString::fromStdString(_msg.localSystemTime())));
	this->iniTableItem(r, tService, new QTableWidgetItem(serviceName));
	this->iniTableItem(r, tFunction, new QTableWidgetItem(QString::fromStdString(_msg.functionName())));
	this->iniTableItem(r, tMessage, new QTableWidgetItem(QString::fromStdString(_msg.text())));

	// Store message
	m_messages.push_back(_msg);

	// If required scroll to the last entry in the table
	if (m_autoScrollToBottom->isChecked()) m_table->scrollToBottom();

	// Check if the service name is in the service name list
	bool found = false;
	for (int i = 0; i < m_serviceFilter->count(); i++) {
		if (m_serviceFilter->item(i)->text() == serviceName) {
			found = true;
			break;
		}
	}

	if (!found) {
		QListWidgetItem * itm = new QListWidgetItem(serviceName);
		itm->setFlags(itm->flags() | Qt::ItemIsUserCheckable);
		itm->setCheckState(Qt::Checked);
		m_serviceFilter->addItem(itm);
		m_serviceFilter->sortItems(Qt::SortOrder::AscendingOrder);
	}

	m_filterLock = false;

	// Update status labels (warnings and errors were increased while checking the typeString)
	this->updateCountLabels();

	// Check the filter for the new entry
	this->checkEntryFilter(r);
}

void LogVisualization::appendLogMessages(const QList<ot::LogMessage>& _messages) {
	for (auto m : _messages) {
		appendLogMessage(m); 
	}
}

QString LogVisualization::logMessageTypeString(const ot::LogMessage& _msg) {
	QString typeString;
	if (_msg.flags() & ot::DETAILED_LOG) {
		if (!typeString.isEmpty()) typeString.append(" | ");
		typeString.append(TABLE_TXT_DETAILED);
	}
	if (_msg.flags() & ot::INFORMATION_LOG) {
		if (!typeString.isEmpty()) typeString.append(" | ");
		typeString.append(TABLE_TXT_INFO);
	}
	if (_msg.flags() & ot::ERROR_LOG) {
		if (!typeString.isEmpty()) typeString.append(" | ");
		typeString.append(TABLE_TXT_ERROR);
	}
	if (_msg.flags() & ot::WARNING_LOG) {
		if (!typeString.isEmpty()) typeString.append(" | ");
		typeString.append(TABLE_TXT_WARNING);
	}
	if (_msg.flags() & ot::INBOUND_MESSAGE_LOG) {
		if (!typeString.isEmpty()) typeString.append(" | ");
		typeString.append(TABLE_TXT_MSG_IN);
	}
	if (_msg.flags() & ot::QUEUED_INBOUND_MESSAGE_LOG) {
		if (!typeString.isEmpty()) typeString.append(" | ");
		typeString.append(TABLE_TXT_MSG_QUEUE);
	}
	if (_msg.flags() & ot::ONEWAY_TLS_INBOUND_MESSAGE_LOG) {
		if (!typeString.isEmpty()) typeString.append(" | ");
		typeString.append(TABLE_TXT_MSG_OW_TLS);
	}
	if (_msg.flags() & ot::OUTGOING_MESSAGE_LOG) {
		if (!typeString.isEmpty()) typeString.append(" | ");
		typeString.append(TABLE_TXT_MSG_OUT);
	}

	return typeString;
}

void LogVisualization::iniTableItem(int _row, int _column, QTableWidgetItem * _itm) {
	//_itm->setFlags(_itm->flags() & (~Qt::ItemIsEditable) & (~Qt::ItemIsSelectable));
	m_table->setItem(_row, _column, _itm);
}

void LogVisualization::checkEntryFilter(int _row) {
	// Type

	bool typePass = false;
	QTableWidgetItem * typeItm = m_table->item(_row, tType);
	if (m_msgTypeFilterDetailed->isChecked() && typeItm->text().contains(TABLE_TXT_DETAILED)) typePass = true;
	else if (m_msgTypeFilterInfo->isChecked() && typeItm->text().contains(TABLE_TXT_INFO)) typePass = true;
	else if (m_msgTypeFilterWarning->isChecked() && typeItm->text().contains(TABLE_TXT_WARNING)) typePass = true;
	else if (m_msgTypeFilterError->isChecked() && typeItm->text().contains(TABLE_TXT_ERROR)) typePass = true;
	else if (m_msgTypeFilterMsgIn->isChecked() && typeItm->text().contains(TABLE_TXT_MSG_IN)) typePass = true;
	else if (m_msgTypeFilterMsgIn->isChecked() && typeItm->text().contains(TABLE_TXT_MSG_QUEUE)) typePass = true;
	else if (m_msgTypeFilterMsgIn->isChecked() && typeItm->text().contains(TABLE_TXT_MSG_OW_TLS)) typePass = true;
	else if (m_msgTypeFilterMsgOut->isChecked() && typeItm->text().contains(TABLE_TXT_MSG_OUT)) typePass = true;

	// Service

	bool servicePass = false;
	QString serviceName = m_table->item(_row, tService)->text();
	for (int i = 0; i < m_serviceFilter->count(); i++) {
		if (m_serviceFilter->item(i)->checkState() == Qt::Checked && m_serviceFilter->item(i)->text() == serviceName) {
			servicePass = true;
			break;
		}
	}

	// Message
	bool messagePass = true;
	if (!m_messageFilter->text().isEmpty()) {
		messagePass = m_table->item(_row, tMessage)->text().contains(m_messageFilter->text(), Qt::CaseInsensitive);
	}

	m_table->setRowHidden(_row, !(typePass && servicePass && messagePass));
}

void LogVisualization::updateCountLabels(void) {
	//m_errorCountLabel->setHidden(m_errorCount == 0);
	//m_warningCountLabel->setHidden(m_warningCount == 0);

	m_messageCountLabel->setText("Log count: " + QString::number(m_messages.size()));
	m_warningCountLabel->setText("Warnings: " + QString::number(m_warningCount));
	m_errorCountLabel->setText("Errors: " + QString::number(m_errorCount));
}

void LogVisualization::connectToLogger(bool _isAutoConnect) {
	if (!m_loggerUrl.empty()) {
		this->disconnectFromLogger();
		return;
	}

	ConnectToLoggerDialog dia(_isAutoConnect);
	dia.exec();

	if (!dia.success()) {
		return;
	}
	
	m_loggerUrl = dia.loggerServiceUrl().toStdString();

	LOGVIS_LOG("Successfully connected to LoggerService. Refreshing log messages...");

	m_connectButton->setText("Disconnect");
	m_connectButton->setIcon(QIcon(":/images/Connected.png"));
	this->slotClear();
	this->appendLogMessages(dia.messageBuffer());
	
	LOGVIS_LOG("Done.");
}

bool LogVisualization::disconnectFromLogger(void) {
	if (m_loggerUrl.empty()) return true;

	std::string response;
	OT_rJSON_createDOC(doc);
	ot::rJSON::add(doc, OT_ACTION_MEMBER, OT_ACTION_CMD_RemoveService);
	ot::rJSON::add(doc, OT_ACTION_PARAM_SERVICE_URL, AppBase::instance()->url().toStdString());

	if (!ot::msg::send("", m_loggerUrl, ot::EXECUTE, ot::rJSON::toJSON(doc), response)) {
		LOGVIS_LOGE("Failed to send remove service request to logger service.");
		return false;
	}

	LOGVIS_LOG("Successfully deregistered at LoggerService \"" + QString::fromStdString(m_loggerUrl) + "\"");
	m_loggerUrl.clear();

	m_connectButton->setText("Connect");
	m_connectButton->setIcon(QIcon(":/images/Disconnected.png"));
	
	return true;
}

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

LogVisualizationItemViewDialog::LogVisualizationItemViewDialog(const ot::LogMessage& _msg, size_t _index, QWidget* _parent) 
	: QDialog(_parent), m_msg(_msg)
{
	// Create layouts
	m_centralLayout = new QVBoxLayout;
	m_dataLayout = new QGridLayout;
	m_bigVLayout = new QVBoxLayout;
	m_messageTitleLayout = new QHBoxLayout;
	m_buttonLayout = new QHBoxLayout;

	// Create controls
	m_timeL = new QLabel("Time (Global):");
	m_time = new QLineEdit(QString::fromStdString(m_msg.globalSystemTime()));
	m_time->setReadOnly(true);

	m_timeLocalL = new QLabel("Time (Local):");
	m_timeLocal = new QLineEdit(QString::fromStdString(m_msg.localSystemTime()));
	m_timeLocal->setReadOnly(true);

	m_senderNameL = new QLabel("Sender:");
	m_senderName = new QLineEdit(QString::fromStdString(m_msg.serviceName()));
	m_senderName->setReadOnly(true);

	m_messageTypeL = new QLabel("Type:");
	m_messageType = new QLineEdit(LogVisualization::logMessageTypeString(m_msg));
	m_messageType->setReadOnly(true);

	m_functionL = new QLabel("Function:");
	m_function = new QLineEdit(QString::fromStdString(m_msg.functionName()));
	m_function->setReadOnly(true);

	m_findMessageSyntax = new QCheckBox("Syntax check");
	m_findMessageSyntax->setToolTip("If active, a syntax check on the message will be performed.\n"
		"If for example a JSON document was found inside the message, the json document will be displayed indented.");


	m_messageL = new QLabel("Message text:");
	m_message = new QPlainTextEdit(QString::fromStdString(m_msg.text()));
	m_message->setReadOnly(true);

	m_okButton = new QPushButton("Ok");
	m_okButton->setMinimumWidth(100);
	this->connect(m_okButton, &QPushButton::clicked, this, &LogVisualizationItemViewDialog::close);

	// Create shortcuts
	m_closeShortcut = new QShortcut(QKeySequence("Esc"), this);
	this->connect(m_closeShortcut, &QShortcut::activated, this, &LogVisualizationItemViewDialog::close);

	m_recenterShortcut = new QShortcut(QKeySequence("F11"), this);
	this->connect(m_recenterShortcut, &QShortcut::activated, this, &LogVisualizationItemViewDialog::slotRecenter);

	// Setup layouts
	this->setLayout(m_centralLayout);
	m_centralLayout->addLayout(m_dataLayout, 0);
	m_centralLayout->addLayout(m_bigVLayout, 1);
	m_centralLayout->addLayout(m_buttonLayout, 0);

	m_dataLayout->addWidget(m_timeL, 0, 0);
	m_dataLayout->addWidget(m_time, 0, 1);
	m_dataLayout->addWidget(m_timeLocalL, 1, 0);
	m_dataLayout->addWidget(m_timeLocal, 1, 1);
	m_dataLayout->addWidget(m_senderNameL, 2, 0);
	m_dataLayout->addWidget(m_senderName, 2, 1);
	m_dataLayout->addWidget(m_messageTypeL, 3, 0);
	m_dataLayout->addWidget(m_messageType, 3, 1);
	m_dataLayout->addWidget(m_findMessageSyntax, 4, 1);

	m_messageTitleLayout->addWidget(m_messageL, 0);
	m_messageTitleLayout->addStretch(1);
	m_messageTitleLayout->addWidget(m_findMessageSyntax, 0);

	m_bigVLayout->addWidget(m_functionL, 0);
	m_bigVLayout->addWidget(m_function, 0);
	m_bigVLayout->addLayout(m_messageTitleLayout, 0);
	m_bigVLayout->addWidget(m_message, 1);

	m_buttonLayout->addStretch(1);
	m_buttonLayout->addWidget(m_okButton);
	m_buttonLayout->addStretch(1);

	// Setup window
	this->setWindowTitle("MessageViewer (Message #" + QString::number(_index) + ") | OToolkit");
	this->setWindowFlag(Qt::WindowContextHelpButtonHint, false);
	this->setWindowIcon(AppBase::instance()->windowIcon());

	this->setMinimumSize(400, 300);
	this->setFocusPolicy(Qt::ClickFocus);
	
	otoolkit::SettingsRef settings = AppBase::instance()->createSettingsInstance();
	this->move(settings->value("LogVisualizationItemViewDialog.X", 0).toInt(), settings->value("LogVisualizationItemViewDialog.Y", 0).toInt());
	this->resize(settings->value("LogVisualizationItemViewDialog.W", 800).toInt(), settings->value("LogVisualizationItemViewDialog.H", 600).toInt());
	m_findMessageSyntax->setChecked(settings->value("LogVisualizationItemViewDialog.FindSyntax", true).toBool());

	this->slotDisplayMessageText((int)m_findMessageSyntax->checkState());

	// Connect signals
	this->connect(m_findMessageSyntax, &QCheckBox::stateChanged, this, &LogVisualizationItemViewDialog::slotDisplayMessageText);
}

LogVisualizationItemViewDialog::~LogVisualizationItemViewDialog() {
	
}

void LogVisualizationItemViewDialog::closeEvent(QCloseEvent* _event) {
	otoolkit::SettingsRef settings = AppBase::instance()->createSettingsInstance();
	settings->setValue("LogVisualizationItemViewDialog.X", pos().x());
	settings->setValue("LogVisualizationItemViewDialog.Y", pos().y());
	settings->setValue("LogVisualizationItemViewDialog.W", size().width());
	settings->setValue("LogVisualizationItemViewDialog.H", size().height());
	settings->setValue("LogVisualizationItemViewDialog.FindSyntax", m_findMessageSyntax->isChecked());
}

void LogVisualizationItemViewDialog::mousePressEvent(QMouseEvent* _event) {
	if (!geometry().contains(_event->globalPos())) {
		close();
	}
	else {
		QDialog::mousePressEvent(_event);
	}
}

bool LogVisualizationItemViewDialog::eventFilter(QObject* _obj, QEvent* _event)
{
	if (_event->type() == QEvent::MouseButtonPress)
	{
		QMessageBox m(QMessageBox::Warning, "A", "B");
		m.exec();
		QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(_event);
		if (!geometry().contains(mouseEvent->globalPos()))
		{
			close(); // Close the dialog
			return true; // Event handled
		}
		if (mouseEvent->globalPos().x() > (this->pos().x() + this->width())) {
			close(); // Close the dialog
			return true; // Event handled
		}
	}
	return QDialog::eventFilter(_obj, _event); // Pass the event to the base class
}

bool LogVisualizationItemViewDialog::event(QEvent* _event)
{
	return QDialog::event(_event); // Pass the event to the base class
}

void LogVisualizationItemViewDialog::slotRecenter(void) {
	this->move(0, 0);
	this->resize(800, 600);
}

void LogVisualizationItemViewDialog::slotDisplayMessageText(int _state) {
	QString str = QString::fromStdString(m_msg.text());
	if (m_findMessageSyntax->isChecked()) {
		// JSON check
		str = this->findJsonSyntax(str);

		// Display result string
		m_message->setPlainText(str);
	}
	else {
		m_message->setPlainText(str);
	}
}

QString LogVisualizationItemViewDialog::findJsonSyntax(const QString& _inputString) {
	//QRegularExpression regex("(.*?)(\\s*([\\{\\[])(?:[^{}\\[\\]]|(?3))*\\3\\s*)(.*$)");
	QRegularExpression regex("(.*?)(\\s*\\{.*\\}\\s*)(.*$)");
	QRegularExpressionMatch match = regex.match(_inputString);

	if (match.hasMatch()) {
		QString pre = match.captured(1);
		QString doc = match.captured(2);
		QString suf = match.captured(3);
		return (pre + "\n\n" + 
			QJsonDocument::fromJson(QByteArray::fromStdString(doc.toStdString())).toJson(QJsonDocument::Indented) +
			"\n\n" + suf);
	}
	else {
		return _inputString; // Return an empty string if no match is found
	}
}
