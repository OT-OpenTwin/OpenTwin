//! @file Logging.cpp
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OToolkit header
#include "AppBase.h"
#include "Logging.h"
#include "LogModeSetter.h"
#include "LoggingFilterView.h"
#include "ConnectToLoggerDialog.h"
#include "LogVisualizationItemViewDialog.h"

// OT header
#include "OTCommunication/ActionTypes.h"
#include "OTCommunication/Msg.h"
#include "OTWidgets/Splitter.h"

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
	: m_warningCount(0), m_errorCount(0), m_filterView(nullptr) 
{

}

Logging::~Logging() {
	/*otoolkit::SettingsRef settings = AppBase::instance()->createSettingsInstance();

	settings->setValue("Logging.AutoScrollToBottom", m_autoScrollToBottom->isChecked());

	settings->setValue("Logging.FilterActive.Detailed", m_msgTypeFilterDetailed->isChecked());
	settings->setValue("Logging.FilterActive.Info", m_msgTypeFilterInfo->isChecked());
	settings->setValue("Logging.FilterActive.Warning", m_msgTypeFilterWarning->isChecked());
	settings->setValue("Logging.FilterActive.Error", m_msgTypeFilterError->isChecked());
	settings->setValue("Logging.FilterActive.Message.In", m_msgTypeFilterMsgIn->isChecked());
	settings->setValue("Logging.FilterActive.Message.Out", m_msgTypeFilterMsgOut->isChecked());

	QJsonArray serviceFilterArr;
	for (int i = 0; i < m_serviceFilter->count(); i++) {
		QJsonObject serviceObj;
		serviceObj["Name"] = m_serviceFilter->item(i)->text();
		serviceObj["Active"] = (m_serviceFilter->item(i)->checkState() == Qt::Checked);
		serviceFilterArr.push_back(serviceObj);
	}
	QJsonDocument serviceFilterDoc(serviceFilterArr);
	settings->setValue("Logging.ServiceFilter.List", QVariant(serviceFilterDoc.toJson(QJsonDocument::Compact)));

	QString tableColumnWidths;
	for (int i = 0; i < m_table->columnCount(); i++) {
		tableColumnWidths.append(QString::number(m_table->columnWidth(i)) + ";");
	}
	settings->setValue("Logging.Table.ColumnWidth", tableColumnWidths);*/
}

QString Logging::toolName(void) const {
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
	_content.addView(this->createSideWidgetView(m_filterView->getRootWidget(), "Log Filter"));

	splitter->setOrientation(Qt::Orientation::Vertical);

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

	splitter->setStretchFactor(1, 1);
	
	// Initialize colors and text
	this->slotUpdateCheckboxColors();
	this->updateCountLabels();

	this->connect(m_filterView, &LoggingFilterView::filterChanged, this, &Logging::slotFilterChanged);

	// Connect checkbox color signals
	connect(m_ignoreNewMessages, &QCheckBox::stateChanged, this, &Logging::slotUpdateCheckboxColors);
	connect(m_autoScrollToBottom, &QCheckBox::stateChanged, this, &Logging::slotUpdateCheckboxColors);

	// Connect signals
	connect(m_autoScrollToBottom, &QCheckBox::stateChanged, this, &Logging::slotAutoScrollToBottomChanged);

	connect(m_table, &QTableWidget::itemDoubleClicked, this, &Logging::slotViewCellContent);

	// Create menu bar
	m_connectButton = _rootMenu->addAction(QIcon(":/images/Disconnected.png"), "Connect");
	m_autoConnect = _rootMenu->addAction("Auto-Connect");

	_rootMenu->addSeparator();
	m_importButton = _rootMenu->addAction(QIcon(":images/Import.png"), "Import");
	m_exportButton = _rootMenu->addAction(QIcon(":images/Export.png"), "Export");

	connect(m_connectButton, &QAction::triggered, this, &Logging::slotConnect);
	connect(m_autoConnect, &QAction::triggered, this, &Logging::slotToggleAutoConnect);
	connect(m_importButton, &QAction::triggered, this, &Logging::slotImport);
	connect(m_exportButton, &QAction::triggered, this, &Logging::slotExport);

	LOGVIS_LOG("Initialization completed");

	return true;
}

