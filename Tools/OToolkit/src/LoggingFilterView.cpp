// @otlicense
// File: LoggingFilterView.cpp
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
#include "LoggingFilterView.h"

// OpenTwin header
#include "OTWidgets/SpinBox.h"
#include "OTWidgets/CheckBox.h"

// Qt header
#include <QtCore/qjsonarray.h>
#include <QtCore/qjsonobject.h>
#include <QtCore/qjsondocument.h>
#include <QtWidgets/qlabel.h>
#include <QtWidgets/qlayout.h>
#include <QtWidgets/qlineedit.h>
#include <QtWidgets/qcombobox.h>
#include <QtWidgets/qcheckbox.h>
#include <QtWidgets/qgroupbox.h>
#include <QtWidgets/qlistwidget.h>
#include <QtWidgets/qpushbutton.h>
#include <QtWidgets/qscrollarea.h>

#define LFV_Filter_AllNames "< All >"

LoggingFilterView::LoggingFilterView() 
	: m_filterTimer(nullptr), m_filterLock(true)
{
	// Create layouts
	QScrollArea* scrollArea = new QScrollArea;
	m_root = scrollArea;
	scrollArea->setObjectName("LoggingFilterViewRoot");
	scrollArea->setWidgetResizable(true);

	QWidget* centralLayoutW = new QWidget(scrollArea);
	QVBoxLayout* centralLayout = new QVBoxLayout(centralLayoutW);
	scrollArea->setWidget(centralLayoutW);

	QHBoxLayout* messageContainsLayout = new QHBoxLayout;
	centralLayout->addLayout(messageContainsLayout);

	// Message Content Filter

	QLabel* messageFilterL = new QLabel("Message contains:", centralLayoutW);
	m_messageFilter = new QLineEdit(centralLayoutW);
	m_messageFilter->setPlaceholderText("<Confirm by pressing return>");
	m_messageFilter->setToolTip("Only messages containing the specified text will be displayed. Case insensitive.");
	messageContainsLayout->addWidget(messageFilterL);
	messageContainsLayout->addWidget(m_messageFilter, 1);
	this->connect(m_messageFilter, &QLineEdit::returnPressed, this, &LoggingFilterView::slotFilterChanged);

	// Message Type Filter

	QGroupBox* filterByMessageTypeBox = new QGroupBox("Message Type Filter", centralLayoutW);
	QVBoxLayout* filterByMessageTypeLayout = new QVBoxLayout(filterByMessageTypeBox);
	centralLayout->addWidget(filterByMessageTypeBox, 0);

	m_msgTypeFilterDetailed = new ot::CheckBox("Detailed", filterByMessageTypeBox);
	filterByMessageTypeLayout->addWidget(m_msgTypeFilterDetailed);
	this->connect(m_msgTypeFilterDetailed, &QCheckBox::stateChanged, this, &LoggingFilterView::slotUpdateCheckboxColors);
	this->connect(m_msgTypeFilterDetailed, &QCheckBox::stateChanged, this, &LoggingFilterView::slotFilterChanged);
	m_msgTypeFilterDetailed->setChecked(true);

	m_msgTypeFilterInfo = new ot::CheckBox("Info", filterByMessageTypeBox);
	filterByMessageTypeLayout->addWidget(m_msgTypeFilterInfo);
	this->connect(m_msgTypeFilterInfo, &QCheckBox::stateChanged, this, &LoggingFilterView::slotUpdateCheckboxColors);
	this->connect(m_msgTypeFilterInfo, &QCheckBox::stateChanged, this, &LoggingFilterView::slotFilterChanged);
	m_msgTypeFilterInfo->setChecked(true);

	m_msgTypeFilterWarning = new ot::CheckBox("Warning", filterByMessageTypeBox);
	filterByMessageTypeLayout->addWidget(m_msgTypeFilterWarning);
	this->connect(m_msgTypeFilterWarning, &QCheckBox::stateChanged, this, &LoggingFilterView::slotUpdateCheckboxColors);
	this->connect(m_msgTypeFilterWarning, &QCheckBox::stateChanged, this, &LoggingFilterView::slotFilterChanged);
	m_msgTypeFilterWarning->setChecked(true);

	m_msgTypeFilterError = new ot::CheckBox("Error", filterByMessageTypeBox);
	filterByMessageTypeLayout->addWidget(m_msgTypeFilterError);
	this->connect(m_msgTypeFilterError, &QCheckBox::stateChanged, this, &LoggingFilterView::slotUpdateCheckboxColors);
	this->connect(m_msgTypeFilterError, &QCheckBox::stateChanged, this, &LoggingFilterView::slotFilterChanged);
	m_msgTypeFilterError->setChecked(true);

	m_msgTypeFilterMsgIn = new ot::CheckBox("Inbound Message", filterByMessageTypeBox);
	filterByMessageTypeLayout->addWidget(m_msgTypeFilterMsgIn);
	this->connect(m_msgTypeFilterMsgIn, &QCheckBox::stateChanged, this, &LoggingFilterView::slotUpdateCheckboxColors);
	this->connect(m_msgTypeFilterMsgIn, &QCheckBox::stateChanged, this, &LoggingFilterView::slotFilterChanged);

	m_msgTypeFilterMsgOut = new ot::CheckBox("Outgoing Message", filterByMessageTypeBox);
	filterByMessageTypeLayout->addWidget(m_msgTypeFilterMsgOut);
	this->connect(m_msgTypeFilterMsgOut, &QCheckBox::stateChanged, this, &LoggingFilterView::slotUpdateCheckboxColors);
	this->connect(m_msgTypeFilterMsgOut, &QCheckBox::stateChanged, this, &LoggingFilterView::slotFilterChanged);

	m_msgTypeFilterTest = new ot::CheckBox("Testing", filterByMessageTypeBox);
	filterByMessageTypeLayout->addWidget(m_msgTypeFilterTest);
	this->connect(m_msgTypeFilterTest, &QCheckBox::stateChanged, this, &LoggingFilterView::slotUpdateCheckboxColors);
	this->connect(m_msgTypeFilterTest, &QCheckBox::stateChanged, this, &LoggingFilterView::slotFilterChanged);

	// User Filter

	QGroupBox* filterByUserBox = new QGroupBox("User Filter", centralLayoutW);
	QVBoxLayout* filterByUserLayout = new QVBoxLayout(filterByUserBox);
	centralLayout->addWidget(filterByUserBox, 0);

	m_userFilter = new QComboBox(filterByUserBox);
	m_userFilter->setToolTip("Only messages from the selected user will be displayed.");
	m_userFilter->addItem(LFV_Filter_AllNames);
	filterByUserLayout->addWidget(m_userFilter);
	this->connect(m_userFilter, &QComboBox::currentTextChanged, this, &LoggingFilterView::slotFilterChanged);
	
	// Project Filter

	QGroupBox* filterByProjectBox = new QGroupBox("Project Filter", centralLayoutW);
	QVBoxLayout* filterByProjectLayout = new QVBoxLayout(filterByProjectBox);
	centralLayout->addWidget(filterByProjectBox, 0);

	m_sessionFilter = new QComboBox(filterByProjectBox);
	m_sessionFilter->setToolTip("Only messages from the selected project will be displayed.");
	m_sessionFilter->addItem(LFV_Filter_AllNames);
	filterByProjectLayout->addWidget(m_sessionFilter);
	this->connect(m_sessionFilter, &QComboBox::currentTextChanged, this, &LoggingFilterView::slotFilterChanged);

	// Service Type Filter
	QGroupBox* filterByServiceBox = new QGroupBox("Service Type Filter", centralLayoutW);
	QVBoxLayout* filterByServiceLayout = new QVBoxLayout(filterByServiceBox);
	centralLayout->addWidget(filterByServiceBox, 1);
	
	m_serviceFilter = new QListWidget(filterByServiceBox);
	filterByServiceLayout->addWidget(m_serviceFilter);
	this->connect(m_serviceFilter, &QListWidget::itemChanged, this, &LoggingFilterView::slotFilterChanged);

	QPushButton* btnSelectAllServices = new QPushButton("Select All", filterByServiceBox);
	btnSelectAllServices->setToolTip("Select all services in the list");
	filterByServiceLayout->addWidget(btnSelectAllServices);
	this->connect(btnSelectAllServices, &QPushButton::clicked, this, &LoggingFilterView::slotSelectAllServices);

	QPushButton* btnDeselectAllServices = new QPushButton("Deselect All", filterByServiceBox);
	btnDeselectAllServices->setToolTip("Deselect all services in the list");
	filterByServiceLayout->addWidget(btnDeselectAllServices);
	this->connect(btnDeselectAllServices, &QPushButton::clicked, this, &LoggingFilterView::slotDeselectAllServices);

	// Settings
	QGroupBox* settingsBox = new QGroupBox("Settings", centralLayoutW);
	QVBoxLayout* settingsLayout = new QVBoxLayout(settingsBox);
	centralLayout->addWidget(settingsBox, 0);

	m_messageLimitEnabled = new ot::CheckBox("Enable Message Limit", settingsBox);
	m_messageLimitEnabled->setToolTip("If enabled, only the specified number of messages will be kept in the log. Older messages will be removed.");
	this->connect(m_messageLimitEnabled, &QCheckBox::stateChanged, this, &LoggingFilterView::slotMessageLimitChanged);

	QHBoxLayout* messageLimitLayout = new QHBoxLayout;
	messageLimitLayout->setContentsMargins(0, 0, 0, 0);
	m_messageLimit = new ot::SpinBox(100, std::numeric_limits<int>::max(), 20000, settingsBox);
	m_messageLimit->setToolTip("Maximum number of messages that will be kept in the log.");
	messageLimitLayout->addWidget(new QLabel("Limit:"));
	messageLimitLayout->addWidget(m_messageLimit, 1);
	settingsLayout->addWidget(m_messageLimitEnabled);
	settingsLayout->addLayout(messageLimitLayout);
	this->connect(m_messageLimit, &ot::SpinBox::valueChangeCompleted, this, &LoggingFilterView::slotMessageLimitChanged);

	m_useInterval = new ot::CheckBox("Use Interval", settingsBox);
	m_useInterval->setToolTip("If enabled, the log will be updated only in the specified intervals. This can help to reduce the load on the GUI when a lot of messages are incoming.");
	this->connect(m_useInterval, &QCheckBox::stateChanged, this, &LoggingFilterView::slotIntervalChanged);

	QHBoxLayout* intervalLayout = new QHBoxLayout;
	intervalLayout->setContentsMargins(0, 0, 0, 0);
	m_intervalMilliseconds = new ot::SpinBox(10, 10000, 1000, settingsBox);
	m_intervalMilliseconds->setToolTip("Interval in milliseconds that will be used to update the log view.");
	m_intervalMilliseconds->setSuffix(" ms");
	intervalLayout->addWidget(new QLabel("Interval:"));
	intervalLayout->addWidget(m_intervalMilliseconds, 1);
	settingsLayout->addWidget(m_useInterval);
	settingsLayout->addLayout(intervalLayout);
	this->connect(m_intervalMilliseconds, &ot::SpinBox::valueChangeCompleted, this, &LoggingFilterView::slotIntervalChanged);

	// Initialize colors
	slotUpdateCheckboxColors();
}

