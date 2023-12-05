//! @file ConnectToLoggerDialog.cpp
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

#include "ConnectToLoggerDialog.h"
#include "AppBase.h"

#include "OTCommunication/Msg.h"
#include "OTCommunication/ActionTypes.h"		// action member and types definition

#include <QtCore/qsettings.h>
#include <QtWidgets/qlayout.h>
#include <QtWidgets/qlabel.h>
#include <QtWidgets/qlineedit.h>
#include <QtWidgets/qpushbutton.h>
#include <QtWidgets/qmessagebox.h>

#include <string>
#include <thread>

#define DIA_LOG(___message) OTOOLKIT_LOG("Logger Service Connector", ___message)
#define DIA_LOGW(___message) OTOOLKIT_LOGW("Logger Service Connector", ___message)
#define DIA_LOGE(___message) OTOOLKIT_LOGE("Logger Service Connector", ___message)

ConnectToLoggerDialog::ConnectToLoggerDialog() : m_success(false) {
	otoolkit::SettingsRef settings = AppBase::instance()->createSettingsInstance();

	// Create layouts
	m_centralLayout = new QVBoxLayout(this);
	m_inputLayout = new QGridLayout;
	m_buttonLayout = new QHBoxLayout;
	
	// Create controls
	m_urlL = new QLabel("URL:");
	m_url = new QLineEdit(settings->value("LoggerServiceURL", "").toString());

	m_btnConnect = new QPushButton("Connect");
	m_btnCancel = new QPushButton("Cancel");

	// Setup layouts
	m_centralLayout->addLayout(m_inputLayout, 1);
	m_centralLayout->addLayout(m_buttonLayout);

	m_inputLayout->addWidget(m_urlL, 0, 0);
	m_inputLayout->addWidget(m_url, 0, 1);
	m_inputLayout->setColumnStretch(1, 1);

	m_buttonLayout->addStretch(1);
	m_buttonLayout->addWidget(m_btnConnect);
	m_buttonLayout->addWidget(m_btnCancel);

	// Setup window
	setWindowFlags(windowFlags() & ~(Qt::WindowContextHelpButtonHint));
	setWindowTitle("Connect to Logger");
	setWindowIcon(AppBase::instance()->windowIcon());

	// Connect signals
	connect(m_btnConnect, &QPushButton::clicked, this, &ConnectToLoggerDialog::slotConnect);
	connect(m_btnCancel, &QPushButton::clicked, this, &ConnectToLoggerDialog::slotCancel);
}

ConnectToLoggerDialog::~ConnectToLoggerDialog() {

}

QString ConnectToLoggerDialog::loggerServiceUrl(void) const {
	return m_url->text();
}

void ConnectToLoggerDialog::queueConnectRequest(void) {
	QMetaObject::invokeMethod(this, &ConnectToLoggerDialog::slotConnect, Qt::QueuedConnection);
}

void ConnectToLoggerDialog::queueRecenterRequest(void) {
	QMetaObject::invokeMethod(this, &ConnectToLoggerDialog::slotRecenter, Qt::QueuedConnection);
}

void ConnectToLoggerDialog::slotCancel(void) {
	close();
}

void ConnectToLoggerDialog::slotConnect(void) {
	if (m_url->text().isEmpty()) {
		QMessageBox msg(QMessageBox::Warning, "Invalid input", "Please specify a valid URL", QMessageBox::Ok, this);
		msg.exec();
		return;
	}

	setEnabled(false);

	std::thread t(&ConnectToLoggerDialog::worker, this, m_url->text());
	t.detach();
}

void ConnectToLoggerDialog::slotPingFail(void) {
	QMessageBox msg(QMessageBox::Warning, "Connection error", "Failed to ping Logger Service at \"" + m_url->text() + "\". Please ensure that the Logger Service is running and the specified url is correct.", QMessageBox::Ok, this);
	msg.exec();

	setEnabled(true);
}

void ConnectToLoggerDialog::slotRegisterFail(void) {
	QMessageBox msg(QMessageBox::Warning, "Connection error", "Failed to register at Logger Service at \"" + m_url->text() + "\".", QMessageBox::Ok, this);
	msg.exec();

	setEnabled(true);
}

void ConnectToLoggerDialog::slotDone(void) {
	otoolkit::SettingsRef settings = AppBase::instance()->createSettingsInstance();
	settings->setValue("LoggerServiceURL", m_url->text());

	m_success = true;
	this->close();
}

void ConnectToLoggerDialog::slotRecenter(void) {
	this->centerOnParent(otoolkit::api::getGlobalInterface()->rootWidget());
}

void ConnectToLoggerDialog::worker(QString _url) {
	DIA_LOG("Connection worker started (url = \"" + _url + "\")...");

	std::string response;
	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_Ping, doc.GetAllocator()), doc.GetAllocator());
	if (!ot::msg::send("", _url.toStdString(), ot::EXECUTE, doc.toJson(), response)) {
		DIA_LOGE("Failed to send ping message to Logger Service at \"" + _url + "\"");
		QMetaObject::invokeMethod(this, &ConnectToLoggerDialog::slotPingFail, Qt::QueuedConnection);
		return;
	}
	else if (response != OT_ACTION_CMD_Ping) {
		DIA_LOGE("Invalid ping response from Logger Service at \"" + _url + "\"");
		QMetaObject::invokeMethod(this, &ConnectToLoggerDialog::slotPingFail, Qt::QueuedConnection);
		return;
	}

	DIA_LOG("Connection successfully checked to Logger Service at \"" + _url + "\". Sending registration request...");

	response.clear();

	ot::JsonDocument registerDoc;
	registerDoc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_RegisterNewService, registerDoc.GetAllocator()), registerDoc.GetAllocator());
	registerDoc.AddMember(OT_ACTION_PARAM_SERVICE_URL, ot::JsonString(AppBase::instance()->url().toStdString(), registerDoc.GetAllocator()), registerDoc.GetAllocator());
	
	if (!ot::msg::send("", _url.toStdString(), ot::EXECUTE, registerDoc.toJson(), response)) {
		DIA_LOGE("Failed to send registration request to Logger Service at \"" + _url + "\"");
		QMetaObject::invokeMethod(this, &ConnectToLoggerDialog::slotRegisterFail, Qt::QueuedConnection);
		return;
	}
	
	ot::JsonDocument responseDoc;
	responseDoc.fromJson(response);
	if (!responseDoc.IsArray()) {
		DIA_LOGE("Invalid registration response from Logger Service at \"" + _url + "\"");
		QMetaObject::invokeMethod(this, &ConnectToLoggerDialog::slotRegisterFail, Qt::QueuedConnection);
		return;
	}

	DIA_LOG("Registration successful at Logger Service \"" + _url + "\". Retreiving currently buffered log messages...");

	for (rapidjson::SizeType i = 0; i < responseDoc.Size(); i++) {
		if (responseDoc[i].IsObject()) {
			ot::ConstJsonObject msgObj = responseDoc.constRef()[i].GetObject();
			ot::LogMessage msg;
			try {
				msg.setFromJsonObject(msgObj);
				m_messageBuffer.push_back(msg);
			}
			catch (const std::exception& _e) {
				OT_LOG_W(_e.what());
			}
			catch (...) {
				OT_LOG_E("Unknown error");
			}
		}
	}

	DIA_LOG("Connection worker success");

	QMetaObject::invokeMethod(this, &ConnectToLoggerDialog::slotDone, Qt::QueuedConnection);
}
