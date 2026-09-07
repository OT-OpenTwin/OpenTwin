// @otlicense
// File: StartArgumentParser.h
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
#include "Login/LoginData.h"

// OpenTwin header
#include "OTCore/ProjectInformation.h"

// Qt header
#include <QtCore/qstring.h>

class StartArgumentParser {
	OT_DECL_DEFMOVE(StartArgumentParser)
	OT_DECL_DEFCOPY(StartArgumentParser)
public:
	StartArgumentParser() = default;
	~StartArgumentParser() = default;

	bool parse();

	//! @brief Create a list of command line arguments with the current data.
	QStringList createCommandLineArgs() const;

	//! @brief Create a URL with the current data.
	QString createFrontendUrlLink() const;

	QString createShareLink() const;

	void clear();

	void setDebug(bool _debug = true) { m_debug = _debug; };
	bool getDebug() const { return m_debug; };

	void setCheckGraphics(bool _check = true) { m_checkGraphics = _check; };
	bool getCheckGraphics() const { return m_checkGraphics; };

	//! @brief Set the log in data.
	//! The login data is used to log in automatically when the application starts with the given login information.
	//! @warning This should only be used when launching a new instance of the application locally.
	//! Do not use this to create shared information.
	void setLogInData(const LoginData& _data) { m_loginData = _data; m_logInDataSet = true; };
	bool getLogInDataSet() const { return m_logInDataSet; };
	const LoginData& getLogInData() const { return m_loginData; };

	//! @brief Set auto login enabled flag.
	//! If enabled the application will attempt to log in automatically using the intially set login data.
	//! This is only possible if the user uses SSO or has saved the password.
	//! This has no effect if the log in data is set.
	void setAutoLogin(bool _autoLogin = true) { m_autoLogin = _autoLogin; };
	bool getAutoLogin() const { return m_autoLogin; };

	bool getOpenProjectSet() const { return m_openProject; };
	void setProjectInfo(const ot::ProjectInformation& _info) { m_projectInfo = _info; m_openProject = true; };
	const ot::ProjectInformation& getProjectInfo() const { return m_projectInfo; };

	void setProjectVersion(const std::string& _version) { m_projectVersion = _version; };
	const std::string& getProjectVersion() const { return m_projectVersion; };

	void setScriptFile(const QString& _file) { m_scriptFile = _file; };
	const QString& getScriptFile() const { return m_scriptFile; };

private:
	bool parseUrl(const QString& _url);
	bool parseCommandLine();

	enum class ArgumentKey
	{
		Debug,
		CheckGraphics,
		CheckGraphicsShort,
		LogInData,
		AutoLogIn,
		OpenProject,
		ProjectVersion,
		ScriptFile
	};
	QString toString(ArgumentKey _key) const;
	QString toUrl(ArgumentKey _key) const;
	QString toCommandLine(ArgumentKey _key) const;

	static QString shareBackendScheme() { return "https"; };
	static QString shareBackendEndpoint() { return "/open"; };
	static QString shareBackendArgumentsKey() { return "uri"; };

	static QString shareFrontendScheme() { return "opentwin"; };
	static QString shareFrontendHost() { return "run"; };

	bool m_debug = false;
	bool m_checkGraphics = false;

	bool m_logInDataSet = false;
	LoginData m_loginData;

	bool m_autoLogin = false;

	bool m_openProject = false;
	ot::ProjectInformation m_projectInfo;
	std::string m_projectVersion;

	QString m_scriptFile;
};