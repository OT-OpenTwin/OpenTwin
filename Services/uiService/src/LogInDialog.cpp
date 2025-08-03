//! @file LogInDialog.cpp
//! @author Alexander Kuester (alexk95)
//! @date April 2024
// ###########################################################################################################################################################################################################################################################################################################################

// Frontend header
#include "AppBase.h"
#include "LogInDialog.h"
#include "LogInGSSEditDialog.h"
#include "DownloadFile.h"

// OpenTwin header
#include "OTSystem/SystemProcess.h"
#include "OTSystem/SystemInformation.h"
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
#include "OTCommunication/ServiceLogNotifier.h"

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

#define TOGGLE_MODE_LABEL_SwitchToLogIn "Switch to Login"
#define TOGGLE_MODE_LABEL_SwitchToRegister "Switch to Registration"
#define TOGGLE_MODE_LABEL_SwitchToChangePassword "Change Password"

LogInDialog::LogInDialog() 
	: m_state(LogInStateFlag::NoState), m_logInAttempt(0)
{
	using namespace ot;

	// Create layouts
	QVBoxLayout* centralLayout = new QVBoxLayout(this);
	QHBoxLayout* imageViewLayout = new QHBoxLayout;
	QGridLayout* inputLayout = new QGridLayout;
	QGridLayout* buttonLayout = new QGridLayout;
	QHBoxLayout* changePasswordLayout = new QHBoxLayout;
	QHBoxLayout* registerLayout = new QHBoxLayout;
	
	// Create controls
	ImagePreview* titleImageView = new ImagePreview(IconManager::getPixmap("Images/OpenTwinIcon.png").scaled(QSize(192, 192), Qt::KeepAspectRatio).toImage());
	titleImageView->setObjectName("LogInDialogImageView");
	titleImageView->setEnableResizing(false);
	titleImageView->setFixedSize(titleImageView->getImage().size());
	titleImageView->setEnabled(false);

	Label* gssLabel = new Label("Global Session Service:");
	m_gss = new ComboBox;
	m_gss->setEditable(false);

	Label* usernameLabel = new Label("Username:");
	m_username = new LineEdit;
	
	m_passwordLabel = new Label("Password:");
	m_password = new LineEdit;
	m_password->setEchoMode(QLineEdit::Password);
	
	m_passwordNewLabel = new Label("New Password:");
	m_passwordNewLabel->setHidden(true);
	m_passwordNew = new LineEdit;
	m_passwordNew->setEchoMode(QLineEdit::Password);
	m_passwordNew->setHidden(true);

	m_passwordConfirmLabel = new Label("Confirm Password:");
	m_passwordConfirmLabel->setHidden(true);
	m_passwordConfirm = new LineEdit;
	m_passwordConfirm->setEchoMode(QLineEdit::Password);
	m_passwordConfirm->setHidden(true);

	m_savePassword = new CheckBox("Save Password");
	m_savePassword->setObjectName("LogInDialogSavePassword");

	Label* spacerLabel1 = new Label;
	Label* spacerLabel2 = new Label;

	m_logInButton = new PushButton("Log In");
	m_registerButton = new PushButton("Register");
	m_registerButton->setHidden(true);
	m_changePasswordButton = new PushButton("Change Password");
	m_changePasswordButton->setHidden(true);
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
	
	// Setup layouts
	imageViewLayout->addStretch(1);
	imageViewLayout->addWidget(titleImageView);
	imageViewLayout->addStretch(1);

	inputLayout->addWidget(gssLabel, 0, 0);
	inputLayout->addWidget(m_gss, 0, 1);
	inputLayout->addWidget(usernameLabel, 1, 0);
	inputLayout->addWidget(m_username, 1, 1);
	inputLayout->addWidget(m_passwordLabel, 2, 0);
	inputLayout->addWidget(m_password, 2, 1);
	inputLayout->addWidget(m_passwordNewLabel, 3, 0);
	inputLayout->addWidget(m_passwordNew, 3, 1);
	inputLayout->addWidget(m_passwordConfirmLabel, 4, 0);
	inputLayout->addWidget(m_passwordConfirm, 4, 1);
	inputLayout->addWidget(m_savePassword, 5, 1);
	
	buttonLayout->addWidget(m_logInButton, 0, 0);
	buttonLayout->addWidget(m_registerButton, 1, 0);
	buttonLayout->addWidget(m_changePasswordButton, 2, 0);
	buttonLayout->addWidget(m_exitButton, 3, 0);

	centralLayout->addWidget(spacerLabel1);
	centralLayout->addLayout(imageViewLayout);
	centralLayout->addStretch(1);
	centralLayout->addLayout(inputLayout);
	centralLayout->addWidget(spacerLabel2);
	centralLayout->addLayout(buttonLayout);
	centralLayout->addLayout(changePasswordLayout);
	centralLayout->addLayout(registerLayout);


	// ... Setup optional inputs (this section may be disabled when a registration is not allowed)
	m_toggleRegisterModeLabel = new Label(TOGGLE_MODE_LABEL_SwitchToRegister);
	m_toggleRegisterModeLabel->setObjectName("LogInDialogRegisterLabel");
	m_toggleRegisterModeLabel->setMargin(4);
	registerLayout->addStretch(1);
	registerLayout->addWidget(m_toggleRegisterModeLabel);
	registerLayout->addStretch(1);
	this->connect(m_toggleRegisterModeLabel, &Label::mouseClicked, this, &LogInDialog::slotToggleLogInAndRegisterMode);

	m_toggleChangePasswordModeLabel = new Label(TOGGLE_MODE_LABEL_SwitchToChangePassword);
	m_toggleChangePasswordModeLabel->setObjectName("LogInDialogChangePasswordLabel");
	m_toggleChangePasswordModeLabel->setMargin(4);
	m_toggleChangePasswordModeLabel->setVisible(false);
	changePasswordLayout->addStretch(1);
	changePasswordLayout->addWidget(m_toggleChangePasswordModeLabel);
	changePasswordLayout->addStretch(1);
	this->connect(m_toggleChangePasswordModeLabel, &Label::mouseClicked, this, &LogInDialog::slotToggleChangePasswordMode);

	// Setup window
	QRect newTargetRect(ot::Positioning::getCenterWidgetOnParentRect(nullptr, static_cast<QWidget*>(this)).topLeft(), QSize(350, 500));

	this->setObjectName("LogInDialog");
	this->setWindowTitle("OpenTwin Login");
	this->setWindowFlags(Qt::Dialog | Qt::WindowType::FramelessWindowHint);
	this->setWindowIcon(ot::IconManager::getApplicationIcon());
	this->setFixedSize(newTargetRect.size());
	this->setDialogFlag(ot::DialogCfg::MoveGrabAnywhere);
	this->setDialogFlag(ot::DialogCfg::RecenterOnF11);
	QPoint topLeftPos = newTargetRect.topLeft();
	topLeftPos.setX(settings->value("LogInPos.X", topLeftPos.x()).toInt());
	topLeftPos.setY(settings->value("LogInPos.Y", topLeftPos.y()).toInt());
	newTargetRect.moveTo(topLeftPos);
	this->move(ot::Positioning::fitOnScreen(newTargetRect).topLeft());

	// Connect signals
	this->connect(m_logInButton, &PushButton::clicked, this, &LogInDialog::slotLogIn);
	this->connect(m_registerButton, &PushButton::clicked, this, &LogInDialog::slotRegister);
	this->connect(m_changePasswordButton, &PushButton::clicked, this, &LogInDialog::slotChangePassword);
	this->connect(m_exitButton, &PushButton::clicked, this, &LogInDialog::closeCancel);
	this->connect(m_gss, &ComboBox::currentTextChanged, this, &LogInDialog::slotGSSChanged);
	this->connect(m_password, &LineEdit::textChanged, this, &LogInDialog::slotPasswordChanged);

}

