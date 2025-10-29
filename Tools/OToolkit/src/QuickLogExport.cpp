// @otlicense

//! @file QuickLogExport.cpp
//! @author Alexander Kuester (alexk95)
//! @date August 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OToolkit header
#include "AppBase.h"
#include "QuickLogExport.h"
#include "OToolkitAPI/OToolkitAPI.h"

// OpenTwin header
#include "OTWidgets/Label.h"
#include "OTWidgets/CheckBox.h"
#include "OTWidgets/ComboBox.h"
#include "OTWidgets/PushButton.h"

// Qt header
#include <QtCore/qfile.h>
#include <QtWidgets/qlayout.h>
#include <QtWidgets/qfiledialog.h>

#define QLE_USE_ALL "< All >"

#define QLE_LOG(___message) OTOOLKIT_LOG("Quick Log Export", ___message)
#define QLE_LOGW(___message) OTOOLKIT_LOGW("Quick Log Export", ___message)
#define QLE_LOGE(___message) OTOOLKIT_LOGE("Quick Log Export", ___message)

QuickLogExport::QuickLogExport(const std::list<ot::LogMessage>& _messages) 
	: m_messages(_messages)
{
	// Create layouts
	QVBoxLayout* centralLayout = new QVBoxLayout(this);
	QGridLayout* inputLayout = new QGridLayout;
	QHBoxLayout* btnLayout = new QHBoxLayout;

	// Create controls
	ot::Label* userL = new ot::Label("User:");
	m_user = new ot::ComboBox;

	ot::Label* projectL = new ot::Label("Project:");
	m_project = new ot::ComboBox;

	m_autoClose = new ot::CheckBox("Auto Close");
	m_autoClose->setToolTip("If enabled the OToolkit will be closed after a successful export.");

	ot::PushButton* btnExport = new ot::PushButton("Export");
	ot::PushButton* btnCancel = new ot::PushButton("Cancel");

	// Initialize data
	QStringList userList({ QLE_USE_ALL });
	QStringList projectList({ QLE_USE_ALL });

	for (const ot::LogMessage& msg : _messages) {
		bool userFound = false;
		bool projectFound = false;

		for (const QString& str : userList) {
			if (str == QString::fromStdString(msg.getUserName())) {
				userFound = true;
				break;
			}
		}
		if (!userFound) userList.push_back(QString::fromStdString(msg.getUserName()));

		for (const QString& str : projectList) {
			if (str == QString::fromStdString(msg.getProjectName())) {
				projectFound = true;
				break;
			}
		}
		if (!projectFound) projectList.push_back(QString::fromStdString(msg.getProjectName()));
	}

	m_user->addItems(userList);
	m_project->addItems(projectList);

	otoolkit::SettingsRef settings = AppBase::instance()->createSettingsInstance();
	m_autoClose->setChecked(settings->value("QuickLogExport.AutoClose", false).toBool());

	QString iniUser = settings->value("QuickLogExport.User", QLE_USE_ALL).toString();
	QString iniProj = settings->value("QuickLogExport.Proj", QLE_USE_ALL).toString();

	bool userFound = false;
	bool projectFound = false;

	for (const QString& user : userList) {
		if (user == iniUser) {
			userFound = true;
			break;
		}
	}
	for (const QString& proj : projectList) {
		if (proj == iniProj) {
			projectFound = true;
			break;
		}
	}

	if (userFound) m_user->setCurrentText(iniUser);
	else m_user->setCurrentText(QLE_USE_ALL);

	if (projectFound) m_project->setCurrentText(iniProj);
	else m_project->setCurrentText(QLE_USE_ALL);

	// Setup layouts
	centralLayout->addLayout(inputLayout);
	centralLayout->addStretch(1);
	centralLayout->addLayout(btnLayout);

	inputLayout->addWidget(userL, 0, 0);
	inputLayout->addWidget(m_user, 0, 1);
	inputLayout->addWidget(projectL, 1, 0);
	inputLayout->addWidget(m_project, 1, 1);
	inputLayout->addWidget(m_autoClose, 2, 1);

	btnLayout->addStretch(1);
	btnLayout->addWidget(btnExport);
	btnLayout->addWidget(btnCancel);

	// Setup window
	this->setWindowTitle("Quick Log Export");
	this->setWindowIcon(AppBase::instance()->windowIcon());

	// Connect signals
	this->connect(btnExport, &ot::PushButton::clicked, this, &QuickLogExport::slotExport);
	this->connect(btnCancel, &ot::PushButton::clicked, this, &QuickLogExport::closeCancel);
}

QuickLogExport::~QuickLogExport() {

}

bool QuickLogExport::isAutoClose(void) {
	return m_autoClose->isChecked();
}

void QuickLogExport::slotExport(void) {
	otoolkit::SettingsRef settings = AppBase::instance()->createSettingsInstance();
	QString fn = QFileDialog::getSaveFileName(this, "Export Log Messages", settings->value("Logging.LastExportedFile", "").toString(), "OpenTwin Log (*.otlog.json)");
	if (fn.isEmpty()) return;

	QFile f(fn);
	if (!f.open(QIODevice::WriteOnly)) {
		QLE_LOGE("Failed to open file for writing. File: \"" + fn + "\"");
		return;
	}

	ot::JsonDocument doc(rapidjson::kArrayType);

	for (const ot::LogMessage& msg : m_messages) {
		if (this->messageOkForExport(msg)) {
			ot::JsonObject msgObj;
			msg.addToJsonObject(msgObj, doc.GetAllocator());
			doc.PushBack(msgObj, doc.GetAllocator());
		}
	}

	f.write(QByteArray::fromStdString(doc.toJson()));
	f.close();

	settings->setValue("Logging.LastExportedFile", fn);
	settings->setValue("QuickLogExport.User", m_user->currentText());
	settings->setValue("QuickLogExport.Proj", m_project->currentText());

	QLE_LOG("Log Messages successfully exported to file \"" + fn + "\"");

	this->closeOk();
}

bool QuickLogExport::messageOkForExport(const ot::LogMessage& _message) {
	return (m_user->currentText() == QLE_USE_ALL || m_user->currentText().toStdString() == _message.getUserName()) &&
		(m_project->currentText() == QLE_USE_ALL || m_project->currentText().toStdString() == _message.getProjectName());
	
}