//! @file LogInDialog.cpp
//! @author Alexander Kuester (alexk95)
//! @date April 2024
// ###########################################################################################################################################################################################################################################################################################################################

// Frontend header
#include "AppBase.h"
#include "LogInDialog.h"
#include "LogInGSSEditDialog.h"

// OpenTwin header
#include "OTWidgets/Label.h"
#include "OTWidgets/CheckBox.h"
#include "OTWidgets/LineEdit.h"
#include "OTWidgets/ComboBox.h"
#include "OTWidgets/PushButton.h"
#include "OTWidgets/IconManager.h"
#include "OTWidgets/Positioning.h"
#include "OTWidgets/ImagePreview.h"
#include "OTCommunication/Msg.h"
#include "OTCommunication/ActionTypes.h"

// Qt header
#include <QtCore/qjsonarray.h>
#include <QtCore/qjsonobject.h>
#include <QtCore/qjsondocument.h>
#include <QtGui/qevent.h>
#include <QtWidgets/qlayout.h>
#include <QtWidgets/qtooltip.h>
#include <QtWidgets/qmessagebox.h>

// std header
#include <thread>

#define LOG_IN_RESTOREDPASSWORD_PLACEHOLDER "******"

#define MAX_LOGIN_ATTEMPTS 5

#define EDIT_GSS_TEXT "< Edit >"

#define TOGGLE_MODE_LABEL_SwitchToLogIn "Switch to login"
#define TOGGLE_MODE_LABEL_SwitchToRegister "Switch to registration"