void Logging::restoreToolSettings(QSettings& _settings) {
	m_autoScrollToBottom->setChecked(_settings.value("Logging.AutoScrollToBottom", true).toBool());
	QString tableColumnWidths = _settings.value("Logging.Table.ColumnWidth", "").toString();
	QStringList tableColumnWidthsList = tableColumnWidths.split(";", Qt::SkipEmptyParts);
	if (tableColumnWidthsList.count() == m_table->columnCount()) {
		int column = 0;
		for (auto w : tableColumnWidthsList) {
			m_table->setColumnWidth(column++, w.toInt());
		}
	}

	m_logModeSetter->restoreSettings(_settings);
	m_filterView->restoreSettings(_settings);

	bool needAutoConnect = _settings.value("Logging.AutoConnect", false).toBool();
	m_autoConnect->setIcon(needAutoConnect ? QIcon(":/images/True.png") : QIcon(":/images/False.png"));

	if (needAutoConnect) {
		LOGVIS_LOG("Auto connect requested, request queued");
		QMetaObject::invokeMethod(this, &Logging::slotAutoConnect, Qt::QueuedConnection);
	}
}

//QList<QWidget *> Logging::statusBarWidgets(void) const {
//	QList<QWidget *> ret;
//	
//	ret.push_back(m_errorCountLabel);
//	ret.push_back(m_warningCountLabel);
//	ret.push_back(m_messageCountLabel);
//
//	return ret;
//}

bool Logging::prepareToolShutdown(QSettings& _settings) {
	_settings.setValue("Logging.AutoScrollToBottom", m_autoScrollToBottom->isChecked());

	QString tableColumnWidths;
	for (int i = 0; i < m_table->columnCount(); i++) {
		tableColumnWidths.append(QString::number(m_table->columnWidth(i)) + ";");
	}
	_settings.setValue("Logging.Table.ColumnWidth", tableColumnWidths);

	m_logModeSetter->saveSettings(_settings);
	m_filterView->saveSettings(_settings);

	if (this->disconnectFromLogger()) {
		return true;
	}
	else {
		LOGVIS_LOGE("Failed to send deregistration request to LoggerService \"" + QString::fromStdString(m_loggerUrl) + "\"");
		return false;
	}
}

void Logging::slotConnect(void) {
	this->connectToLogger(false);
}

void Logging::slotAutoConnect(void) {
	this->connectToLogger(true);
}

void Logging::slotImport(void) {
	otoolkit::SettingsRef settings = AppBase::instance()->createSettingsInstance();
	QString fn = QFileDialog::getOpenFileName(m_table, "Import Log Messages", settings->value("Logging.LastExportedFile", "").toString(), "OpenTwin Log (*.otlog.json)");
	if (fn.isEmpty()) return;

	QFile f(fn);
	if (!f.open(QIODevice::ReadOnly)) {
		LOGVIS_LOGE("Failed to open file for reading. File: \"" + fn + "\"");
		return;
	}

	ot::JsonDocument doc;
	doc.fromJson(f.readAll().toStdString());
	f.close();

	if (!doc.IsArray()) {
		LOGVIS_LOGE("Import failed: Document is not an array");
	}

	QList<ot::LogMessage> msg;

	for (rapidjson::SizeType i = 0; i < doc.Size(); i++) {
		try {
			ot::ConstJsonObject obj = ot::json::getObject(doc, i);

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

	settings->setValue("Logging.LastExportedFile", fn);
	LOGVIS_LOG("Log Messages successfully import from file \"" + fn + "\"");
}

void Logging::slotExport(void) {
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

	ot::JsonDocument doc(rapidjson::kArrayType);

	for (auto m : m_messages) {
		ot::JsonObject msgObj;
		m.addToJsonObject(msgObj, doc.GetAllocator());
		doc.PushBack(msgObj, doc.GetAllocator());
	}

	f.write(QByteArray::fromStdString(doc.toJson()));
	f.close();

	settings->setValue("Logging.LastExportedFile", fn);
	LOGVIS_LOG("Log Messages successfully exported to file \"" + fn + "\"");
}

void Logging::slotClear(void) {
	m_table->setRowCount(0);
	m_messages.clear();
	m_errorCount = 0;
	m_warningCount = 0;
	this->updateCountLabels();
}

void Logging::slotClearAll(void) {
	this->slotClear();
	m_filterView->reset();
	LOGVIS_LOG("Logging Clear All completed");
}

void Logging::slotFilterChanged(void) {
	int r = 0;
	for (const ot::LogMessage& msg : m_messages) {
		m_table->setRowHidden(r++, !m_filterView->filterMessage(msg));
	}
}

void Logging::slotAutoScrollToBottomChanged(void) {
	if (m_autoScrollToBottom->isChecked()) m_table->scrollToBottom();
}

void Logging::slotUpdateCheckboxColors(void) {
	QString red("QCheckBox { color: #c02020; }");
	QString def("");

	m_ignoreNewMessages->setStyleSheet(m_ignoreNewMessages->isChecked() ? red : def);
	m_autoScrollToBottom->setStyleSheet(m_autoScrollToBottom->isChecked() ? red : def);
}

void Logging::slotToggleAutoConnect(void) {
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
			dia.exec();
			return;
		}
	}
}