LoggingFilterView::~LoggingFilterView() {

}

void LoggingFilterView::reset() {
	m_serviceFilter->clear();
}

bool LoggingFilterView::filterMessage(const ot::LogMessage& _msg) {
	bool found = false;
	QString serviceName = QString::fromStdString(_msg.getServiceName());

	// Check Service Name exists
	for (int i = 0; i < m_serviceFilter->count(); i++) {
		if (m_serviceFilter->item(i)->text() == serviceName) {
			found = true;
			break;
		}
	}

	if (!found) {
		bool tmp = m_filterLock;
		m_filterLock = true;
		QListWidgetItem* itm = new QListWidgetItem(serviceName);
		itm->setFlags(itm->flags() | Qt::ItemIsUserCheckable);
		itm->setCheckState(Qt::Checked);
		m_serviceFilter->addItem(itm);
		m_serviceFilter->sortItems(Qt::SortOrder::AscendingOrder);
		m_filterLock = tmp;
	}

	// Check User Name exists
	QString userName = QString::fromStdString(_msg.getUserName());
	if (!userName.isEmpty()) {
		found = false;
		for (int i = 0; i < m_userFilter->count(); i++) {
			if (m_userFilter->itemText(i) == userName) {
				found = true;
				break;
			}
		}

		if (!found) {
			m_filterLock = true;
			m_userFilter->addItem(userName);
			m_filterLock = false;
		}
	}

	// Check Project Name exists
	QString projectName = QString::fromStdString(_msg.getProjectName());
	if (!projectName.isEmpty()) {
		found = false;
		for (int i = 0; i < m_sessionFilter->count(); i++) {
			if (m_sessionFilter->itemText(i) == projectName) {
				found = true;
				break;
			}
		}

		if (!found) {
			m_filterLock = true;
			m_sessionFilter->addItem(projectName);
			m_filterLock = false;
		}
	}

	// Type

	bool typePass = false;
	if (m_msgTypeFilterDetailed->isChecked() && (_msg.getFlags() & ot::DETAILED_LOG)) typePass = true;
	else if (m_msgTypeFilterInfo->isChecked() && (_msg.getFlags() & ot::INFORMATION_LOG)) typePass = true;
	else if (m_msgTypeFilterWarning->isChecked() && (_msg.getFlags() & ot::WARNING_LOG)) typePass = true;
	else if (m_msgTypeFilterError->isChecked() && (_msg.getFlags() & ot::ERROR_LOG)) typePass = true;
	else if (m_msgTypeFilterMsgIn->isChecked() && (_msg.getFlags() & ot::ALL_INCOMING_MESSAGE_LOG_FLAGS)) typePass = true;
	else if (m_msgTypeFilterMsgOut->isChecked() && (_msg.getFlags() & ot::ALL_OUTGOING_MESSAGE_LOG_FLAGS)) typePass = true;
	else if (m_msgTypeFilterTest->isChecked() && (_msg.getFlags() & ot::TEST_LOG)) typePass = true;

	// User

	bool userPass = true;
	if (m_userFilter->currentText() != LFV_Filter_AllNames) {
		userPass = m_userFilter->currentText() == userName;
	}

	// Project

	bool projectPass = true;
	if (m_sessionFilter->currentText() != LFV_Filter_AllNames) {
		projectPass = m_sessionFilter->currentText() == projectName;
	}

	// Service

	bool servicePass = false;
	for (int i = 0; i < m_serviceFilter->count(); i++) {
		if (m_serviceFilter->item(i)->checkState() == Qt::Checked && m_serviceFilter->item(i)->text() == serviceName) {
			servicePass = true;
			break;
		}
	}

	// Message

	bool messagePass = true;
	QString txt = QString::fromStdString(_msg.getText());
	if (!m_messageFilter->text().isEmpty()) {
		messagePass = txt.contains(m_messageFilter->text(), Qt::CaseInsensitive);
	}

	return typePass && userPass && projectPass && servicePass && messagePass;
}

