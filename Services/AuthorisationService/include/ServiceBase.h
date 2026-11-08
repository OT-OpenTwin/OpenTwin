// @otlicense
// File: ServiceBase.h
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

// Auth header
#include "SSO/SSOBuffer.h"
#include "Management/User.h"
#include "Management/Group.h"
#include "Management/Project.h"
#include "MongoDB/MongoConstants.h"
#include "MongoDB/MongoUserFunctions.h"
#include "MongoDB/MongoRoleFunctions.h"
#include "MongoDB/MongoGroupFunctions.h"
#include "MongoDB/MongoProjectFunctions.h"
#include "AccessToken/AccessTokenManager.h"

// OpenTwin header
#include "OTCommunication/CommunicationTypes.h"

// MongoDB header
#include <bsoncxx/json.hpp>
#include <bsoncxx/types.hpp>
#include <mongocxx/uri.hpp>
#include <mongocxx/stdx.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <bsoncxx/builder/stream/array.hpp>
#include <bsoncxx/builder/stream/helpers.hpp>
#include <bsoncxx/builder/stream/document.hpp>

// std header
#include <mutex>
#include <string>

class ServiceBase
{
	OT_DECL_NOCOPY(ServiceBase)
	OT_DECL_NOMOVE(ServiceBase)
public:
	static ServiceBase& instance();

	int initialize(const char * _ownIP, const char * _databaseIP, const char * _databasePWD);
	const char *getServiceURL();

	std::string performAction(const char * _json, const char * _senderIP);
	std::string performActionOneWayTLS(const char * _json, const char * _senderIP);

	std::string dispatchAction(const char * _json, ot::MessageType _messageType);
	mongocxx::client& getAdminClient() { return m_adminClient; }
	std::string& getDatabaseURL() { return m_databaseURL; }
private:

	enum class State
	{
		None = 0 << 0,
		Initialized = 1 << 0,
		DatabaseInitialized = 1 << 1
	};
	typedef ot::Flags<State> StateFlags;
	OT_ADD_FRIEND_FLAG_FUNCTIONS(State, StateFlags)
	
	AccessTokenManager m_accessTokenManager;
	std::string m_serviceURL;

	std::string m_databaseURL;
	std::string m_providedDbPassword; //! @brief The password provided by the user for the database connection. If no password is provided, the default password is used.
	std::string m_dbUsername; //! @brief The username used for the database connection. This is always "admin" for the authorization service.
	std::string m_dbPassword; //! @brief The password used for the database connection. This is either the provided password or the default password.
	
	mongocxx::instance m_mongoInstance;

	//! @brief The username of the admin user. This is always "admin" for the authorization service.
	//! @warning If this ever needs to be changed or made configurable, the setup must be performed before the database is initialized.
	std::string m_adminUserName;
	
	mongocxx::client m_adminClient;

	std::mutex m_mutex;
	const int m_dataBaseVersion = 1;

	SSOBuffer m_ssoBuffer;

	std::string dispatchAction(const std::string& _action, const ot::JsonDocument& _actionDocument, ot::MessageType _messageType);

	// ###########################################################################################################################################################################################################################################################################################################################

	// Action handler: No authentication required

	//api @action OT_ACTION_LOGIN_ADMIN 
	//api @brief Processes the login of an administrator. 
	//api @security TLS
	//api @param OT_PARAM_AUTH_USERNAME String User name
	//api @param OT_PARAM_AUTH_PASSWORD String User password
	//api @param OT_PARAM_AUTH_ENCRYPTED_PASSWORD Boolean Is the password encrypted 
	//api @rparam OT_ACTION_AUTH_SUCCESS Boolean Are the credentials correct
	//api @rparam OT_PARAM_AUTH_ENCRYPTED_PASSWORD String Encrypted password (if successful)
	std::string handleAdminLogIn(const ot::ConstJsonObject& _actionDocument);
	