LogInDialog::~LogInDialog() {
	
}

void LogInDialog::setControlsEnabled(bool _enabled) {
	m_gss->setEnabled(_enabled);
	m_username->setEnabled(_enabled);
	m_password->setEnabled(_enabled);
	m_passwordNew->setEnabled(_enabled);
	m_passwordConfirm->setEnabled(_enabled);
	m_savePassword->setEnabled(_enabled);
	m_toggleChangePasswordModeLabel->setEnabled(_enabled);
	m_toggleRegisterModeLabel->setEnabled(_enabled);
	m_logInButton->setEnabled(_enabled);
	m_registerButton->setEnabled(_enabled);
	m_changePasswordButton->setEnabled(_enabled);
	m_exitButton->setEnabled(_enabled);
}

bool LogInDialog::mayCloseDialogWindow() {
	return !(m_state & LogInStateFlag::WorkerRunning);
}

// ###########################################################################################################################################################################################################################################################################################################################

// Private slots

void LogInDialog::slotLogIn() {
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

	this->setControlsEnabled(false);

	m_loginData.setGss(gssData);

	// Store settings
	std::shared_ptr<QSettings> settings = AppBase::instance()->createSettingsInstance();
	settings->setValue("SessionServiceURL", gssData.getName());
	settings->setValue("LogInPos.X", this->pos().x());
	settings->setValue("LogInPos.Y", this->pos().y());

	// Run worker
	m_state |= LogInStateFlag::WorkerRunning;

	std::thread worker(&LogInDialog::loginWorkerStart, this);
	worker.detach();
}