void LoggingFilterView::restoreSettings(QSettings& _settings) {
	m_msgTypeFilterDetailed->setChecked(_settings.value("LoggingFilterView.FilterActive.Detailed", true).toBool());
	m_msgTypeFilterInfo->setChecked(_settings.value("LoggingFilterView.FilterActive.Info", true).toBool());
	m_msgTypeFilterWarning->setChecked(_settings.value("LoggingFilterView.FilterActive.Warning", true).toBool());
	m_msgTypeFilterError->setChecked(_settings.value("LoggingFilterView.FilterActive.Error", true).toBool());
	m_msgTypeFilterMsgIn->setChecked(_settings.value("LoggingFilterView.FilterActive.Message.In", false).toBool());
	m_msgTypeFilterMsgOut->setChecked(_settings.value("LoggingFilterView.FilterActive.Message.Out", false).toBool());
	m_msgTypeFilterTest->setChecked(_settings.value("LoggingFilterView.FilterActive.Test", false).toBool());

	m_messageLimitEnabled->setChecked(_settings.value("LoggingFilterView.MessageLimitActive", false).toBool());
	m_messageLimit->setValue(_settings.value("LoggingFilterView.MessageLimit", 20000).toInt());
	m_useInterval->setChecked(_settings.value("LoggingFilterView.IntervalActive", false).toBool());
	m_intervalMilliseconds->setValue(_settings.value("LoggingFilterView.IntervalMilliseconds", 1000).toInt());

	QByteArray serviceFilter = _settings.value("LoggingFilterView.ServiceFilter.List", QByteArray()).toByteArray();
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

	this->slotUpdateCheckboxColors();
	this->updateMessageLimitColor();
	this->updateIntervalColor();

	Q_EMIT filterChanged();
	Q_EMIT messageLimitChanged(this->getMessageLimit());
	Q_EMIT useIntervalChaged();
}

