//! @file LogModeSetter.cpp
//! @author Alexander Kuester (alexk95)
//! @date August 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OToolkit header
#include "LogModeSetter.h"

// OpenTwin header
#include "OTCore/JSON.h"
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
	m_root = rootScrollArea;
	QWidget* rootLayoutW = new QWidget;
	QVBoxLayout* rootLayout = new QVBoxLayout(rootLayoutW);
	rootScrollArea->setWidget(rootLayoutW);
	rootScrollArea->setWidgetResizable(true);
	
	QGridLayout* inputLayout = new QGridLayout;
	
	QGroupBox* typeBox = new QGroupBox("Active Log Type");
	QVBoxLayout* typeLayout = new QVBoxLayout(typeBox);

	// Create controls
	QLabel* gssL = new QLabel("GSS Url:");
	m_gssUrl = new QLineEdit;
	m_gssUrl->setPlaceholderText("GSS URL (e.g. 127.0.0.1:8091");
	m_gssUrl->setToolTip("GSS (Global Session Service) url.");

	QLabel* modeL = new QLabel("Mode:");
	m_mode = new QComboBox;
	m_mode->addItems({ LMS_Mode_Global });
	m_mode->setToolTip("Type of log command:\n"
		LMS_Mode_Global ": Log Flags will be applied to all services in all sessions.\n"
	);

	m_msgTypeDetailed = new QCheckBox("Detailed");
	m_msgTypeInfo = new QCheckBox("Info");
	m_msgTypeWarning = new QCheckBox("Warning");
	m_msgTypeError = new QCheckBox("Error");
	m_msgTypeMsgIn = new QCheckBox("Inbound Message");
	m_msgTypeMsgOut = new QCheckBox("Outgoing Message");

	QPushButton* applyButton = new QPushButton("Send");

	// Setup layouts
	rootLayout->addLayout(inputLayout);
	rootLayout->addWidget(typeBox);
	rootLayout->addStretch(1);
	rootLayout->addWidget(applyButton);

	inputLayout->addWidget(gssL, 0, 0);
	inputLayout->addWidget(m_gssUrl, 0, 1);
	inputLayout->addWidget(modeL, 1, 0);
	inputLayout->addWidget(m_mode, 1, 1);

	typeLayout->addWidget(m_msgTypeInfo);
	typeLayout->addWidget(m_msgTypeDetailed);
	typeLayout->addWidget(m_msgTypeWarning);
	typeLayout->addWidget(m_msgTypeError);
	typeLayout->addWidget(m_msgTypeMsgIn);
	typeLayout->addWidget(m_msgTypeMsgOut);

	// Connect signals
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
}

void LogModeSetter::saveSettings(QSettings& _settings) {
	_settings.setValue("LogModeSetter.GSSUrl", m_gssUrl->text());

	_settings.setValue("LogModeSetter.Detailed", m_msgTypeDetailed->isChecked());
	_settings.setValue("LogModeSetter.Info", m_msgTypeInfo->isChecked());
	_settings.setValue("LogModeSetter.Warning", m_msgTypeWarning->isChecked());
	_settings.setValue("LogModeSetter.Error", m_msgTypeError->isChecked());
	_settings.setValue("LogModeSetter.Message.In", m_msgTypeMsgIn->isChecked());
	_settings.setValue("LogModeSetter.Message.Out", m_msgTypeMsgOut->isChecked());
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

	ot::LogModeManager newMode;

	if (m_mode->currentText() == LMS_Mode_Global) {
		newMode.setGlobalLogFlags(newFlags);
	}

	m_root->setEnabled(false);

	std::thread worker(&LogModeSetter::sendWorker, this, gssUrl, newMode);
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

void LogModeSetter::sendWorker(std::string _gss, ot::LogModeManager _newMode) {
	if (_newMode.getGlobalLogFlagsSet()) {
		this->stopWorker(this->sendGlobalMode(_gss, _newMode.getGlobalLogFlags()));
	}
	else {
		OT_LOG_E("Unsupported mode");
		this->stopWorker(false);
	}
}

bool LogModeSetter::sendGlobalMode(const std::string& _gss, const ot::LogFlags& _flags) {
	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_SetGlobalLogFlags, doc.GetAllocator()), doc.GetAllocator());

	ot::JsonArray flagsArr;
	ot::addLogFlagsToJsonArray(_flags, flagsArr, doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_Flags, flagsArr, doc.GetAllocator());

	std::string response;
	if (!ot::msg::send("", _gss, ot::EXECUTE, doc.toJson(), response, 30000, false, false)) {
		OT_LOG_E("Failed to send request");
		return false;
	}
	if (response != OT_ACTION_RETURN_VALUE_OK) {
		OT_LOG_E("Invalid response: \"" + response + "\"");
		return false;
	}
	else {
		return true;
	}
}