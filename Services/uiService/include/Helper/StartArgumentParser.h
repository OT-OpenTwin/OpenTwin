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

// std header
#include <optional>

class StartArgumentParser {
	OT_DECL_DEFMOVE(StartArgumentParser)
	OT_DECL_DEFCOPY(StartArgumentParser)
public:
	StartArgumentParser() = default;
	~StartArgumentParser() = default;

	// ###########################################################################################################################################################################################################################################################################################################################

	// Parsing

	//! @brief Parse the command line arguments and store the data.
	//! @note The QCoreApplication must be initialized before calling this function.
	//! @return True if the parsing was successful, false otherwise.
	bool parse();

	// ###########################################################################################################################################################################################################################################################################################################################

	// Data Export

	//! @brief Create a list of command line arguments with the current data.
	QStringList createCommandLineArgs() const;

	//! @brief Create a URL with the current data.
	QString createFrontendUrlLink() const;

	QString createShareLink() const;

	// ###########################################################################################################################################################################################################################################################################################################################

	// Setter / Getter

	void clear();

	void setDebug(bool _debug = true) { m_debug = _debug; };
	bool getDebug() const { return m_debug; };

	void setCheckGraphics(bool _check = true) { m_checkGraphics = _check; };
	bool getCheckGraphics() const { return m_checkGraphics; };

	//! @brief Eanbles automatic login with the provided data.
	void setLoginData(const LoginData& _data) { m_loginData = _data; };
	bool getLoginDataSet() const { return m_loginData.has_value(); };
	const LoginData& getLoginData() const { return m_loginData.value(); };

	//! @brief Enables automatic login with the provided GSS data.
	//! The users last stored credentials will be used for login.
	void setLoginGSS(const LogInGSSEntry& _gss) { m_loginGSS = _gss; };
	bool getLoginGSSSet() const { return m_loginGSS.has_value(); };
	const LogInGSSEntry& getLoginGSS() const { return m_loginGSS.value(); };

	void setProjectInfo(const ot::ProjectInformation& _info) { m_projectInfo = _info; };
	bool getProjectInfoSet() const { return m_projectInfo.has_value(); };
	const ot::ProjectInformation& getProjectInfo() const { return m_projectInfo.value(); };

	void setProjectVersion(const std::string& _version) { m_projectVersion = _version; };
	const std::string& getProjectVersion() const { return m_projectVersion; };

	void setScriptFile(const QString& _file) { m_scriptFile = _file; };
	const QString& getScriptFile() const { return m_scriptFile; };

	// ###########################################################################################################################################################################################################################################################################################################################

	// Private helper

private:
	bool parseUrl(const QString& _url);
	bool parseCommandLine();

	enum class ArgumentKey
	{
		Debug,
		CheckGraphics,
		CheckGraphicsShort,
		LogInData,
		LogInGss,
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

	std::optional<LoginData> m_loginData;
	std::optional<LogInGSSEntry> m_loginGSS;

	std::optional<ot::ProjectInformation> m_projectInfo;
	std::string m_projectVersion;

	QString m_scriptFile;
};