void LogInDialog::slotRegister() {
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

	if (m_password->text() != m_passwordConfirm->text()) {
		QToolTip::showText(this->mapToGlobal(m_passwordConfirm->pos()), "Confirm password does not match the password", m_passwordConfirm, QRect(), 3000);
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

void LogInDialog::slotChangePassword() {
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

	if (m_passwordNew->text().isEmpty()) {
		QToolTip::showText(this->mapToGlobal(m_passwordNew->pos()), "No password provided", m_passwordNew, QRect(), 3000);
		return;
	}

	if (m_passwordNew->text().length() < 4) {
		QToolTip::showText(this->mapToGlobal(m_passwordNew->pos()), "Password is too short", m_passwordNew, QRect(), 3000);
		return;
	}

	if (m_passwordNew->text() != m_passwordConfirm->text()) {
		QToolTip::showText(this->mapToGlobal(m_passwordConfirm->pos()), "Confirm password does not match the new password", m_passwordConfirm, QRect(), 3000);
		return;
	}

	m_logInAttempt++;

	// Run worker
	this->setControlsEnabled(false);

	m_loginData.setGss(gssData);

	m_state |= LogInStateFlag::WorkerRunning;

	std::thread worker(&LogInDialog::changePasswordWorkerStart, this);
	worker.detach();
}

void LogInDialog::slotToggleLogInAndRegisterMode() {
	OTAssert(!(m_state & LogInStateFlag::WorkerRunning), "Worker running");

	if (m_state & LogInStateFlag::RegisterMode) {
		m_logInButton->setHidden(false);
		m_registerButton->setHidden(true);
		m_passwordConfirmLabel->setHidden(true);
		m_passwordConfirm->setHidden(true);
		m_toggleRegisterModeLabel->setText(TOGGLE_MODE_LABEL_SwitchToRegister);
		//m_toggleChangePasswordModeLabel->setHidden(false);
		m_state &= (~LogInStateFlag::RegisterMode);
	}
	else {
		m_logInButton->setHidden(true);
		m_registerButton->setHidden(false);
		m_passwordConfirm->setHidden(false);
		m_passwordConfirmLabel->setHidden(false);
		m_toggleRegisterModeLabel->setText(TOGGLE_MODE_LABEL_SwitchToLogIn);
		//m_toggleChangePasswordModeLabel->setHidden(true);
		m_state |= LogInStateFlag::RegisterMode;

		if (m_state & LogInStateFlag::RestoredPassword) {
			m_password->blockSignals(true);
			m_password->setText(QString());
			m_password->blockSignals(false);
		}
	}

	this->update();
}

void LogInDialog::slotToggleChangePasswordMode() {
	OTAssert(!(m_state & LogInStateFlag::WorkerRunning), "Worker running");

	if (m_state & LogInStateFlag::ChangePasswordMode) {
		m_logInButton->setHidden(false);
		m_changePasswordButton->setHidden(true);
		m_passwordNew->setHidden(true);
		m_passwordNewLabel->setHidden(true);
		m_passwordConfirm->setHidden(true);
		m_passwordConfirmLabel->setHidden(true);
		m_toggleChangePasswordModeLabel->setText(TOGGLE_MODE_LABEL_SwitchToChangePassword);
		m_toggleRegisterModeLabel->setHidden(false);
		m_state &= (~LogInStateFlag::ChangePasswordMode);
	}
	else {
		m_logInButton->setHidden(true);
		m_changePasswordButton->setHidden(false);
		m_passwordNew->setHidden(false);
		m_passwordNewLabel->setHidden(false);
		m_passwordConfirm->setHidden(false);
		m_passwordConfirmLabel->setHidden(false);
		m_toggleChangePasswordModeLabel->setText(TOGGLE_MODE_LABEL_SwitchToLogIn);
		m_toggleRegisterModeLabel->setHidden(true);
		m_state |= LogInStateFlag::ChangePasswordMode;

		if (m_state & LogInStateFlag::RestoredPassword) {
			m_password->blockSignals(true);
			m_password->setText(QString());
			m_password->blockSignals(false);
		}
	}

	this->update();
}

void LogInDialog::slotGSSChanged() {
	if (m_gss->currentText() != EDIT_GSS_TEXT) {
		return;
	}

	LogInGSSEditDialog dialog(m_gssData);

	ot::Dialog::DialogResult result = dialog.showDialog();

	m_gss->blockSignals(true);
	
	if (result == ot::Dialog::Ok) {
		m_gssData = dialog.getEntries();
		this->updateGssOptions();
		this->saveGSSOptions();
	}

	m_gss->setCurrentIndex(0);
	m_gss->blockSignals(false);
}

void LogInDialog::slotPasswordChanged() {
	if (!(m_state & LogInStateFlag::RestoredPassword)) {
		return;
	}

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

void LogInDialog::slotLogInSuccess() {
	m_state &= (~LogInStateFlag::WorkerRunning);
	this->saveUserSettings();
	this->closeDialog(ot::Dialog::Ok);
}

void LogInDialog::slotRegisterSuccess() {
	m_state &= (~LogInStateFlag::WorkerRunning);

	QMessageBox msgBox(QMessageBox::Information, "Registration", "The account was created successfully.", QMessageBox::Ok);
	msgBox.exec();

	m_passwordConfirm->setText(QString());

	this->slotToggleLogInAndRegisterMode();
	this->setControlsEnabled(true);
}

void LogInDialog::slotChangePasswordSuccess() {
	m_state &= (~LogInStateFlag::WorkerRunning);

	QMessageBox msgBox(QMessageBox::Information, "Change Password", "The password was updated successfully.", QMessageBox::Ok);
	msgBox.exec();

	m_passwordNew->setText(QString());
	m_passwordConfirm->setText(QString());
	
	this->slotToggleChangePasswordMode();
	this->setControlsEnabled(true);
}

void LogInDialog::slotWorkerError(WorkerError _error) {
	m_state &= (~LogInStateFlag::WorkerRunning);

	// Check for max login attempt
	if (m_logInAttempt >= MAX_LOGIN_ATTEMPTS) {
		this->closeDialog(ot::Dialog::Cancel);
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

	if (_error == WorkerError::IncompatibleVersions)
	{
		msg.append("Incompatible versions of frontend and backend services.\n\n"
				   "Do you want to update the frontend to match the backend?");

		QMessageBox msgBox(QMessageBox::Critical, "Login Error", msg, QMessageBox::Yes | QMessageBox::Cancel);

		if (msgBox.exec() == QMessageBox::Yes)
		{
			// We try to update the frontend installation
			QMetaObject::invokeMethod(AppBase::instance(), "downloadInstaller", Qt::QueuedConnection, Q_ARG(QString, m_loginData.getGss().getConnectionUrl()));

			m_loginData.clear();
			this->setControlsEnabled(true);
			return;
		}

		m_loginData.clear();
		this->setControlsEnabled(true);
		return;
	}


	switch (_error)
	{
	case WorkerError::GSSConnectionFailed:
		msg.append("Failed to connect to Global Session Service. Check backend service status, firewall settings and certificates.");
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

	case WorkerError::FailedToChangePassword:
		msg.append("Password update failed (invalid password provided).");
		break;

	default:
		msg.append("Unknown error");
		OT_LOG_E("Unknown worker error (" + std::to_string((int)_error) + ")");
		break;
	}
	// Display error message and unlock controls
	QMessageBox msgBox(QMessageBox::Critical, "Login Error", msg, QMessageBox::Ok);
	if (!m_curlErrorMessage.empty()) {
		msgBox.setDetailedText(QString::fromStdString(m_curlErrorMessage));
	}
	msgBox.exec();

	m_loginData.clear();
	this->setControlsEnabled(true);
}

// ###########################################################################################################################################################################################################################################################################################################################

// Private helper

void LogInDialog::saveUserSettings() const {
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

	this->saveGSSOptions();
}

void LogInDialog::saveGSSOptions() const {
	std::shared_ptr<QSettings> settings = AppBase::instance()->createSettingsInstance();

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

LogInGSSEntry LogInDialog::findCurrentGssEntry() {
	int index = m_gss->currentIndex();
	if (index < 0 || index >= m_gssData.size()) {
		return LogInGSSEntry();
	}
	else {
		return m_gssData[index];
	}
}

void LogInDialog::initializeGssData(std::shared_ptr<QSettings> _settings) {
	QString lastSessionServiceName = _settings->value("SessionServiceURL", "").toString();
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

				if (newEntry.getName() == lastSessionServiceName) {
					newCurrentIndex = counter;
				}

				counter++;
			}
		}
		else {
			OT_LOG_EA("SessionService list is not a JSON array");
		}
	}

	this->updateGssOptions();

	if (newCurrentIndex >= 0) {
		m_gss->setCurrentIndex(newCurrentIndex);
	}
	else if (counter >= 0) {
		m_gss->setCurrentIndex(0);
	}
}

void LogInDialog::updateGssOptions() {
	QStringList options;
	for (const LogInGSSEntry& entry : m_gssData) {
		options.append(entry.getDisplayText());
	}
	
	if (options.isEmpty()) {
		options.append(QString());
	}
	
	options.append(EDIT_GSS_TEXT);

	m_gss->clear();
	m_gss->addItems(options);
}

// ###########################################################################################################################################################################################################################################################################################################################

// Async worker

void LogInDialog::stopWorkerWithError(WorkerError _error) {
	QMetaObject::invokeMethod(this, "slotWorkerError", Qt::QueuedConnection, Q_ARG(WorkerError, _error));
}

void LogInDialog::loginWorkerStart() {
	WorkerError currentError = WorkerError::NoError;

	// Check the version compatiblity
	currentError = this->workerCheckVersionCompatibility();
	if (currentError != WorkerError::NoError) {
		m_curlErrorMessage = ot::msg::getLastError();
		this->stopWorkerWithError(currentError);
		return;
	}
	
	// Get data from GSS
	currentError = this->workerConnectToGSS();
	if (currentError != WorkerError::NoError) {
		m_curlErrorMessage = ot::msg::getLastError();
		this->stopWorkerWithError(currentError);
		return;
	}

	// Ensure the authorization connection is valid
	UserManagement userManager;
	userManager.setAuthServerURL(m_loginData.getAuthorizationUrl());
	userManager.setDatabaseURL(m_loginData.getDatabaseUrl());
	if (!userManager.checkConnectionAuthorizationService()) {
		m_curlErrorMessage = ot::msg::getLastError();
		this->stopWorkerWithError(WorkerError::AuthorizationConnetionFailed);
		return;
	}

	// Attempt to log in the user
	currentError = this->workerLogin(userManager);
	if (currentError != WorkerError::NoError) {
		m_curlErrorMessage = ot::msg::getLastError();
		this->stopWorkerWithError(currentError);
		return;
	}

	if (!m_loginData.isValid()) {
		m_curlErrorMessage = ot::msg::getLastError();
		this->stopWorkerWithError(WorkerError::InvalidData);
		return;
	}

	QMetaObject::invokeMethod(this, &LogInDialog::slotLogInSuccess, Qt::QueuedConnection);
}

void LogInDialog::registerWorkerStart() {
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

void LogInDialog::changePasswordWorkerStart() {
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

	currentError = workerChangePassword(userManager);
	if (currentError != WorkerError::NoError) {
		this->stopWorkerWithError(currentError);
		return;
	}

	m_loginData.clear();

	QMetaObject::invokeMethod(this, &LogInDialog::slotChangePasswordSuccess, Qt::QueuedConnection);
}

LogInDialog::WorkerError LogInDialog::workerCheckVersionCompatibility() {
	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_GetBuildInformation, doc.GetAllocator()), doc.GetAllocator());

	std::string response;
	if (!ot::msg::send("", m_loginData.getGss().getConnectionUrl().toStdString(), ot::EXECUTE_ONE_WAY_TLS, doc.toJson(), response, ot::msg::defaultTimeout, ot::msg::DefaultFlagsNoExit)) {
		return WorkerError::GSSConnectionFailed;
	}

	// Now we get the local build information
	ot::SystemInformation info;
	std::string buildInfo = info.getBuildInformation();

	// Compare the compatibility
	if (response != buildInfo)
	{
		return WorkerError::IncompatibleVersions;
	}

	return WorkerError::NoError;
}

LogInDialog::WorkerError LogInDialog::workerConnectToGSS() {
	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_GetGlobalServicesUrl, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_MESSAGE, ot::JsonString(OT_INFO_MESSAGE_LogIn, doc.GetAllocator()), doc.GetAllocator());

	std::string response;
	if (!ot::msg::send("", m_loginData.getGss().getConnectionUrl().toStdString(), ot::EXECUTE_ONE_WAY_TLS, doc.toJson(), response, ot::msg::defaultTimeout, ot::msg::DefaultFlagsNoExit)) {
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

	if (responseDoc.HasMember(OT_ACTION_PARAM_GlobalLoggerUrl)) {
		if (!responseDoc[OT_ACTION_PARAM_GlobalLoggerUrl].IsString()) {
			return WorkerError::InvalidGssResponseSyntax;
		}

		ot::ServiceLogNotifier::instance().setLoggingServiceURL(ot::json::getString(responseDoc, OT_ACTION_PARAM_GlobalLoggerUrl));
	}

	if (responseDoc.HasMember(OT_ACTION_PARAM_GlobalLogFlags)) {
		if (!responseDoc[OT_ACTION_PARAM_GlobalLogFlags].IsArray()) {
			return WorkerError::InvalidGssResponseSyntax;
		}

		ot::ConstJsonArray flags = ot::json::getArray(responseDoc, OT_ACTION_PARAM_GlobalLogFlags);
		ot::LogDispatcher::instance().setLogFlags(ot::logFlagsFromJsonArray(flags));
	}

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

LogInDialog::WorkerError LogInDialog::workerChangePassword(const UserManagement& _userManager) {
	if (true) //!_userManager.addUser(m_username->text().toStdString(), m_password->text().toStdString()))
	{
		return WorkerError::FailedToChangePassword;
	}
	else
	{
		return WorkerError::NoError;
	}
}