void LoggingFilterView::saveSettings(QSettings& _settings) {
	_settings.setValue("LoggingFilterView.FilterActive.Detailed", m_msgTypeFilterDetailed->isChecked());
	_settings.setValue("LoggingFilterView.FilterActive.Info", m_msgTypeFilterInfo->isChecked());
	_settings.setValue("LoggingFilterView.FilterActive.Warning", m_msgTypeFilterWarning->isChecked());
	_settings.setValue("LoggingFilterView.FilterActive.Error", m_msgTypeFilterError->isChecked());
	_settings.setValue("LoggingFilterView.FilterActive.Message.In", m_msgTypeFilterMsgIn->isChecked());
	_settings.setValue("LoggingFilterView.FilterActive.Message.Out", m_msgTypeFilterMsgOut->isChecked());
	_settings.setValue("LoggingFilterView.FilterActive.Test", m_msgTypeFilterTest->isChecked());

	_settings.setValue("LoggingFilterView.MessageLimitActive", m_messageLimitEnabled->isChecked());
	_settings.setValue("LoggingFilterView.MessageLimit", m_messageLimit->value());
	_settings.setValue("LoggingFilterView.IntervalActive", m_useInterval->isChecked());
	_settings.setValue("LoggingFilterView.IntervalMilliseconds", m_intervalMilliseconds->value());

	QJsonArray serviceFilterArr;
	for (int i = 0; i < m_serviceFilter->count(); i++) {
		QJsonObject serviceObj;
		serviceObj["Name"] = m_serviceFilter->item(i)->text();
		serviceObj["Active"] = (m_serviceFilter->item(i)->checkState() == Qt::Checked);
		serviceFilterArr.push_back(serviceObj);
	}
	QJsonDocument serviceFilterDoc(serviceFilterArr);
	_settings.setValue("LoggingFilterView.ServiceFilter.List", QVariant(serviceFilterDoc.toJson(QJsonDocument::Compact)));}

