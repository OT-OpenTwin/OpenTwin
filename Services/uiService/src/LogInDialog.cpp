//! @file LogInDialog.cpp
//! @author Alexander Kuester (alexk95)
//! @date April 2024
// ###########################################################################################################################################################################################################################################################################################################################

// Frontend header
#include "AppBase.h"
#include "LogInDialog.h"

// OpenTwin header
#include "OTWidgets/Label.h"
#include "OTWidgets/LineEdit.h"
#include "OTWidgets/ComboBox.h"
#include "OTWidgets/PushButton.h"
#include "OTWidgets/IconManager.h"
#include "OTWidgets/ImagePreview.h"

// Qt header
#include <QtCore/qjsonarray.h>
#include <QtCore/qjsonobject.h>
#include <QtCore/qjsondocument.h>
#include <QtWidgets/qlayout.h>

#define LOG_IN_RESTOREDPASSWORD_PLACEHOLDER "******"

LogInDialog::LogInDialog() 
	: m_state(LogInStateFlag::NoState)
{
	using namespace ot;

	// Create layouts
	QVBoxLayout* centralLayout = new QVBoxLayout(this);
	QHBoxLayout* imageViewLayout = new QHBoxLayout;
	QGridLayout* inputLayout = new QGridLayout;
	QGridLayout* buttonLayout = new QGridLayout;
	QHBoxLayout* registerLayout = new QHBoxLayout;
	
	// Create controls
	ImagePreview* titleImageView = new ImagePreview(IconManager::getPixmap("Images/OpenTwinIcon.png").scaled(QSize(172, 172), Qt::KeepAspectRatio).toImage());
	titleImageView->setObjectName("LogInDialogImageView");
	titleImageView->setEnableResizing(false);
	titleImageView->setImageMargins(QMargins(0, 0, 0, 0));

	Label* gssLabel = new Label("Global Session Service:");
	m_gss = new ComboBox;
	m_gss->setEditable(false);

	Label* usernameLabel = new Label("Username:");
	m_username = new LineEdit;
	
	Label* passwordLabel = new Label("Password:");
	m_password = new LineEdit;
	m_password->setEchoMode(QLineEdit::Password);
	
	Label* spacerLabel1 = new Label;
	Label* spacerLabel2 = new Label;

	PushButton* logInButton = new PushButton("Log In");
	PushButton* exitButton = new PushButton("Exit");

	// Initialize data
	std::shared_ptr<QSettings> settings = AppBase::instance()->createSettingsInstance();

	m_username->setText(settings->value("LastUsername", QString()).toString());
	m_lastPassword = settings->value("LastPassword", QString()).toString();

	if (!m_lastPassword.isEmpty()) {
		m_password->setText(LOG_IN_RESTOREDPASSWORD_PLACEHOLDER);
		m_state |= LogInStateFlag::RestoredPassword;
	}

	this->initializeGssData(settings);
	this->updateGssOptions();

	// Setup layouts
	imageViewLayout->addStretch(1);
	imageViewLayout->addWidget(titleImageView);
	imageViewLayout->addStretch(1);

	inputLayout->addWidget(gssLabel, 0, 0);
	inputLayout->addWidget(m_gss, 0, 1);
	inputLayout->addWidget(usernameLabel, 1, 0);
	inputLayout->addWidget(m_username, 1, 1);
	inputLayout->addWidget(passwordLabel, 2, 0);
	inputLayout->addWidget(m_password, 2, 1);
	
	buttonLayout->addWidget(logInButton, 0, 0);
	buttonLayout->addWidget(exitButton, 1, 0);

	centralLayout->addWidget(spacerLabel1);
	centralLayout->addLayout(imageViewLayout);
	centralLayout->addStretch(1);
	centralLayout->addLayout(inputLayout);
	centralLayout->addWidget(spacerLabel2);
	centralLayout->addLayout(buttonLayout);
	centralLayout->addLayout(registerLayout);

	// ... Setup optional inputs (this section may be disabled when a registration is not allowed)
	Label* newUserLabel = new Label("Create new account");
	newUserLabel->setObjectName("LogInDialogRegisterLabel");
	newUserLabel->setMargin(2);
	registerLayout->addStretch(1);
	registerLayout->addWidget(newUserLabel);
	registerLayout->addStretch(1);
	this->connect(newUserLabel, &Label::mousePressed, this, &LogInDialog::slotNewUser);
	// ...

	// Setup window
	this->setObjectName("LogInDialog");
	this->setWindowTitle("OpenTwin Log-In");
	this->setWindowFlags(Qt::Dialog | Qt::WindowType::FramelessWindowHint);
	this->setFixedSize(300, 450);

	// Connect signals
	this->connect(logInButton, &PushButton::clicked, this, &LogInDialog::slotLogIn);
	this->connect(exitButton, &PushButton::clicked, this, &LogInDialog::slotCancel);
	this->connect(m_gss, &ComboBox::currentTextChanged, this, &LogInDialog::slotGSSChanged);
	this->connect(m_password, &LineEdit::textChanged, this, &LogInDialog::slotPasswordChanged);

}

