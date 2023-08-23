#include "ConnectToLoggerDialog.h"
#include "AppBase.h"

#include "OpenTwinCommunication/Msg.h"
#include "OpenTwinCommunication/ActionTypes.h"		// action member and types definition

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

ConnectToLoggerDialog::ConnectToLoggerDialog(bool _isAutoConnect) : m_success(false) {
	QSettings s("OpenTwin", APP_BASE_APP_NAME);

	// Create layouts
	m_centralLayout = new QVBoxLayout(this);
	m_inputLayout = new QGridLayout;
	m_buttonLayout = new QHBoxLayout;
	
	// Create controls
	m_urlL = new QLabel("URL:");
	m_url = new QLineEdit(s.value("LoggerServiceURL", "").toString());

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

	if (_isAutoConnect) {
		QMetaObject::invokeMethod(this, &ConnectToLoggerDialog::slotConnect, Qt::QueuedConnection);
	}
}

ConnectToLoggerDialog::~ConnectToLoggerDialog() {

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
	QSettings s("OpenTwin", APP_BASE_APP_NAME);
	s.setValue("LoggerServiceURL", m_url->text());

	m_success = true;
	close();
}

void ConnectToLoggerDialog::worker(QString _url) {
	DIA_LOG("Connection worker started (url = \"" + _url + "\")...");

	std::string response;
	OT_rJSON_createDOC(doc);
	ot::rJSON::add(doc, OT_ACTION_MEMBER, OT_ACTION_CMD_Ping);
	if (!ot::msg::send("", _url.toStdString(), ot::EXECUTE, ot::rJSON::toJSON(doc), response)) {
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

	OT_rJSON_createDOC(registerDoc);
	ot::rJSON::add(registerDoc, OT_ACTION_MEMBER, OT_ACTION_CMD_RegisterNewService);
	ot::rJSON::add(registerDoc, OT_ACTION_PARAM_SERVICE_URL, AppBase::instance()->url().toStdString());

	if (!ot::msg::send("", _url.toStdString(), ot::EXECUTE, ot::rJSON::toJSON(registerDoc), response)) {
		DIA_LOGE("Failed to send registration request to Logger Service at \"" + _url + "\"");
		QMetaObject::invokeMethod(this, &ConnectToLoggerDialog::slotRegisterFail, Qt::QueuedConnection);
		return;
	}
	
	OT_rJSON_parseDOC(responseDoc, response.c_str());
	if (!responseDoc.IsArray()) {
		DIA_LOGE("Invalid registration response from Logger Service at \"" + _url + "\"");
		QMetaObject::invokeMethod(this, &ConnectToLoggerDialog::slotRegisterFail, Qt::QueuedConnection);
		return;
	}

	DIA_LOG("Registration successful at Logger Service \"" + _url + "\". Retreiving currently buffered log messages...");

	for (rapidjson::SizeType i = 0; i < responseDoc.Size(); i++) {
		if (responseDoc[i].IsObject()) {
			OT_rJSON_val msgObj = responseDoc[i].GetObject();
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