LogInDialog::LogInDialog() 
	: m_state(LogInStateFlag::NoState), m_toggleModeLabel(nullptr), m_logInAttempt(0)
{
	using namespace ot;

	// Create layouts
	QVBoxLayout* centralLayout = new QVBoxLayout(this);
	QHBoxLayout* imageViewLayout = new QHBoxLayout;
	QGridLayout* inputLayout = new QGridLayout;
	QGridLayout* buttonLayout = new QGridLayout;
	QHBoxLayout* registerLayout = new QHBoxLayout;
	
	// Create controls
	ImagePreview* titleImageView = new ImagePreview(IconManager::getPixmap("Images/OpenTwinIcon.png").scaled(QSize(192, 192), Qt::KeepAspectRatio).toImage());
	titleImageView->setObjectName("LogInDialogImageView");
	titleImageView->setEnableResizing(false);
	titleImageView->setImageMargins(QMargins(0, 0, 0, 0));
	titleImageView->setFixedSize(titleImageView->image().size());

	Label* gssLabel = new Label("Global Session Service:");
	m_gss = new ComboBox;
	m_gss->setEditable(false);

	Label* usernameLabel = new Label("Username:");
	m_username = new LineEdit;
	
	Label* passwordLabel = new Label("Password:");
	m_password = new LineEdit;
	m_password->setEchoMode(QLineEdit::Password);
	
	m_confirmPasswordLabel = new Label("Confirm password:");
	m_confirmPasswordLabel->setHidden(true);
	m_confirmPassword = new LineEdit;
	m_confirmPassword->setEchoMode(QLineEdit::Password);
	m_confirmPassword->setHidden(true);

	m_savePassword = new CheckBox("Save Password");
	m_savePassword->setObjectName("LogInDialogSavePassword");

	Label* spacerLabel1 = new Label;
	Label* spacerLabel2 = new Label;

	m_logInButton = new PushButton("Log In");
	m_registerButton = new PushButton("Register");
	m_registerButton->setHidden(true);
	m_exitButton = new PushButton("Exit");

	// Initialize data
	std::shared_ptr<QSettings> settings = AppBase::instance()->createSettingsInstance();

	m_username->setText(settings->value("LastUsername", QString()).toString());
	m_restoredPassword = settings->value("LastPassword", QString()).toString();
	m_savePassword->setChecked(settings->value("LastSavePassword", false).toBool());
	
	if (!m_restoredPassword.isEmpty()) {
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
	inputLayout->addWidget(m_confirmPasswordLabel, 3, 0);
	inputLayout->addWidget(m_confirmPassword, 3, 1);
	inputLayout->addWidget(m_savePassword, 4, 1);
	
	buttonLayout->addWidget(m_logInButton, 0, 0);
	buttonLayout->addWidget(m_registerButton, 1, 0);
	buttonLayout->addWidget(m_exitButton, 2, 0);

	centralLayout->addWidget(spacerLabel1);
	centralLayout->addLayout(imageViewLayout);
	centralLayout->addStretch(1);
	centralLayout->addLayout(inputLayout);
	centralLayout->addWidget(spacerLabel2);
	centralLayout->addLayout(buttonLayout);
	centralLayout->addLayout(registerLayout);

	// ... Setup optional inputs (this section may be disabled when a registration is not allowed)
	m_toggleModeLabel = new Label(TOGGLE_MODE_LABEL_SwitchToRegister);
	m_toggleModeLabel->setObjectName("LogInDialogRegisterLabel");
	m_toggleModeLabel->setMargin(4);
	registerLayout->addStretch(1);
	registerLayout->addWidget(m_toggleModeLabel);
	registerLayout->addStretch(1);
	this->connect(m_toggleModeLabel, &Label::mousePressed, this, &LogInDialog::slotToggleLogInMode);
	// ...

	// Setup window
	this->setObjectName("LogInDialog");
	this->setWindowTitle("OpenTwin Login");
	this->setWindowFlags(Qt::Dialog | Qt::WindowType::FramelessWindowHint);
	this->setFixedSize(350, 500);

	// Connect signals
	this->connect(m_logInButton, &PushButton::clicked, this, &LogInDialog::slotLogIn);
	this->connect(m_registerButton, &PushButton::clicked, this, &LogInDialog::slotRegister);
	this->connect(m_exitButton, &PushButton::clicked, this, &LogInDialog::slotCancel);
	this->connect(m_gss, &ComboBox::currentTextChanged, this, &LogInDialog::slotGSSChanged);
	this->connect(m_password, &LineEdit::textChanged, this, &LogInDialog::slotPasswordChanged);

}

LogInDialog::~LogInDialog() {

}

void LogInDialog::setControlsEnabled(bool _enabled) {
	m_gss->setEnabled(_enabled);
	m_username->setEnabled(_enabled);
	m_password->setEnabled(_enabled);
	m_confirmPassword->setEnabled(_enabled);
	m_savePassword->setEnabled(_enabled);
	m_toggleModeLabel->setEnabled(_enabled);
	m_logInButton->setEnabled(_enabled);
	m_registerButton->setEnabled(_enabled);
	m_exitButton->setEnabled(_enabled);
}

void LogInDialog::closeEvent(QCloseEvent* _event) {
	if (m_state & LogInStateFlag::WorkerRunning) {
		_event->ignore();
	}
	else {
		ot::Dialog::closeEvent(_event);
	}
}

// ###########################################################################################################################################################################################################################################################################################################################

// Private slots

void LogInDialog::slotLogIn(void) {
	OTAssert(!(m_state & LogInStateFlag::WorkerRunning), "Worker already running");

	m_loginData.clear();
	
	// Check user inputs
	LogInGSSEntry gssData = this->findCurrentGssEntry();
	if (!gssData.isValid()) {
		QToolTip::showText(this->mapToGlobal(m_gss->pos()), "Invalid Global Session Service", m_gss, QRect(), 3000);
		return;
	}

	if (m_username->text().isEmpty()) {
		QToolTip::showText(this->mapToGlobal(m_username->pos()), "No username provided", m_username, QRect(), 3000);
		return;
	}

	if (m_password->text().isEmpty()) {
		QToolTip::showText(this->mapToGlobal(m_password->pos()), "No password provided", m_password, QRect(), 3000);
		return;
	}

	m_logInAttempt++;

	// Run worker
	this->setControlsEnabled(false);

	m_loginData.setGss(gssData);

	m_state |= LogInStateFlag::WorkerRunning;

	std::thread worker(&LogInDialog::loginWorkerStart, this);
	worker.detach();
}

void LogInDialog::slotRegister(void) {
	OTAssert(!(m_state & LogInStateFlag::WorkerRunning), "Worker already running");

	m_loginData.clear();

	LogInGSSEntry gssData = this->findCurrentGssEntry();
	if (!gssData.isValid()) {
		QToolTip::showText(this->mapToGlobal(m_gss->pos()), "Invalid Global Session Service", m_gss, QRect(), 3000);
		return;
	}

	if (m_username->text().isEmpty()) {
		QToolTip::showText(this->mapToGlobal(m_username->pos()), "No username provided", m_username, QRect(), 3000);
		return;
	}

	if (m_password->text().isEmpty()) {
		QToolTip::showText(this->mapToGlobal(m_password->pos()), "No password provided", m_password, QRect(), 3000);
		return;
	}

	if (m_password->text().length() < 4) {
		QToolTip::showText(this->mapToGlobal(m_password->pos()), "Password is too short", m_password, QRect(), 3000);
		return;
	}

	if (m_password->text() != m_confirmPassword->text()) {
		QToolTip::showText(this->mapToGlobal(m_confirmPassword->pos()), "Confirm password does not match the password", m_confirmPassword, QRect(), 3000);
		return;
	}

	m_logInAttempt++;

	// Run worker
	this->setControlsEnabled(false);

	m_loginData.setGss(gssData);

	m_state |= LogInStateFlag::WorkerRunning;

	std::thread worker(&LogInDialog::registerWorkerStart, this);
	worker.detach();
}

void LogInDialog::slotCancel(void) {
	this->close(ot::Dialog::Cancel);
}

void LogInDialog::slotToggleLogInMode(void) {
	OTAssert(!(m_state & LogInStateFlag::WorkerRunning), "Worker running");

	if (m_state & LogInStateFlag::RegisterMode) {
		m_logInButton->setHidden(false);
		m_registerButton->setHidden(true);
		m_confirmPassword->setHidden(true);
		m_confirmPasswordLabel->setHidden(true);
		m_toggleModeLabel->setText(TOGGLE_MODE_LABEL_SwitchToRegister);
		m_state &= (~LogInStateFlag::RegisterMode);
	}
	else {
		m_logInButton->setHidden(true);
		m_registerButton->setHidden(false);
		m_confirmPassword->setHidden(false);
		m_confirmPasswordLabel->setHidden(false);
		m_toggleModeLabel->setText(TOGGLE_MODE_LABEL_SwitchToLogIn);
		m_state |= LogInStateFlag::RegisterMode;

		if (m_state & LogInStateFlag::RestoredPassword) {
			m_password->blockSignals(true);
			m_password->setText(QString());
			m_password->blockSignals(false);
		}
	}

	this->update();
}

void LogInDialog::slotGSSChanged(void) {
	if (m_gss->currentText() != EDIT_GSS_TEXT) return;

	LogInGSSEditDialog dialog(m_gssData);

	ot::Dialog::DialogResult result = dialog.showDialog();

	m_gss->blockSignals(true);
	
	if (result == ot::Dialog::Ok) {
		m_gssData = dialog.getEntries();
		this->updateGssOptions();
	}

	m_gss->setCurrentIndex(0);
	m_gss->blockSignals(false);
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

void LogInDialog::slotLogInSuccess(void) {
	m_state &= (~LogInStateFlag::WorkerRunning);
	this->saveUserSettings();
	this->close(ot::Dialog::Ok);
}

void LogInDialog::slotRegisterSuccess(void) {
	m_state &= (~LogInStateFlag::WorkerRunning);

	QMessageBox msgBox(QMessageBox::Information, "Registration", "The account was created successfully.", QMessageBox::Ok);
	msgBox.exec();

	m_confirmPassword->setText(QString());

	this->slotToggleLogInMode();
	this->setControlsEnabled(true);
}

void LogInDialog::slotWorkerError(WorkerError _error) {
	m_state &= (~LogInStateFlag::WorkerRunning);

	// Check for max login attempt
	if (m_logInAttempt >= MAX_LOGIN_ATTEMPTS) {
		this->close(ot::Dialog::Cancel);
		return;
	}

	// Create error message
	QString msg;

	if (m_state & LogInStateFlag::RegisterMode) {
		msg = "Registration failed:\n";
	}
	else {
		msg = "Login failed:\n";
	}


	switch (_error)
	{
	case WorkerError::GSSConnectionFailed:
		msg.append("Failed to connect to Global Session Service.");
		break;
	case WorkerError::InvalidGssResponse:
		msg.append("Invalid Global Session Service response.");
		break;
	case WorkerError::InvalidGssResponseSyntax:
		msg.append("Invalid Global Session Service response syntax.");
		break;
	case WorkerError::AuthorizationConnetionFailed:
		msg.append("Failed to connect to the Authorization Service.");
		break;
	case WorkerError::InvalidCreadentials:
		msg.append("Invalid Credentials.");
		break;
	case WorkerError::DatabaseConnectionFailed:
		msg.append("Failed to connect to the Database.");
		break;
	case WorkerError::InvalidData:
		msg.append("[FATAL] Invalid data.");
		break;

	case WorkerError::FailedToRegister:
		msg.append("Account could not be created (maybe the specified user name is already in use).");
		break;

	default:
		msg.append("Unknown error");
		OT_LOG_E("Unknown worker error (" + std::to_string((int)_error) + ")");
		break;
	}

	// Display error message and unlock controls
	QMessageBox msgBox(QMessageBox::Critical, "Login Error", msg, QMessageBox::Ok);
	msgBox.exec();

	this->setControlsEnabled(true);
}

// ###########################################################################################################################################################################################################################################################################################################################

// Private helper

void LogInDialog::saveUserSettings(void) const {
	OTAssert(m_loginData.isValid(), "Invalid login data");
	std::shared_ptr<QSettings> settings = AppBase::instance()->createSettingsInstance();

	if (m_savePassword->isChecked()) {
		settings->setValue("LastUsername", QString::fromStdString(m_loginData.getUserName()));
		settings->setValue("LastPassword", QString::fromStdString(m_loginData.getEncryptedUserPassword()));
	}
	else {
		settings->setValue("LastUsername", QString());
		settings->setValue("LastPassword", QString());
	}

	settings->setValue("LastSavePassword", m_savePassword->isChecked());
	settings->setValue("SessionServiceURL", m_loginData.getGss().getName());

	QJsonArray gssOptionsArr;
	for (const LogInGSSEntry& entry : m_gssData) {
		QJsonObject entryObj;
		entryObj["Name"] = entry.getName();
		entryObj["IP"] = entry.getUrl();
		entryObj["Port"] = entry.getPort();
		gssOptionsArr.append(entryObj);
	}

	QJsonDocument gssOptionsDoc(gssOptionsArr);
	settings->setValue("SessionServiceJSON", gssOptionsDoc.toJson(QJsonDocument::Compact));

}

LogInGSSEntry LogInDialog::findCurrentGssEntry(void) {
	if (m_gss->currentIndex() < 0 || m_gss->currentIndex() >= m_gssData.size()) return LogInGSSEntry();
	else return m_gssData[m_gss->currentIndex()];
}

void LogInDialog::initializeGssData(std::shared_ptr<QSettings> _settings) {
	QString lastSessionService = _settings->value("SessionServiceURL", "").toString();
	QByteArray sessionServiceJSON = _settings->value("SessionServiceJSON", QByteArray()).toByteArray();
	int newCurrentIndex = -1;
	int counter = 0;

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
					newCurrentIndex = counter;
				}

				counter++;
			}

			if (newCurrentIndex >= 0) {
				m_gss->setCurrentIndex(newCurrentIndex);
			}
			else if (counter >= 0) {
				m_gss->setCurrentIndex(0);
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
	if (options.isEmpty()) options.append(QString());
	options.append(EDIT_GSS_TEXT);

	m_gss->clear();
	m_gss->addItems(options);
}

// ###########################################################################################################################################################################################################################################################################################################################

// Async worker

void LogInDialog::stopWorkerWithError(WorkerError _error) {
	m_loginData.clear();
	QMetaObject::invokeMethod(this, "slotWorkerError", Qt::QueuedConnection, Q_ARG(WorkerError, _error));
}

void LogInDialog::loginWorkerStart(void) {
	WorkerError currentError = WorkerError::NoError;

	// Get data from GSS
	currentError = this->workerConnectToGSS();
	if (currentError != WorkerError::NoError) {
		this->stopWorkerWithError(currentError);
		return;
	}

	// Ensure the authorization connection is valid
	UserManagement userManager;
	userManager.setAuthServerURL(m_loginData.getAuthorizationUrl());
	userManager.setDatabaseURL(m_loginData.getDatabaseUrl());
	if (!userManager.checkConnectionAuthorizationService()) {
		this->stopWorkerWithError(WorkerError::AuthorizationConnetionFailed);
		return;
	}

	// Attempt to log in the user
	currentError = this->workerLogin(userManager);
	if (currentError != WorkerError::NoError) {
		this->stopWorkerWithError(currentError);
		return;
	}

	if (!m_loginData.isValid()) {
		this->stopWorkerWithError(WorkerError::InvalidData);
		return;
	}

	QMetaObject::invokeMethod(this, &LogInDialog::slotLogInSuccess, Qt::QueuedConnection);
}

void LogInDialog::registerWorkerStart(void) {
	WorkerError currentError = WorkerError::NoError;

	// Get data from GSS
	currentError = this->workerConnectToGSS();
	if (currentError != WorkerError::NoError) {
		this->stopWorkerWithError(currentError);
		return;
	}

	// Ensure the authorization connection is valid
	UserManagement userManager;
	userManager.setAuthServerURL(m_loginData.getAuthorizationUrl());
	userManager.setDatabaseURL(m_loginData.getDatabaseUrl());
	if (!userManager.checkConnectionAuthorizationService()) {
		this->stopWorkerWithError(WorkerError::AuthorizationConnetionFailed);
		return;
	}

	currentError = workerRegister(userManager);
	if (currentError != WorkerError::NoError) {
		this->stopWorkerWithError(currentError);
		return;
	}

	m_loginData.clear();

	QMetaObject::invokeMethod(this, &LogInDialog::slotRegisterSuccess, Qt::QueuedConnection);
}

LogInDialog::WorkerError LogInDialog::workerConnectToGSS(void) {
	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_GetDBandAuthServerUrl, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_MESSAGE, ot::JsonString(OT_INFO_MESSAGE_LogIn, doc.GetAllocator()), doc.GetAllocator());

	std::string response;
	if (!ot::msg::send("", m_loginData.getGss().getConnectionUrl().toStdString(), ot::EXECUTE, doc.toJson(), response, 5000)) {
		return WorkerError::GSSConnectionFailed;
	}

	OT_ACTION_IF_RESPONSE_ERROR(response) {
		return WorkerError::InvalidGssResponse;
	}
	OT_ACTION_IF_RESPONSE_WARNING(response) {
		return WorkerError::InvalidGssResponse;
	}
	
	ot::JsonDocument responseDoc;
	responseDoc.fromJson(response);

	if (!responseDoc.IsObject()) {
		return WorkerError::InvalidGssResponseSyntax;
	}
	if (!responseDoc.HasMember(OT_ACTION_PARAM_SERVICE_DBURL)) {
		return WorkerError::InvalidGssResponseSyntax;
	}
	if (!responseDoc.HasMember(OT_ACTION_PARAM_SERVICE_AUTHURL)) {
		return WorkerError::InvalidGssResponseSyntax;
	}
	if (!responseDoc[OT_ACTION_PARAM_SERVICE_DBURL].IsString()) {
		return WorkerError::InvalidGssResponseSyntax;
	}
	if (!responseDoc[OT_ACTION_PARAM_SERVICE_AUTHURL].IsString()) {
		return WorkerError::InvalidGssResponseSyntax;
	}

	m_loginData.setDatabaseUrl(ot::json::getString(responseDoc, OT_ACTION_PARAM_SERVICE_DBURL));
	m_loginData.setAuthorizationUrl(ot::json::getString(responseDoc, OT_ACTION_PARAM_SERVICE_AUTHURL));

	return WorkerError::NoError;
}

