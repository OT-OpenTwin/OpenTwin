//! @file LogInDialog.h
//! @author Alexander Kuester (alexk95)
//! @date April 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// Frontend header
#include "LoginData.h"
#include "LogInGSSEntry.h"
#include "UserManagement.h"

// OpenTwin header
#include "OTCore/Flags.h"
#include "OTWidgets/Dialog.h"

// Qt header
#include <QtCore/qstring.h>

// std header
#include <vector>

class QSettings;

namespace ot {
	class Label;
	class CheckBox;
	class LineEdit;
	class ComboBox;
	class PushButton;
}

class LogInDialog : public ot::Dialog {
	Q_OBJECT
public:
	LogInDialog();
	virtual ~LogInDialog();

	void setControlsEnabled(bool _enabled);

	const LoginData& getLoginData(void) const { return m_loginData; };

protected:
	virtual void closeEvent(QCloseEvent* _event) override;

private:
	enum class LogInStateFlag {
		NoState = 0x00,
		RestoredPassword = 0x01,
		WorkerRunning = 0x02,
		LogInSuccess = 0x04,
		RegisterMode = 0x08
	};
	typedef ot::Flags<LogInStateFlag> LogInState;

	enum class WorkerError {
		NoError,
		GSSConnectionFailed,
		InvalidGssResponse,
		InvalidGssResponseSyntax,
		AuthorizationConnetionFailed,
		InvalidCreadentials,
		DatabaseConnectionFailed,
		InvalidData,
		FailedToRegister
	};

	LogInState m_state;
	int m_logInAttempt;

	LoginData m_loginData; //! \brief Holds the login data that is set during the login by the worker thread.

	ot::ComboBox* m_gss;
	ot::LineEdit* m_username;
	ot::LineEdit* m_password;
	ot::Label* m_confirmPasswordLabel;
	ot::LineEdit* m_confirmPassword;
	ot::CheckBox* m_savePassword;
	ot::Label* m_toggleModeLabel;
	ot::PushButton* m_logInButton;
	ot::PushButton* m_registerButton;
	ot::PushButton* m_exitButton;

	QString m_restoredPassword;

	std::string m_databaseUrl;
	std::string m_authorizationServiceUrl;


	std::vector<LogInGSSEntry> m_gssData;

	// ###########################################################################################################################################################################################################################################################################################################################

	// Private slots

private Q_SLOTS:
	void slotLogIn(void);
	void slotRegister(void);
	void slotCancel(void);
	void slotToggleLogInMode(void);
	void slotGSSChanged(void);
	void slotPasswordChanged(void);
	
	void slotLogInSuccess(void);
	void slotRegisterSuccess(void);
	void slotWorkerError(WorkerError _error);

	// ###########################################################################################################################################################################################################################################################################################################################

	// Private helper

private:
	void saveUserSettings(void) const;

	LogInGSSEntry findCurrentGssEntry(void);
	void initializeGssData(std::shared_ptr<QSettings> _settings);
	void updateGssOptions(void);

	// ###########################################################################################################################################################################################################################################################################################################################

	// Async worker

	void stopWorkerWithError(WorkerError _error);

	void loginWorkerStart(void);
	void registerWorkerStart(void);
	WorkerError workerConnectToGSS(void);
	WorkerError workerLogin(const UserManagement& _userManager);
	WorkerError workerRegister(const UserManagement& _userManager);

	OT_FRIEND_FLAG_FUNCTIONS(LogInDialog::LogInStateFlag)
};

OT_ADD_FLAG_FUNCTIONS(LogInDialog::LogInStateFlag)