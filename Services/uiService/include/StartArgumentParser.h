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
#include "LoginData.h"

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

	QStringList createCommandLineArgs() const;

	void setDebug(bool _debug = true) { m_debug = _debug; };
	bool getDebug() const { return m_debug; };

	void setCheckGraphics(bool _check = true) { m_checkGraphics = _check; };
	bool getCheckGraphics() const { return m_checkGraphics; };

	bool getLogInSet() const { return m_logIn; };
	void setLogInData(const LoginData& _data) { m_loginData = _data; m_logIn = true; };
	const LoginData& getLogInData() const { return m_loginData; };

	bool getOpenProjectSet() const { return m_openProject; };
	void setProjectInfo(const ot::ProjectInformation& _info) { m_projectInfo = _info; m_openProject = true; };
	const ot::ProjectInformation& getProjectInfo() const { return m_projectInfo; };

	void setProjectVersion(const std::string& _version) { m_projectVersion = _version; };
	const std::string& getProjectVersion() const { return m_projectVersion; };

private:
	bool m_debug = false;
	bool m_checkGraphics = false;

	bool m_logIn = false;
	LoginData m_loginData;

	bool m_openProject = false;
	ot::ProjectInformation m_projectInfo;
	std::string m_projectVersion;
};