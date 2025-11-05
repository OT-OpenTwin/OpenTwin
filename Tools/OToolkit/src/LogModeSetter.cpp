// @otlicense
// File: LogModeSetter.cpp
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
#include "LogModeSetter.h"

// OpenTwin header
#include "OTCore/JSON.h"
#include "OTCore/ReturnMessage.h"
#include "OTCommunication/Msg.h"
#include "OTCommunication/ActionTypes.h"

// Qt header
#include <QtWidgets/qlabel.h>
#include <QtWidgets/qlayout.h>
#include <QtWidgets/qcombobox.h>
#include <QtWidgets/qcheckbox.h>
#include <QtWidgets/qgroupbox.h>
#include <QtWidgets/qlineedit.h>
#include <QtWidgets/qpushbutton.h>
#include <QtWidgets/qscrollarea.h>
#include <QtWidgets/qmessagebox.h>

// std header
#include <thread>

#define LMS_Mode_Global "Global"

LogModeSetter::LogModeSetter() {
	// Create layouts
	QScrollArea* rootScrollArea = new QScrollArea;
	rootScrollArea->setWidgetResizable(true);
	m_root = rootScrollArea;

	QWidget* rootLayoutW = new QWidget(rootScrollArea);
	rootScrollArea->setWidget(rootLayoutW);

	QVBoxLayout* rootLayout = new QVBoxLayout(rootLayoutW);

	QGridLayout* inputLayout = new QGridLayout;
	rootLayout->addLayout(inputLayout);

	QLabel* gssL = new QLabel("GSS Url:", rootLayoutW);
	inputLayout->addWidget(gssL, 0, 0);

	m_gssUrl = new QLineEdit(rootLayoutW);
	m_gssUrl->setPlaceholderText("GSS URL (e.g. 127.0.0.1:8091");
	m_gssUrl->setToolTip("GSS (Global Session Service) url.");
	inputLayout->addWidget(m_gssUrl, 0, 1);

	QLabel* modeL = new QLabel("Mode:", rootLayoutW);
	inputLayout->addWidget(modeL, 1, 0);

	m_mode = new QComboBox(rootLayoutW);
	m_mode->addItems({ LMS_Mode_Global });
	m_mode->setToolTip("Type of log command:\n"
		LMS_Mode_Global ": Log Flags will be applied to all services in all sessions.\n"
	);
	inputLayout->addWidget(m_mode, 1, 1);
	
	QGroupBox* typeBox = new QGroupBox("Active Log Type", rootLayoutW);
	rootLayout->addWidget(typeBox);

	QVBoxLayout* typeLayout = new QVBoxLayout(typeBox);

	m_msgTypeInfo = new QCheckBox("Info", typeBox);
	typeLayout->addWidget(m_msgTypeInfo);

	m_msgTypeDetailed = new QCheckBox("Detailed", typeBox);
	typeLayout->addWidget(m_msgTypeDetailed);

	m_msgTypeWarning = new QCheckBox("Warning", typeBox);
	typeLayout->addWidget(m_msgTypeWarning);

	m_msgTypeError = new QCheckBox("Error", typeBox);
	typeLayout->addWidget(m_msgTypeError);

	m_msgTypeMsgIn = new QCheckBox("Inbound Message", typeBox);
	typeLayout->addWidget(m_msgTypeMsgIn);

	m_msgTypeMsgOut = new QCheckBox("Outgoing Message", typeBox);
	typeLayout->addWidget(m_msgTypeMsgOut);

	m_msgTypeTest = new QCheckBox("Test", typeBox);
	typeLayout->addWidget(m_msgTypeTest);

	rootLayout->addStretch(1);

	QPushButton* applyButton = new QPushButton("Send", rootLayoutW);
	rootLayout->addWidget(applyButton);
	this->connect(applyButton, &QPushButton::clicked, this, &LogModeSetter::slotApply);
}

LogModeSetter::~LogModeSetter() {

}

void LogModeSetter::restoreSettings(QSettings& _settings) {
	m_gssUrl->setText(_settings.value("LogModeSetter.GSSUrl", QString("127.0.0.1:8091")).toString());

	m_msgTypeDetailed->setChecked(_settings.value("LogModeSetter.Detailed", true).toBool());
	m_msgTypeInfo->setChecked(_settings.value("LogModeSetter.Info", true).toBool());
	m_msgTypeWarning->setChecked(_settings.value("LogModeSetter.Warning", true).toBool());
	m_msgTypeError->setChecked(_settings.value("LogModeSetter.Error", true).toBool());
	m_msgTypeMsgIn->setChecked(_settings.value("LogModeSetter.Message.In", false).toBool());
	m_msgTypeMsgOut->setChecked(_settings.value("LogModeSetter.Message.Out", false).toBool());
	m_msgTypeTest->setChecked(_settings.value("LogModeSetter.Test", false).toBool());
}

