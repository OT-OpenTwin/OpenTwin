// @otlicense
// File: UserManagement.h
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

// std header
#include <string>
#include <list>

// BSON header
#include <bsoncxx/document/view.hpp>
#include <bsoncxx/builder/basic/document.hpp>

class UserManagement
{
public:
	UserManagement();
	UserManagement(const LoginData& _loginData);
	~UserManagement();

	void setAuthServerURL(const std::string &url);
	void setDatabaseURL(const std::string &url);
	bool checkConnection(void) const;
	bool checkConnectionAuthorizationService(void) const;
	bool checkConnectionDataBase(const std::string &userName, const std::string &password) const;
	void initializeNewSession(void);

	bool addUser(const std::string &userName, const std::string &password) const;
	bool deleteUser(const std::string &userName) const;
	//bool changePassword(const std::string &oldPassword, const std::string &newPassword);

	bool checkUserName(const std::string &userName) const;
	bool checkPassword(const std::string &userName, const std::string &password, bool isEncryptedPassword, std::string& sessionUser, std::string& sessionPassword, std::string &validPassword, std::string &validEncryptedPassword) const;

	bool storeSetting(const std::string &settingName, const std::string &settingString);
	std::string restoreSetting(const std::string &settingName);

	bool addRecentProject(const std::string &projectName);
	bool removeRecentProject(const std::string &projectName);
	void getListOfRecentProjects(std::list<std::string> &recentProjectList);

	std::string getUserSettingsCollection(void);

private:
	bool hasError(const std::string& _response) const;
	bool hasSuccessful(const std::string& _response) const;
	bool initializeDatabaseConnection(void);
	bool ensureSettingsCollectionCanBeAccessed(void);
	bool storeListOfRecentProjects(std::list<std::string> &recentProjectList);

	const int m_maxNumberRecentProjects;

	bool m_isConnected;
	std::string m_authServerURL;
	std::string m_databaseURL;
	const std::string m_settingsDataBaseName;
	static std::string m_userSettingsCollection;
};