LogInDialog::~LogInDialog() {

}

void LogInDialog::resizeEvent(QResizeEvent* _event) {
	ot::Dialog::resizeEvent(_event);
	m_username->setText(QString::number(this->width()) + "; " + QString::number(this->height()));
}

// ###########################################################################################################################################################################################################################################################################################################################

// Private slots

void LogInDialog::slotLogIn(void) {
	
}

void LogInDialog::slotCancel(void) {
	this->close(ot::Dialog::Cancel);
}

void LogInDialog::slotNewUser(void) {

}

void LogInDialog::slotGSSChanged(void) {

}

void LogInDialog::slotPasswordChanged(void) {
	if (!(m_state & LogInStateFlag::RestoredPassword)) return;

	QString txt = m_password->text();
	QString newTxt;
	int placeholderLength = QString(LOG_IN_RESTOREDPASSWORD_PLACEHOLDER).length();
	if (txt.length() < placeholderLength) {
		newTxt = "";
		for (const QChar& c : txt) {
			if (c != '*') newTxt.append(c);
		}
	}
	else if (txt.length() == placeholderLength) {
		newTxt = "";
		for (const QChar& c : txt) {
			if (c != '*') newTxt.append(c);
		}
	}
	else {
		int ct = 0;
		newTxt = "";
		for (const QChar& c : txt) {
			if (c == '*') ct++;
			if (c != '*' || ct > placeholderLength) {
				newTxt.append(c);
			}
		}
	}

	m_state &= (~LogInStateFlag::RestoredPassword);
	m_password->setText(newTxt);
}

// ###########################################################################################################################################################################################################################################################################################################################

// Private helper

void LogInDialog::initializeGssData(std::shared_ptr<QSettings> _settings) {
	QString lastSessionService = _settings->value("SessionServiceURL", "").toString();
	QByteArray sessionServiceJSON = _settings->value("SessionServiceJSON", QByteArray()).toByteArray();

	QStringList gssOptions;
	if (!sessionServiceJSON.isEmpty()) {
		QJsonDocument doc = QJsonDocument::fromJson(sessionServiceJSON);
		if (doc.isArray()) {
			QJsonArray arr = doc.array();
			for (int i = 0; i < arr.count(); i++) {
				if (!arr[i].isObject()) {
					OT_LOG_EA("GSS data entry is not an object");
					continue;
				}
				QJsonObject obj = arr[i].toObject();
				if (!obj.contains("Name")) {
					OT_LOG_EA("JSON member \"Name\" is missing");
					continue;
				}
				if (!obj.contains("IP")) {
					OT_LOG_EA("JSON member \"Name\" is missing");
					continue;
				}
				if (!obj.contains("Port")) {
					OT_LOG_EA("JSON member \"Name\" is missing");
					continue;
				}
				if (!obj["Name"].isString()) {
					OT_LOG_EA("JSON member \"Name\" is not a string");
					continue;
				}
				if (!obj["IP"].isString()) {
					OT_LOG_EA("JSON member \"Name\" is not a string");
					continue;
				}
				if (!obj["Port"].isString()) {
					OT_LOG_EA("JSON member \"Name\" is not a string");
					continue;
				}

				LogInGSSEntry newEntry(obj["Name"].toString(), obj["IP"].toString(), obj["Port"].toString());
				m_gssData.push_back(newEntry);

				if (newEntry.getName() == lastSessionService) {
					m_gss->setCurrentText(newEntry.getName());
				}
			}
		}
		else {
			OT_LOG_EA("SessionService list is not a JSON array");
		}
	}
}

void LogInDialog::updateGssOptions(void) {
	QStringList options;
	for (const LogInGSSEntry& entry : m_gssData) {
		options.append(entry.getDisplayText());
	}
	options.append("< Edit >");

	m_gss->clear();
	m_gss->addItems(options);

	if (m_gss->currentText().isEmpty() && !options.isEmpty()) {
		m_gss->setCurrentText(options.first());
	}
}