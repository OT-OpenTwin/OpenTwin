//! @file ConnectToLoggerDialog.cpp
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OToolkit header
#include "ConnectToLoggerDialog.h"
#include "AppBase.h"

// OpenTwin header
#include "OTWidgets/IconManager.h"
#include "OTCommunication/Msg.h"
#include "OTCommunication/ActionTypes.h"		// action member and types definition
#include "OTWidgets/IconManager.h"
#include "OTWidgets/Positioning.h"

// Qt header
#include <QtCore/qsettings.h>
#include <QtWidgets/qlayout.h>
#include <QtWidgets/qlabel.h>
#include <QtWidgets/qlineedit.h>
#include <QtWidgets/qpushbutton.h>
#include <QtWidgets/qmessagebox.h>

// std header
#include <string>
#include <thread>

#define DIA_LOG(___message) OTOOLKIT_LOG("Logger Service Connector", ___message)
#define DIA_LOGW(___message) OTOOLKIT_LOGW("Logger Service Connector", ___message)
#define DIA_LOGE(___message) OTOOLKIT_LOGE("Logger Service Connector", ___message)

ConnectToLoggerDialog::ConnectToLoggerDialog()
	: m_workerRunning(false), m_stopWorker(false)
{
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
	setWindowIcon(ot::IconManager::getApplicationIcon());

	// Connect signals
	connect(m_btnConnect, &QPushButton::clicked, this, &ConnectToLoggerDialog::slotConnect);
	connect(m_btnCancel, &QPushButton::clicked, this, &ConnectToLoggerDialog::slotCancel);
}

ConnectToLoggerDialog::~ConnectToLoggerDialog() {
	this->stopWorker();
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

bool ConnectToLoggerDialog::mayCloseDialogWindow(void) const {
	return !m_workerRunning;
}

void ConnectToLoggerDialog::slotCancel(void) {
	this->close(ot::Dialog::Cancel);
}

void ConnectToLoggerDialog::slotConnect(void) {
	if (m_url->text().isEmpty()) {
		QMessageBox msg(QMessageBox::Warning, "Invalid input", "Please specify a valid URL", QMessageBox::Ok, this);
		msg.exec();
		return;
	}

	this->setEnabled(false);
	
	m_workerRunning = true;
	m_stopWorker = false;

	std::thread t(&ConnectToLoggerDialog::worker, this, m_url->text());
	t.detach();
}

void ConnectToLoggerDialog::slotPingFail(void) {
	QMessageBox msg(QMessageBox::Warning, "Connection error", "Failed to ping Logger Service at \"" + m_url->text() + "\". Please ensure that the Logger Service is running and the specified url is correct.", QMessageBox::Ok, this);
	msg.exec();

	this->setEnabled(true);
}

void ConnectToLoggerDialog::slotRegisterFail(void) {
	QMessageBox msg(QMessageBox::Warning, "Connection error", "Failed to register at Logger Service at \"" + m_url->text() + "\".", QMessageBox::Ok, this);
	msg.exec();

	this->setEnabled(true);
}

void ConnectToLoggerDialog::slotDone(void) {
	otoolkit::SettingsRef settings = AppBase::instance()->createSettingsInstance();
	settings->setValue("LoggerServiceURL", m_url->text());

	if (!(this->getDialogState() & ot::Dialog::DialogState::Closing)) {
		this->close(ot::Dialog::Ok);
	}
}

void ConnectToLoggerDialog::slotRecenter(void) {
	ot::Positioning::centerWidgetOnParent(otoolkit::api::getGlobalInterface()->rootWidget(), static_cast<QWidget*>(this));
}

void ConnectToLoggerDialog::slotEnable(void) {
	this->setEnabled(true);
}

void ConnectToLoggerDialog::worker(QString _url) {
	try {
		DIA_LOG("Connection worker started (url = \"" + _url + "\")...");

		ot::JsonDocument doc;
		doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_Ping, doc.GetAllocator()), doc.GetAllocator());

		std::string response;
		bool sendResult = ot::msg::send("", _url.toStdString(), ot::EXECUTE, doc.toJson(), response, 3000, false, false);

		if (m_stopWorker) {
			m_workerRunning = false;
			return;
		}

		if (!sendResult) {
			DIA_LOGE("Failed to send ping message to Logger Service at \"" + _url + "\"");

			m_workerRunning = false;
			QMetaObject::invokeMethod(this, &ConnectToLoggerDialog::slotPingFail, Qt::QueuedConnection);

			return;
		}
		else if (response != OT_ACTION_CMD_Ping) {
			DIA_LOGE("Invalid ping response from Logger Service at \"" + _url + "\"");

			m_workerRunning = false;
			QMetaObject::invokeMethod(this, &ConnectToLoggerDialog::slotPingFail, Qt::QueuedConnection);
			
			return;
		}

		DIA_LOG("Connection successfully checked to Logger Service at \"" + _url + "\". Sending registration request...");

		response.clear();

		ot::JsonDocument registerDoc;
		registerDoc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_RegisterNewService, registerDoc.GetAllocator()), registerDoc.GetAllocator());
		registerDoc.AddMember(OT_ACTION_PARAM_SERVICE_URL, ot::JsonString(AppBase::instance()->url().toStdString(), registerDoc.GetAllocator()), registerDoc.GetAllocator());

		sendResult = ot::msg::send("", _url.toStdString(), ot::EXECUTE, registerDoc.toJson(), response, 3000, false, false);

		if (m_stopWorker) {
			m_workerRunning = false;
			return;
		}

		if (!sendResult) {
			DIA_LOGE("Failed to send registration request to Logger Service at \"" + _url + "\"");

			m_workerRunning = false;
			QMetaObject::invokeMethod(this, &ConnectToLoggerDialog::slotRegisterFail, Qt::QueuedConnection);
			
			return;
		}

		ot::JsonDocument responseDoc;
		responseDoc.fromJson(response);
		if (!responseDoc.IsArray()) {
			DIA_LOGE("Invalid registration response from Logger Service at \"" + _url + "\"");

			m_workerRunning = false;
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

		m_workerRunning = false;
		QMetaObject::invokeMethod(this, &ConnectToLoggerDialog::slotDone, Qt::QueuedConnection);
	}
	catch (const std::exception& _e) {
		OT_LOG_E(_e.what());
		m_workerRunning = false;
		QMetaObject::invokeMethod(this, &ConnectToLoggerDialog::slotEnable, Qt::QueuedConnection);
	}
	catch (...) {
		OT_LOG_E("Unknown error occured");
		m_workerRunning = false;
		QMetaObject::invokeMethod(this, &ConnectToLoggerDialog::slotEnable, Qt::QueuedConnection);
	}
}

void ConnectToLoggerDialog::stopWorker(void) {
	if (m_workerRunning) {
		m_stopWorker = true;
		while (m_workerRunning) {
			QEventLoop loop;
			loop.processEvents(QEventLoop::ProcessEventsFlag::ExcludeUserInputEvents);
		}
	}
}