LogInDialog::WorkerError LogInDialog::workerLogin(const UserManagement& _userManager) {
	// Check the username, password combination
	std::string sessionUser, sessionPassword, validPassword, validEncryptedPassword;

	std::string currentPassword = m_password->text().toStdString();
	bool isCurrentPasswordEncrypted = false;

	if (m_state & LogInStateFlag::RestoredPassword) {
		currentPassword = m_restoredPassword.toStdString();
		isCurrentPasswordEncrypted = true;
	}

	std::string currentUserName = m_username->text().toStdString();
	if (!_userManager.checkPassword(currentUserName, currentPassword, isCurrentPasswordEncrypted, sessionUser, sessionPassword, validPassword, validEncryptedPassword)) {
		return WorkerError::InvalidCreadentials;
	}

	m_loginData.setUserName(currentUserName);
	m_loginData.setUserPassword(validPassword);
	m_loginData.setEncryptedUserPassword(validEncryptedPassword);
	m_loginData.setSessionUser(sessionUser);
	m_loginData.setSessionPassword(sessionPassword);

	if (!_userManager.checkConnectionDataBase(m_loginData.getSessionUser(), m_loginData.getSessionPassword())) {
		return WorkerError::DatabaseConnectionFailed;
	}

	return WorkerError::NoError;
}

LogInDialog::WorkerError LogInDialog::workerRegister(const UserManagement& _userManager) {
	if (!_userManager.addUser(m_username->text().toStdString(), m_password->text().toStdString()))
	{
		return WorkerError::FailedToRegister;
	}
	else
	{
		return WorkerError::NoError;
	}
}