int LoggingFilterView::getMessageLimit() const {
	if (m_messageLimitEnabled->isChecked()) {
		return m_messageLimit->value();
	}
	else {
		return std::numeric_limits<int>::max();
	}
}

bool LoggingFilterView::getUseInterval() const {
	return m_useInterval->isChecked();
}

int LoggingFilterView::getIntervalMilliseconds() const {
	return m_intervalMilliseconds->value();
}

void LoggingFilterView::slotUpdateCheckboxColors() {
	m_msgTypeFilterDetailed->setSuccessForeground(m_msgTypeFilterDetailed->isChecked());
	m_msgTypeFilterDetailed->setErrorForeground(!m_msgTypeFilterDetailed->isChecked());

	m_msgTypeFilterInfo->setSuccessForeground(m_msgTypeFilterInfo->isChecked());
	m_msgTypeFilterInfo->setErrorForeground(!m_msgTypeFilterInfo->isChecked());

	m_msgTypeFilterWarning->setSuccessForeground(m_msgTypeFilterWarning->isChecked());
	m_msgTypeFilterWarning->setErrorForeground(!m_msgTypeFilterWarning->isChecked());

	m_msgTypeFilterError->setSuccessForeground(m_msgTypeFilterError->isChecked());
	m_msgTypeFilterError->setErrorForeground(!m_msgTypeFilterError->isChecked());

	m_msgTypeFilterMsgIn->setSuccessForeground(m_msgTypeFilterMsgIn->isChecked());
	m_msgTypeFilterMsgIn->setErrorForeground(!m_msgTypeFilterMsgIn->isChecked());

	m_msgTypeFilterMsgOut->setSuccessForeground(m_msgTypeFilterMsgOut->isChecked());
	m_msgTypeFilterMsgOut->setErrorForeground(!m_msgTypeFilterMsgOut->isChecked());

	m_msgTypeFilterTest->setSuccessForeground(m_msgTypeFilterTest->isChecked());
	m_msgTypeFilterTest->setErrorForeground(!m_msgTypeFilterTest->isChecked());
}

void LoggingFilterView::slotFilterChanged() {
	if (m_filterLock) return;
	Q_EMIT filterChanged();
}

void LoggingFilterView::slotSelectAllServices() {
	m_filterLock = true;
	for (int i = 0; i < m_serviceFilter->count(); i++) m_serviceFilter->item(i)->setCheckState(Qt::Checked);
	m_filterLock = false;
	this->slotFilterChanged();
}

void LoggingFilterView::slotDeselectAllServices() {
	m_filterLock = true;
	for (int i = 0; i < m_serviceFilter->count(); i++) m_serviceFilter->item(i)->setCheckState(Qt::Unchecked);
	m_filterLock = false;
	this->slotFilterChanged();
}

void LoggingFilterView::slotMessageLimitChanged() {
	this->updateMessageLimitColor();
	Q_EMIT messageLimitChanged(this->getMessageLimit());
}

void LoggingFilterView::slotIntervalChanged() {
	this->updateIntervalColor();
	Q_EMIT useIntervalChaged();
}

void LoggingFilterView::updateMessageLimitColor() {
	m_messageLimitEnabled->setSuccessForeground(m_messageLimitEnabled->isChecked());
	m_messageLimitEnabled->setErrorForeground(!m_messageLimitEnabled->isChecked());
}

void LoggingFilterView::updateIntervalColor() {
	m_useInterval->setSuccessForeground(m_useInterval->isChecked());
	m_useInterval->setErrorForeground(!m_useInterval->isChecked());
}