void Logging::appendLogMessage(const ot::LogMessage& _msg) {
	if (m_ignoreNewMessages->isChecked()) { return; };

	m_filterView->setFilterLock(true);

	int r = m_table->rowCount();
	m_table->insertRow(r);

	QString serviceName = QString::fromStdString(_msg.getServiceName());

	QTableWidgetItem* iconItm = new QTableWidgetItem("");
	if (_msg.getFlags() & ot::WARNING_LOG) {
		iconItm->setIcon(QIcon(":/images/Warning.png"));
		m_warningCount++;
	}
	else if (_msg.getFlags() & ot::ERROR_LOG) {
		iconItm->setIcon(QIcon(":/images/Error.png"));
		m_errorCount++;
	}
	else {
		iconItm->setIcon(QIcon(":/images/Info.png"));
	}
	this->iniTableItem(r, tIcon, iconItm);


	this->iniTableItem(r, tType, new QTableWidgetItem(logMessageTypeString(_msg)));
	this->iniTableItem(r, tTimeGlobal, new QTableWidgetItem(QString::fromStdString(_msg.getGlobalSystemTime())));
	this->iniTableItem(r, tTimeLocal, new QTableWidgetItem(QString::fromStdString(_msg.getLocalSystemTime())));
	this->iniTableItem(r, tUser, new QTableWidgetItem(QString::fromStdString(_msg.getUserName())));
	this->iniTableItem(r, tProject, new QTableWidgetItem(QString::fromStdString(_msg.getProjectName())));
	this->iniTableItem(r, tService, new QTableWidgetItem(serviceName));
	this->iniTableItem(r, tFunction, new QTableWidgetItem(QString::fromStdString(_msg.getFunctionName())));
	this->iniTableItem(r, tMessage, new QTableWidgetItem(QString::fromStdString(_msg.getText())));

	// Store message
	m_messages.push_back(_msg);

	// If required scroll to the last entry in the table
	if (m_autoScrollToBottom->isChecked()) m_table->scrollToBottom();

	// Check if the service name is in the service name list
	m_filterView->setFilterLock(false);

	// Update status labels (warnings and errors were increased while checking the typeString)
	this->updateCountLabels();

	// Check the filter for the new entry
	m_table->setRowHidden(r, !m_filterView->filterMessage(_msg));
}

void Logging::appendLogMessages(const QList<ot::LogMessage>& _messages) {
	bool actb = m_autoScrollToBottom->isChecked();
	m_autoScrollToBottom->setChecked(false);
	for (auto m : _messages) {
		appendLogMessage(m);
	}
	m_autoScrollToBottom->setChecked(actb);
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

	return typeString;
}

void Logging::iniTableItem(int _row, int _column, QTableWidgetItem* _itm) {
	//_itm->setFlags(_itm->flags() & (~Qt::ItemIsEditable) & (~Qt::ItemIsSelectable));
	m_table->setItem(_row, _column, _itm);
}

void Logging::updateCountLabels(void) {
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
	this->appendLogMessages(dia.messageBuffer());

	LOGVIS_LOG("Done.");
}

bool Logging::disconnectFromLogger(void) {
	if (m_loggerUrl.empty()) return true;

	std::string response;
	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_RemoveService, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_SERVICE_URL, ot::JsonString(AppBase::instance()->url().toStdString(), doc.GetAllocator()), doc.GetAllocator());

	if (!ot::msg::send("", m_loggerUrl, ot::EXECUTE, doc.toJson(), response, 3000, false, false)) {
		LOGVIS_LOGE("Failed to send remove service request to logger service.");
		return false;
	}

	LOGVIS_LOG("Successfully deregistered at LoggerService \"" + QString::fromStdString(m_loggerUrl) + "\"");
	m_loggerUrl.clear();

	m_connectButton->setText("Connect");
	m_connectButton->setIcon(QIcon(":/images/Disconnected.png"));

	return true;
}