	//api @action OT_ACTION_LOGIN
	//api @brief Processes the login of a user.
	//api Authenticates the user and, if successful, creates a new session including a temporary user. 
	//api @security TLS
	//api @param OT_PARAM_AUTH_USERNAME String User name
	//api @param OT_PARAM_AUTH_PASSWORD String User password
	//api @param OT_PARAM_AUTH_ENCRYPTED_PASSWORD Boolean Is the password encrypted 
	//api @rparam OT_ACTION_AUTH_SUCCESS Boolean Are the credentials correct
	//api @rparam OT_PARAM_AUTH_PASSWORD String Unencrypted password (if successful)
	//api @rparam OT_PARAM_AUTH_ENCRYPTED_PASSWORD String Encrypted password (if successful)
	std::string handleLogIn(const ot::ConstJsonObject& _actionDocument);


	std::string handleSSOTokenRefresh(const ot::ConstJsonObject& _actionDocument);

	std::string handleSSOTokenValidate(const ot::ConstJsonObject& _actionDocument);

	//api @action OT_ACTION_REGISTER
	//api @brief Registers a user. 
	//api Throws a std::runtime_error if the user name already exists as an admin user name in the database. 
	//api @security TLS
	//api @param OT_PARAM_AUTH_USERNAME String User name
	//api @param OT_PARAM_AUTH_PASSWORD String User password
	//api @rparam OT_ACTION_AUTH_SUCCESS Boolean Is the registration successfull
	std::string handleRegister(const ot::ConstJsonObject& _actionDocument);

	//api @action OT_ACTION_REFRESH_SESSION
	//api @brief Refreshes a user session.
	//api @security TLS
	//api @param OT_PARAM_DB_USERNAME String User name
	//api @return An empty String.
	std::string handleRefreshSession(const ot::ConstJsonObject& _actionDocument);

	//api @action OT_ACTION_CMD_SetGlobalLogFlags
	//api @brief Sets the global logging mode. 
	//api @security TLS
	//api @param OT_ACTION_PARAM_Flags Array Log flags 
	//api @rparam OT_ACTION_RETURN_VALUE_OK String Return value OK 
	std::string handleSetGlobalLoggingMode(const ot::ConstJsonObject& _actionDocument);

	// ###########################################################################################################################################################################################################################################################################################################################
	
	// Action handler: Authentication required: User functions

	//api @action OT_ACTION_GET_USER_DATA
	//api @brief Gets the user data from the database.
	//api @security TLS
	//api @param OT_PARAM_AUTH_USERNAME String User name 
	//api @return A String containing the user data.
	std::string handleGetUserData(const ot::ConstJsonObject& _actionDocument);

	//api @action OT_ACTION_GET_ALL_USERS
	//api @brief Gets all users from the database.
	//api @security TLS
	//api @return A String containing a list of all user data. 
	std::string handleGetAllUsers(const ot::ConstJsonObject& _actionDocument);

	//api @action OT_ACTION_GET_ALL_USER_COUNT
	//api @brief Counts all users in the database.
	//api @security TLS
	//api @return A String containing the count of all users.
	std::string handleGetAllUsersCount(const ot::ConstJsonObject& _actionDocument);
	std::string handleChangeUserNameByUser(const ot::ConstJsonObject& _actionDocument, const User& _loggedInUser, const std::string& _loggedInUserPassword);
	std::string handleChangeUserPasswordByUser(const ot::ConstJsonObject& _actionDocument, const User& _loggedInUser, const std::string& _loggedInUserPassword);
	std::string handleDeleteUser(const ot::ConstJsonObject& _actionDocument, User& _loggedInUser, const std::string& _loggedInUserPassword);
	std::string handleChangeUserNameByAdmin(const ot::ConstJsonObject& _actionDocument, User& _loggedInUser, const std::string& _loggedInUserPassword);
	std::string handleChangeUserPasswordByAdmin(const ot::ConstJsonObject& _actionDocument, User& _loggedInUser, const std::string& _loggedInUserPassword);
	std::string handleGetSystemInformation(const ot::ConstJsonObject& _actionDocument, User& _loggedInUser, const std::string& _loggedInUserPassword);
	ot::ReturnMessage handleGenerateAccessToken(const ot::ConstJsonObject& _actionDocument, User& _loggedInUser, const std::string& _loggedInUserPassword);

	// ###########################################################################################################################################################################################################################################################################################################################