void LogModeSetter::saveSettings(QSettings& _settings) {
	_settings.setValue("LogModeSetter.GSSUrl", m_gssUrl->text());

	_settings.setValue("LogModeSetter.Detailed", m_msgTypeDetailed->isChecked());
	_settings.setValue("LogModeSetter.Info", m_msgTypeInfo->isChecked());
	_settings.setValue("LogModeSetter.Warning", m_msgTypeWarning->isChecked());
	_settings.setValue("LogModeSetter.Error", m_msgTypeError->isChecked());
	_settings.setValue("LogModeSetter.Message.In", m_msgTypeMsgIn->isChecked());
	_settings.setValue("LogModeSetter.Message.Out", m_msgTypeMsgOut->isChecked());
	_settings.setValue("LogModeSetter.Test", m_msgTypeTest->isChecked());
}

void LogModeSetter::slotApply(void) {
	std::string gssUrl = m_gssUrl->text().toStdString();
	if (gssUrl.empty()) {
		QMessageBox msg(QMessageBox::Warning, "Invalid GSS Url", "No Global Session Service url set.", QMessageBox::Ok);
		msg.exec();
		return;
	}

	ot::LogFlags newFlags(ot::NO_LOG);
	if (m_msgTypeDetailed->isChecked()) newFlags |= ot::DETAILED_LOG;
	if (m_msgTypeInfo->isChecked()) newFlags |= ot::INFORMATION_LOG;
	if (m_msgTypeWarning->isChecked()) newFlags |= ot::WARNING_LOG;
	if (m_msgTypeError->isChecked()) newFlags |= ot::ERROR_LOG;
	if (m_msgTypeMsgIn->isChecked()) newFlags |= ot::ALL_INCOMING_MESSAGE_LOG_FLAGS;
	if (m_msgTypeMsgOut->isChecked()) newFlags |= ot::ALL_OUTGOING_MESSAGE_LOG_FLAGS;
	if (m_msgTypeTest->isChecked()) newFlags |= ot::TEST_LOG;

	m_root->setEnabled(false);

	std::thread worker(&LogModeSetter::sendWorker, this, gssUrl, newFlags);
	worker.detach();
}

void LogModeSetter::slotWorkerSuccess(void) {
	m_root->setEnabled(true);
	OT_LOG_I("Log mode request send successfully.");
}

void LogModeSetter::slotWorkerError(void) {
	QMessageBox msg(QMessageBox::Critical, "Send Error", "Failed to send request to Global Session Service.", QMessageBox::Ok);
	msg.exec();
	m_root->setEnabled(true);
}

void LogModeSetter::stopWorker(bool _success) {
	if (_success) {
		QMetaObject::invokeMethod(this, &LogModeSetter::slotWorkerSuccess, Qt::QueuedConnection);
	}
	else {
		QMetaObject::invokeMethod(this, &LogModeSetter::slotWorkerError, Qt::QueuedConnection);
	}
}

void LogModeSetter::sendWorker(std::string _gss, ot::LogFlags _newMode) {
	this->stopWorker(this->sendGlobalMode(_gss, _newMode));
}

bool LogModeSetter::sendGlobalMode(const std::string& _gss, const ot::LogFlags& _flags) {
	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_SetGlobalLogFlags, doc.GetAllocator()), doc.GetAllocator());

	ot::JsonArray flagsArr;
	ot::addLogFlagsToJsonArray(_flags, flagsArr, doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_Flags, flagsArr, doc.GetAllocator());

	std::string responseStr;
	if (!ot::msg::send("", _gss, ot::EXECUTE, doc.toJson(), responseStr, ot::msg::defaultTimeout, ot::msg::NoRequestFlags)) {
		OT_LOG_E("Failed to send request");
		return false;
	}

	ot::ReturnMessage response = ot::ReturnMessage::fromJson(responseStr);
	if (!response.isOk()) {
		OT_LOG_E("Error response: \"" + response.getWhat() + "\"");
		return false;
	}
	else {
		return true;
	}
}