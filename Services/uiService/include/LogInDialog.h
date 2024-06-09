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

private:
	enum class LogInStateFlag {
		NoState = 0x00,
		RestoredPassword = 0x01,
		LogInSuccess = 0x04
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
		InvalidData
	};

	LogInState m_state;
	int m_logInAttempt;

	LoginData m_loginData; //! \brief Holds the login data that is set during the login by the worker thread.

	ot::ComboBox* m_gss;
	ot::LineEdit* m_username;
	ot::LineEdit* m_password;
	ot::CheckBox* m_savePassword;
	ot::Label* m_newUserLabel;
	ot::PushButton* m_logInButton;
	ot::PushButton* m_exitButton;

	QString m_restoredPassword;

	std::string m_databaseUrl;
	std::string m_authorizationServiceUrl;


	std::vector<LogInGSSEntry> m_gssData;

	// ###########################################################################################################################################################################################################################################################################################################################

	// Private slots

private Q_SLOTS:
	void slotLogIn(void);
	void slotCancel(void);
	void slotNewUser(void);
	void slotGSSChanged(void);
	void slotPasswordChanged(void);
	
	void slotLogInSuccess(void);
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

	void workerStart(void);
	WorkerError workerConnectToGSS(void);
	WorkerError workerLogin(UserManagement& _userManager);

	OT_FRIEND_FLAG_FUNCTIONS(LogInDialog::LogInStateFlag)
};

OT_ADD_FLAG_FUNCTIONS(LogInDialog::LogInStateFlag)