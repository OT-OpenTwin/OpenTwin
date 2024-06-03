/*
 * UserManagement.h
 *
 *	Author: Peter Thoma
 *  Copyright (c) 2020 openTwin
 */

#pragma once

#include <string>
#include <list>

#include <bsoncxx/document/view.hpp>
#include <bsoncxx/builder/basic/document.hpp>

class UserManagement
{
public:
	UserManagement();
	~UserManagement();

	void setAuthServerURL(const std::string &url);
	void setDatabaseURL(const std::string &url);
	bool checkConnection(void);
	bool checkConnectionAuthorizationService(void);
	bool checkConnectionDataBase(const std::string &userName, const std::string &password);
	void initializeNewSession(void);

	bool addUser(const std::string &userName, const std::string &password);
	bool deleteUser(const std::string &userName);
	//bool changePassword(const std::string &oldPassword, const std::string &newPassword);

	bool checkUserName(const std::string &userName);
	bool checkPassword(const std::string &userName, const std::string &password, bool isEncryptedPassword, std::string& sessionUser, std::string& sessionPassword, std::string &validPassword, std::string &validEncryptedPassword);

	bool storeSetting(const std::string &settingName, const std::string &settingString);
	std::string restoreSetting(const std::string &settingName);

	bool addRecentProject(const std::string &projectName);
	bool removeRecentProject(const std::string &projectName);
	void getListOfRecentProjects(std::list<std::string> &recentProjectList);

	std::string getUserSettingsCollection(void);

private:
	bool hasError(const std::string &response);
	bool hasSuccessful(const std::string &response);
	bool initializeDatabaseConnection(void);
	bool ensureSettingsCollectionCanBeAccessed(void);
	bool storeListOfRecentProjects(std::list<std::string> &recentProjectList);

	const int maxNumberRecentProjects;

	bool isConnected;
	std::string authServerURL;
	std::string databaseURL;
	const std::string settingsDataBaseName;
	static std::string userSettingsCollection;
};
