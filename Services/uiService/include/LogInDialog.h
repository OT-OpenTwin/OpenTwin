// @otlicense
// File: LogInDialog.h
// 
// License:
// Copyright 2025 by OpenTwin
//  
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
//  
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// @otlicense-end

#pragma once

// Frontend header
#include "LoginData.h"
#include "LogInGSSEntry.h"
#include "UserManagement.h"

// OpenTwin header
#include "OTSystem/Flags.h"
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

	const LoginData& getLoginData() const { return m_loginData; };

	void initialize();
	
protected:
	virtual bool mayCloseDialogWindow() override;
	virtual void showEvent(QShowEvent* _event) override;

private:
	enum class LogInStateFlag {
		NoState = 0x00,
		RestoredPassword   = 0x01,
		WorkerRunning      = 0x02,
		LogInSuccess       = 0x04,

		RegisterMode       = 0x10,
		ChangePasswordMode = 0x20
	};
	typedef ot::Flags<LogInStateFlag> LogInState;
	OT_ADD_FRIEND_FLAG_FUNCTIONS(LogInDialog::LogInStateFlag, LogInDialog::LogInState)

	enum class WorkerError {
		NoError,
		GSSConnectionFailed,
		IncompatibleVersions,
		InvalidGssResponse,
		InvalidGssResponseSyntax,
		AuthorizationConnetionFailed,
		InvalidCreadentials,
		DatabaseConnectionFailed,
		InvalidData,
		FailedToRegister,
		FailedToChangePassword
	};

	LogInState m_state;

	LoginData m_loginData; //! \brief Holds the login data that is set during the login by the worker thread.
	
	ot::ComboBox* m_gss;
	ot::LineEdit* m_username;
	ot::Label* m_passwordLabel;
	ot::LineEdit* m_password; // LogIn, Register:1, Change:Old
	ot::Label* m_passwordNewLabel;
	ot::LineEdit* m_passwordNew; // Change:New
	ot::Label* m_passwordConfirmLabel;
	ot::LineEdit* m_passwordConfirm; // Register:Confirm, Change:Confirm
	ot::CheckBox* m_savePassword;
	ot::Label* m_toggleChangePasswordModeLabel;
	ot::Label* m_toggleRegisterModeLabel;
	ot::PushButton* m_logInButton;
	ot::PushButton* m_changePasswordButton;
	ot::PushButton* m_registerButton;
	ot::PushButton* m_exitButton;

	QString m_restoredPassword;

	std::string m_databaseUrl;
	std::string m_authorizationServiceUrl;

	std::vector<LogInGSSEntry> m_gssData;

	// ###########################################################################################################################################################################################################################################################################################################################

	// Private slots

private Q_SLOTS:
	void slotLogIn();
	void slotRegister();
	void slotChangePassword();
	void slotToggleLogInAndRegisterMode();
	void slotToggleChangePasswordMode();
	void slotGSSChanged();
	void slotPasswordChanged();
	
	void slotLogInSuccess();
	void slotRegisterSuccess();
	void slotChangePasswordSuccess();
	void slotWorkerError(WorkerError _error);

	// ###########################################################################################################################################################################################################################################################################################################################

	// Private helper

private:
	std::string  m_curlErrorMessage = "";

	void saveUserSettings() const;
	void saveGSSOptions() const;

	LogInGSSEntry findCurrentGssEntry();
	void initializeGssData(std::shared_ptr<QSettings> _settings);
	void updateGssOptions();

	// ###########################################################################################################################################################################################################################################################################################################################

	// Async worker

	void stopWorkerWithError(WorkerError _error);

	void loginWorkerStart();
	void registerWorkerStart();
	void changePasswordWorkerStart();
	WorkerError workerCheckVersionCompatibility();
	WorkerError workerConnectToGSS();
	WorkerError workerLogin(const UserManagement& _userManager);
	WorkerError workerRegister(const UserManagement& _userManager);
	WorkerError workerChangePassword(const UserManagement& _userManager);
};