	// Action handler: Authentication required: Group functions

	std::string handleCreateGroup(const ot::ConstJsonObject& _actionDocument, User& _loggedInUser);
	std::string handleGetGroupData(const ot::ConstJsonObject& _actionDocument);
	std::string handleGetAllUserGroups(const ot::ConstJsonObject& _actionDocument, User& _loggedInUser);
	std::string handleGetAllGroups(const ot::ConstJsonObject& _actionDocument, User& _loggedInUser);
	std::string handleGetAllGroupsCount(const ot::ConstJsonObject& _actionDocument, User& _loggedInUser);
	std::string handleChangeGroupName(const ot::ConstJsonObject& _actionDocument, User& _loggedInUser);
	std::string handleChangeGroupOwner(const ot::ConstJsonObject& _actionDocument, User& _loggedInUser);
	std::string handleAddUserToGroup(const ot::ConstJsonObject& _actionDocument, User& _loggedInUser);
	std::string handleRemoveUserFromGroup(const ot::ConstJsonObject& _actionDocument, User& _loggedInUser);
	std::string handleRemoveGroup(const ot::ConstJsonObject& _actionDocument, User& _loggedInUser);

	// ###########################################################################################################################################################################################################################################################################################################################

	// Action handler: Authentication required: Project functions

	std::string handleGetFilterOptions(const ot::ConstJsonObject& _actionDocument, User& _loggedInUser);
	std::string handleCreateProject(const ot::ConstJsonObject& _actionDocument, User& _loggedInUser);
	std::string handleProjectOpened(const ot::ConstJsonObject& _actionDocument, User& _loggedInUser);
	std::string handleUpdateAdditionalProjectInformation(const ot::ConstJsonObject& _actionDocument, User& _loggedInUser);
	
	//api @action OT_ACTION_GET_PROJECT_DATA
	//api @brief Gets the project data.
	//api @security TLS
	//api @param OT_PARAM_AUTH_PROJECT_NAME String Project name
	//api @return A String containing an ot::ReturnMessage.
	//api In case of success it will contain the project data. 
	std::string handleGetProjectData(const ot::ConstJsonObject& _actionDocument);
	std::string handleGetProjectsInfo(const ot::ConstJsonObject& _actionDocument, User& _loggedInUser);
	std::string handleGetAllUserProjects(const ot::ConstJsonObject& _actionDocument, User& _loggedInUser);
	std::string handleGetAllProjects(const ot::ConstJsonObject& _actionDocument, User& _loggedInUser);
	std::string handleGetAllProjectsCount(const ot::ConstJsonObject& _actionDocument, User& _loggedInUser);
	std::string handleGetAllGroupProjects(const ot::ConstJsonObject& _actionDocument, User& _loggedInUser);
	std::string handleChangeProjectName(const ot::ConstJsonObject& _actionDocument, User& _loggedInUser);
	std::string handleChangeProjectOwner(const ot::ConstJsonObject& _actionDocument, User& _loggedInUser);
	std::string handleAddGroupToProject(const ot::ConstJsonObject& _actionDocument, User& _loggedInUser);
	std::string handleRemoveGroupFromProject(const ot::ConstJsonObject& _actionDocument, User& _loggedInUser);
	std::string handleRemoveProject(const ot::ConstJsonObject& _actionDocument, User& _loggedInUser);
	std::string handleCheckIfCollectionExists(const ot::ConstJsonObject& _actionDocument, User& _loggedInUser);

	// ###########################################################################################################################################################################################################################################################################################################################

	// Private: Initialize functions

	//! @brief Will initialize the database.
	//! @warning Call this function only once during service startup.
	void initializeDatabase();

	// ###########################################################################################################################################################################################################################################################################################################################

	// Private: Helper

	inline bool isAdminUser(const User& _loggedInUser) const;
	inline bool isAdminUser(const std::string& _username) const;
	const std::string& getAdminUserName() const { return m_adminUserName; };
	std::string createRandomPassword();
	bool removeOldSessionsWorker();
	void setDatabaseVersion(int version);
	int getDatabaseVersion();

	ServiceBase() = default;
};

