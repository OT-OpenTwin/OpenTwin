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
	QMenu * topLvlMenu = _menuBar->addMenu("Log Visualization");
	m_connectButton = topLvlMenu->addAction(QIcon(":/images/Shutdown.png"), "Connect");

	connect(m_connectButton, &QAction::triggered, this, &LogVisualization::slotConnect);
}

void LogVisualization::slotConnect(void) {
	ConnectToLoggerDialog dia;
	dia.exec();

	if (!dia.success()) {
		return;
	}

	m_centralLayoutW->setEnabled(true);
	m_connectButton->setEnabled(false);

	slotClear();

	const std::list<ot::LogMessage>& lst = dia.messageBuffer();
	
	for (auto msg : lst) {
		appendLogMessage(msg);
	}
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

void LogVisualization::appendLogMessage(const ot::LogMessage& _msg) {
	if (m_ignoreNewMessages->isChecked()) { return; };

	m_filterLock = true;

	int r = m_table->rowCount();
	m_table->insertRow(r);

	QString serviceName = QString::fromStdString(_msg.serviceName());

	QTableWidgetItem * iconItm = new QTableWidgetItem("");
	if (_msg.flags() & ot::WARNING_LOG) iconItm->setIcon(QIcon(":/images/Warning.png"));
	else if (_msg.flags() & ot::ERROR_LOG) iconItm->setIcon(QIcon(":/images/Error.png"));
	else iconItm->setIcon(QIcon(":/images/Info.png"));
	iniTableItem(r, tIcon, iconItm);

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
		m_errorCount++;
	}
	if (_msg.flags() & ot::WARNING_LOG) {
		if (!typeString.isEmpty()) typeString.append(" | ");
		typeString.append(TABLE_TXT_WARNING);
		m_warningCount++;
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
	iniTableItem(r, tType, new QTableWidgetItem(typeString));
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
