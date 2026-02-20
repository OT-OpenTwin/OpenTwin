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
	enum class ConfigFlag {
		NoFlags    = 0 << 0, //! @brief Default configuration with no special flags.
		NoRegister = 1 << 0  //! @brief Configuration flag to disable the registration option in the dialog.
	};
	typedef ot::Flags<ConfigFlag> Config;

	LogInDialog();
	virtual ~LogInDialog();

	void setControlsEnabled(bool _enabled);

	const LoginData& getLoginData() const { return m_loginData; };

	void initialize();
	
	void setConfigFlag(ConfigFlag _flag, bool _enabled = true);
	void setConfig(const Config& _cfg);
	const Config& getConfig() const { return m_config; };

Q_SIGNALS:
	void dialogShown();
	void configChanged(const Config& _newConfig);

public Q_SLOTS:
	void applyConfig();

protected:
	virtual bool mayCloseDialogWindow() override;
	virtual void showEvent(QShowEvent* _event) override;

private:
	enum class LogInStateFlag {
		NoState            = 0 << 0,
		RestoredPassword   = 1 << 0,
		WorkerRunning      = 1 << 1,
		LogInSuccess       = 1 << 2,

		RegisterMode       = 1 << 3,
		ChangePasswordMode = 1 << 4,
		SSOMode            = 1 << 5
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
	Config m_config;

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

	// Slots

public Q_SLOTS:
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
	std::string m_curlErrorMessage = "";
	QString m_userNameTmp;

	const int m_maxDefaultHeight = 515;
	const int m_maxSSOHeight = 460;

	void saveUserSettings() const;
	void saveGSSOptions() const;
	void editGSSEntries();

	LogInGSSEntry findCurrentGssEntry() const;
	void initializeGssData(std::shared_ptr<QSettings> _settings);
	void updateGssOptions();

	std::wstring determineSSOUsername() const;

	void setControlsForUsernamePassword();
	void setControlsForRegister();
	void setControlsForChangePassword();
	void setControlsForSSO();

	// ###########################################################################################################################################################################################################################################################################################################################

	// Async worker

	void stopWorkerWithError(WorkerError _error);

	void loginWorkerStart();
	void registerWorkerStart();
	void changePasswordWorkerStart();
	WorkerError workerCheckVersionCompatibility();
	WorkerError workerConnectToGSS();
	WorkerError workerLogin(const UserManagement& _userManager);
	WorkerError workerLoginUsernamePassword(const UserManagement& _userManager);
	WorkerError workerLoginSSO(const UserManagement& _userManager);
	WorkerError workerRegister(const UserManagement& _userManager);
	WorkerError workerChangePassword(const UserManagement& _userManager);
};

OT_ADD_FLAG_FUNCTIONS(LogInDialog::ConfigFlag, LogInDialog::Config)