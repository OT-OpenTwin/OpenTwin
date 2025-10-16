//! @file StartArgumentParser.h
//! @author Alexander Kuester (alexk95)
//! @date October 2025
// ###########################################################################################################################################################################################################################################################################################################################

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
	StartArgumentParser();
	~StartArgumentParser() = default;

	//! @brief Import the configuration from a temporary file.
	//! The file is deleted after reading.
	//! @return True if no file existed or the file was read successfully, false otherwise.
	bool importConfig();

	//! @brief Export the configuration to a temporary file.
	//! @return True if the file was written successfully, false otherwise.
	bool exportConfig() const;

	bool getLogInSet() const { return m_logIn; };
	void setLogInData(const LoginData& _data) { m_loginData = _data; m_logIn = true; };
	const LoginData& getLogInData() const { return m_loginData; };

	bool getOpenProjectSet() const { return m_openProject; };
	void setProjectInfo(const ot::ProjectInformation& _info) { m_projectInfo = _info; m_openProject = true; };
	const ot::ProjectInformation& getProjectInfo() const { return m_projectInfo; };

private:
	
	bool m_logIn;
	LoginData m_loginData;

	bool m_openProject;
	ot::ProjectInformation m_projectInfo;
};