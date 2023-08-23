#include "LogVisualization.h"
#include "AppBase.h"
#include "ConnectToLoggerDialog.h"

// OT header
#include "OpenTwinCommunication/ActionTypes.h"
#include "OpenTwinCommunication/Msg.h"

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

LogVisualization::LogVisualization() : m_filterLock(false), m_warningCount(0), m_errorCount(0) {
	LOGVIS_LOG("Initializing LogVisualization...");

	// Create layouts
	m_centralLayoutW = new QWidget;
	m_centralLayout = new QVBoxLayout(m_centralLayoutW);

	m_filterLayout = new QHBoxLayout;
	m_filterByMessageTypeBox = new QGroupBox("Type Filter");
	m_filterByMessageTypeLayout = new QVBoxLayout(m_filterByMessageTypeBox);
	m_filterByServiceBox = new QGroupBox("Service Filter");
	m_filterByServiceLayout = new QVBoxLayout(m_filterByServiceBox);
	m_buttonLayout = new QHBoxLayout;

	// Create controls
	m_splitter = new QSplitter;
	m_splitter->setOrientation(Qt::Orientation::Vertical);

	m_messageFilterL = new QLabel("Message contains:");
	m_messageFilter = new QLineEdit;
	m_messageFilter->setPlaceholderText("<Confirm by pressing return>");
	m_messageFilter->setMinimumWidth(250);

	m_ignoreNewMessages = new QCheckBox("Ignore new messages");
	m_autoScrollToBottom = new QCheckBox("Auto scroll to bottom");
	m_btnClear = new QPushButton("Clear");
	m_btnClear->setToolTip("Will clear the Log Messages");

	m_btnClearAll = new QPushButton("Clear All");
	m_btnClearAll->setToolTip("Will clear the Log Messages and the Service Filter list");

	m_table = new QTableWidget(0, tCount);

	m_msgTypeFilterDetailed = new QCheckBox("Detailed");
	m_msgTypeFilterInfo = new QCheckBox("Info");
	m_msgTypeFilterWarning = new QCheckBox("Warning");
	m_msgTypeFilterError = new QCheckBox("Error");
	m_msgTypeFilterMsgIn = new QCheckBox("Inbound Message");
	m_msgTypeFilterMsgOut = new QCheckBox("Outgoing Message");

	m_serviceFilter = new QListWidget;
	m_btnSelectAllServices = new QPushButton("Select All");
	m_btnDeselectAllServices = new QPushButton("Deselect All");

	m_messageCountLabel = new QLabel;
	m_errorCountLabel = new QLabel;
	m_warningCountLabel = new QLabel;

	// Setup layouts
	m_centralLayout->addLayout(m_filterLayout);
	m_centralLayout->addLayout(m_buttonLayout);

	m_filterLayout->addWidget(m_filterByMessageTypeBox, 0);
	m_filterLayout->addWidget(m_filterByServiceBox, 1);

	m_filterByMessageTypeLayout->addWidget(m_msgTypeFilterDetailed);
	m_filterByMessageTypeLayout->addWidget(m_msgTypeFilterInfo);
	m_filterByMessageTypeLayout->addWidget(m_msgTypeFilterWarning);
	m_filterByMessageTypeLayout->addWidget(m_msgTypeFilterError);
	m_filterByMessageTypeLayout->addWidget(m_msgTypeFilterMsgIn);
	m_filterByMessageTypeLayout->addWidget(m_msgTypeFilterMsgOut);

	m_filterByServiceLayout->addWidget(m_serviceFilter);
	m_filterByServiceLayout->addWidget(m_btnSelectAllServices);
	m_filterByServiceLayout->addWidget(m_btnDeselectAllServices);

	m_buttonLayout->addWidget(m_messageFilterL);
	m_buttonLayout->addWidget(m_messageFilter);
	m_buttonLayout->addStretch(1);
	m_buttonLayout->addWidget(m_ignoreNewMessages, 0);
	m_buttonLayout->addWidget(m_autoScrollToBottom, 0);
	m_buttonLayout->addWidget(m_btnClear, 0);
	m_buttonLayout->addWidget(m_btnClearAll, 0);

	m_splitter->addWidget(m_centralLayoutW);
	m_splitter->addWidget(m_table);

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

	//m_table->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
	//m_table->horizontalHeader()->setSectionResizeMode(tMessage, QHeaderView::Stretch);

	//m_table->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

	m_table->setAlternatingRowColors(true);
	m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
	m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
	m_table->setSelectionMode(QAbstractItemView::SingleSelection);

	m_splitter->setStretchFactor(1, 1);

	// Restore settings
	QSettings s("OpenTwin", applicationName());
	
	QString tableColumnWidths = s.value("LogVisualization.Table.ColumnWidth", "").toString();
	QStringList tableColumnWidthsList = tableColumnWidths.split(";", QString::SkipEmptyParts);
	if (tableColumnWidthsList.count() == m_table->columnCount()) {
		int column = 0;
		for (auto w : tableColumnWidthsList) {
			m_table->setColumnWidth(column++, w.toInt());
		}
	}

	m_autoScrollToBottom->setChecked(s.value("LogVisualization.AutoScrollToBottom", true).toBool());

	m_msgTypeFilterDetailed->setChecked(s.value("LogVisualization.FilterActive.Detailed", true).toBool());
	m_msgTypeFilterInfo->setChecked(s.value("LogVisualization.FilterActive.Info", true).toBool());
	m_msgTypeFilterWarning->setChecked(s.value("LogVisualization.FilterActive.Warning", true).toBool());
	m_msgTypeFilterError->setChecked(s.value("LogVisualization.FilterActive.Error", true).toBool());
	m_msgTypeFilterMsgIn->setChecked(s.value("LogVisualization.FilterActive.Message.In", false).toBool());
	m_msgTypeFilterMsgOut->setChecked(s.value("LogVisualization.FilterActive.Message.Out", false).toBool());

	QByteArray serviceFilter = s.value("LogVisualization.ServiceFilter.List", QByteArray()).toByteArray();
	if (!serviceFilter.isEmpty()) {
		QJsonDocument serviceFilterDoc = QJsonDocument::fromJson(serviceFilter);
		if (serviceFilterDoc.isArray()) {
			QJsonArray serviceFilterArr = serviceFilterDoc.array();

			for (int i = 0; i < serviceFilterArr.count(); i++) {
				if (serviceFilterArr[i].isObject()) {
					QJsonObject serviceObj = serviceFilterArr[i].toObject();

					if (serviceObj.contains("Name") && serviceObj.contains("Active")) {
						if (serviceObj["Name"].isString() && serviceObj["Active"].isBool()) {
							QListWidgetItem * nItm = new QListWidgetItem(serviceObj["Name"].toString());
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
	m_centralLayoutW->setEnabled(false);
	slotUpdateCheckboxColors();
	updateCountLabels();
	
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
	connect(m_btnClear, &QPushButton::clicked, this, &LogVisualization::slotClear);
	connect(m_btnClearAll, &QPushButton::clicked, this, &LogVisualization::slotClearAll);

	connect(m_msgTypeFilterDetailed, &QCheckBox::stateChanged, this, &LogVisualization::slotFilterChanged);
	connect(m_msgTypeFilterInfo, &QCheckBox::stateChanged, this, &LogVisualization::slotFilterChanged);
	connect(m_msgTypeFilterWarning, &QCheckBox::stateChanged, this, &LogVisualization::slotFilterChanged);
	connect(m_msgTypeFilterError, &QCheckBox::stateChanged, this, &LogVisualization::slotFilterChanged);
	connect(m_msgTypeFilterMsgIn, &QCheckBox::stateChanged, this, &LogVisualization::slotFilterChanged);
	connect(m_msgTypeFilterMsgOut, &QCheckBox::stateChanged, this, &LogVisualization::slotFilterChanged);

	connect(m_btnSelectAllServices, &QPushButton::clicked, this, &LogVisualization::slotSelectAllServices);
	connect(m_btnDeselectAllServices, &QPushButton::clicked, this, &LogVisualization::slotDeselectAllServices);

	connect(m_serviceFilter, &QListWidget::itemChanged, this, &LogVisualization::slotFilterChanged);

	connect(m_autoScrollToBottom, &QCheckBox::stateChanged, this, &LogVisualization::slotAutoScrollToBottomChanged);

	connect(m_table, &QTableWidget::itemDoubleClicked, this, &LogVisualization::slotViewCellContent);

	LOGVIS_LOG("LogVisualization initialization completed.");
}

LogVisualization::~LogVisualization() {
	QSettings s("OpenTwin", applicationName());

	s.setValue("LogVisualization.AutoScrollToBottom", m_autoScrollToBottom->isChecked());

	s.setValue("LogVisualization.FilterActive.Detailed", m_msgTypeFilterDetailed->isChecked());
	s.setValue("LogVisualization.FilterActive.Info", m_msgTypeFilterInfo->isChecked());
	s.setValue("LogVisualization.FilterActive.Warning", m_msgTypeFilterWarning->isChecked());
	s.setValue("LogVisualization.FilterActive.Error", m_msgTypeFilterError->isChecked());
	s.setValue("LogVisualization.FilterActive.Message.In", m_msgTypeFilterMsgIn->isChecked());
	s.setValue("LogVisualization.FilterActive.Message.Out", m_msgTypeFilterMsgOut->isChecked());

	QJsonArray serviceFilterArr;
	for (int i = 0; i < m_serviceFilter->count(); i++) {
		QJsonObject serviceObj;
		serviceObj["Name"] = m_serviceFilter->item(i)->text();
		serviceObj["Active"] = (m_serviceFilter->item(i)->checkState() == Qt::Checked);
		serviceFilterArr.push_back(serviceObj);
	}
	QJsonDocument serviceFilterDoc(serviceFilterArr);
	s.setValue("LogVisualization.ServiceFilter.List", QVariant(serviceFilterDoc.toJson(QJsonDocument::Compact)));

	QString tableColumnWidths;
	for (int i = 0; i < m_table->columnCount(); i++) {
		tableColumnWidths.append(QString::number(m_table->columnWidth(i)) + ";");
	}
	s.setValue("LogVisualization.Table.ColumnWidth", tableColumnWidths);
}

QString LogVisualization::toolName(void) const {
	return QString("Log Visualization");
}

QList<QWidget *> LogVisualization::statusBarWidgets(void) const {
	QList<QWidget *> ret;
	
	ret.push_back(m_errorCountLabel);
	ret.push_back(m_warningCountLabel);
	ret.push_back(m_messageCountLabel);

	return ret;
}

void LogVisualization::createMenuBarEntries(QMenuBar * _menuBar) {
	QSettings s("OpenTwin", applicationName());

	QMenu * topLvlMenu = _menuBar->addMenu("Log Visualization");
	m_connectButton = topLvlMenu->addAction(QIcon(":/images/Shutdown.png"), "Connect");
	m_autoConnect = topLvlMenu->addAction("Auto-Connect");
	m_autoConnect->setCheckable(true);
	m_autoConnect->setChecked(s.value("LogVisualization.AutoConnect", false).toBool());

	topLvlMenu->addSeparator();
	m_importButton = topLvlMenu->addAction(QIcon(":images/Import.png"), "Import");
	m_exportButton = topLvlMenu->addAction(QIcon(":images/Export.png"), "Export");
	m_importButton->setEnabled(false);
	m_exportButton->setEnabled(false);

	connect(m_connectButton, &QAction::triggered, this, &LogVisualization::slotConnect);
	connect(m_autoConnect, &QAction::triggered, this, &LogVisualization::slotToggleAutoConnect);
	connect(m_importButton, &QAction::triggered, this, &LogVisualization::slotImport);
	connect(m_exportButton, &QAction::triggered, this, &LogVisualization::slotExport);

	if (m_autoConnect->isChecked()) {
		LOGVIS_LOG("Auto connect requested, request queued");
		QMetaObject::invokeMethod(this, &LogVisualization::slotAutoConnect, Qt::QueuedConnection);
	}
}

void LogVisualization::slotConnect(void) {
	this->connectToLogger(false);
}

void LogVisualization::slotAutoConnect(void) {
	this->connectToLogger(true);
}

void LogVisualization::slotImport(void) {
	QSettings s("OpenTwin", APP_BASE_APP_NAME);
	QString fn = QFileDialog::getOpenFileName(widget(), "Import Log Messages", s.value("LogVisualization.LastExportedFile", "").toString(), "OpenTwin Log (*.otlog.json)");
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

	s.setValue("LogVisualization.LastExportedFile", fn);
	LOGVIS_LOG("Log Messages successfully import from file \"" + fn + "\"");
}

void LogVisualization::slotExport(void) {
	if (m_messages.empty()) {
		LOGVIS_LOGW("No messages to export");
		return;
	}

	QSettings s("OpenTwin", APP_BASE_APP_NAME);
	QString fn = QFileDialog::getSaveFileName(widget(), "Export Log Messages", s.value("LogVisualization.LastExportedFile", "").toString(), "OpenTwin Log (*.otlog.json)");
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

	s.setValue("LogVisualization.LastExportedFile", fn);
	LOGVIS_LOG("Log Messages successfully exported to file \"" + fn + "\"");
}

void LogVisualization::slotClear(void) {
	m_table->setRowCount(0);
	m_messages.clear();
	m_errorCount = 0;
	m_warningCount = 0;
	updateCountLabels();
}

void LogVisualization::slotClearAll(void) {
	slotClear();
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
	slotFilterChanged();
}

void LogVisualization::slotDeselectAllServices(void) {
	m_filterLock = true;
	for (int i = 0; i < m_serviceFilter->count(); i++) m_serviceFilter->item(i)->setCheckState(Qt::Unchecked);
	m_filterLock = false;
	slotFilterChanged();
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
	m_autoConnect->setChecked(!m_autoConnect->isChecked());

	QSettings s("OpenTwin", APP_BASE_APP_NAME);
	s.setValue("LogVisualization.AutoConnect", m_autoConnect->isChecked());
}

void LogVisualization::slotViewCellContent(QTableWidgetItem* _itm) {
	size_t i = 0;
	for (auto itm : m_messages) {
		if (i++ == _itm->row()) {
			LogVisualizationItemViewDialog dia(itm, i, widget());
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
	iniTableItem(r, tIcon, iconItm);

	
	iniTableItem(r, tType, new QTableWidgetItem(logMessageTypeString(_msg)));
	iniTableItem(r, tTimeGlobal, new QTableWidgetItem(QString::fromStdString(_msg.globalSystemTime())));
	iniTableItem(r, tTimeLocal, new QTableWidgetItem(QString::fromStdString(_msg.localSystemTime())));
	iniTableItem(r, tService, new QTableWidgetItem(serviceName));
	iniTableItem(r, tFunction, new QTableWidgetItem(QString::fromStdString(_msg.functionName())));
	iniTableItem(r, tMessage, new QTableWidgetItem(QString::fromStdString(_msg.text())));

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
	updateCountLabels();

	// Check the filter for the new entry
	checkEntryFilter(r);
}

void LogVisualization::appendLogMessages(const QList<ot::LogMessage>& _messages) {
	for (auto m : _messages) {
		appendLogMessage(m); 
	}
}

QWidget * LogVisualization::widget(void) {
	return m_splitter;
}

QString LogVisualization::logMessageTypeString(const ot::LogMessage& _msg) {
	QString typeString;
	if (_msg.flags() & ot::DETAILED_LOG) {
		if (!typeString.isEmpty()) typeString.append(" | ");
		typeString.append(TABLE_TXT_DETAILED);
	}
	if (_msg.flags() & ot::DEFAULT_LOG) {
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
	ConnectToLoggerDialog dia(_isAutoConnect);
	dia.exec();

	if (!dia.success()) {
		return;
	}
	
	LOGVIS_LOG("Successfully connected to LoggerService. Refreshing log messages...");

	m_centralLayoutW->setEnabled(true);
	m_connectButton->setEnabled(false);
	m_importButton->setEnabled(true);
	m_exportButton->setEnabled(true);

	this->slotClear();
	this->appendLogMessages(dia.messageBuffer());

	LOGVIS_LOG("Done.");
}

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

LogVisualizationItemViewDialog::LogVisualizationItemViewDialog(const ot::LogMessage& _msg, size_t _index, QWidget* _parent) : QDialog(_parent) {
	// Create layouts
	m_centralLayout = new QVBoxLayout;
	m_dataLayout = new QGridLayout;
	m_bigVLayout = new QVBoxLayout;
	m_buttonLayout = new QHBoxLayout;

	// Create controls
	m_timeL = new QLabel("Time (Global):");
	m_time = new QLineEdit(QString::fromStdString(_msg.globalSystemTime()));
	m_time->setReadOnly(true);

	m_timeLocalL = new QLabel("Time (Local):");
	m_timeLocal = new QLineEdit(QString::fromStdString(_msg.localSystemTime()));
	m_timeLocal->setReadOnly(true);

	m_senderNameL = new QLabel("Sender:");
	m_senderName = new QLineEdit(QString::fromStdString(_msg.serviceName()));
	m_senderName->setReadOnly(true);

	m_messageTypeL = new QLabel("Type:");
	m_messageType = new QLineEdit(LogVisualization::logMessageTypeString(_msg));
	m_messageType->setReadOnly(true);

	m_functionL = new QLabel("Function:");
	m_function = new QLineEdit(QString::fromStdString(_msg.functionName()));
	m_function->setReadOnly(true);

	m_messageL = new QLabel("Message text:");
	m_message = new QPlainTextEdit(QString::fromStdString(_msg.text()));
	m_message->setReadOnly(true);

	m_okButton = new QPushButton("Ok");
	m_okButton->setMinimumWidth(100);
	connect(m_okButton, &QPushButton::clicked, this, &LogVisualizationItemViewDialog::close);

	// Create shortcuts
	m_closeShortcut = new QShortcut(QKeySequence("Esc"), this);
	connect(m_closeShortcut, &QShortcut::activated, this, &LogVisualizationItemViewDialog::close);

	m_recenterShortcut = new QShortcut(QKeySequence("F11"), this);
	connect(m_recenterShortcut, &QShortcut::activated, this, &LogVisualizationItemViewDialog::slotRecenter);

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

	m_bigVLayout->addWidget(m_functionL, 0);
	m_bigVLayout->addWidget(m_function, 0);
	m_bigVLayout->addWidget(m_messageL, 0);
	m_bigVLayout->addWidget(m_message, 1);

	m_buttonLayout->addStretch(1);
	m_buttonLayout->addWidget(m_okButton);
	m_buttonLayout->addStretch(1);

	// Setup window
	setWindowTitle("MessageViewer (Message #" + QString::number(_index) + ") | OToolkit");
	setWindowFlag(Qt::WindowContextHelpButtonHint, false);
	setWindowIcon(AppBase::instance()->windowIcon());

	setMinimumSize(400, 300);
	setFocusPolicy(Qt::ClickFocus);
	
	QSettings s("OpenTwin", APP_BASE_APP_NAME);
	move(s.value("LogVisualizationItemViewDialog.X", 0).toInt(), s.value("LogVisualizationItemViewDialog.Y", 0).toInt());
	resize(s.value("LogVisualizationItemViewDialog.W", 800).toInt(), s.value("LogVisualizationItemViewDialog.H", 600).toInt());
}

LogVisualizationItemViewDialog::~LogVisualizationItemViewDialog() {
	
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

void LogVisualizationItemViewDialog::mousePressEvent(QMouseEvent* _event) {
	if (!geometry().contains(_event->globalPos())) {
		close();
	}
	else {
		QDialog::mousePressEvent(_event);
	}
}

void LogVisualizationItemViewDialog::slotRecenter(void) {
	this->move(0, 0);
	this->resize(800, 600);
}

void LogVisualizationItemViewDialog::closeEvent(QCloseEvent* _event) {
	QSettings s("OpenTwin", APP_BASE_APP_NAME);
	s.setValue("LogVisualizationItemViewDialog.X", pos().x());
	s.setValue("LogVisualizationItemViewDialog.Y", pos().y());
	s.setValue("LogVisualizationItemViewDialog.W", size().width());
	s.setValue("LogVisualizationItemViewDialog.H", size().height());

}