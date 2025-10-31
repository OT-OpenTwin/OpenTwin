// @otlicense
// File: ServiceBase.cpp
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

// Service header
#include "ServiceBase.h"
#include "MongoURL.h"
#include "MongoSessionFunctions.h"

// OpenTwin header
#include "OTSystem/AppExitCodes.h"
#include "OTCore/LogDispatcher.h"
#include "OTCore/ReturnMessage.h"
#include "OTCommunication/Msg.h"
#include "OTCommunication/ActionTypes.h"
#include "OTServiceFoundation/UserCredentials.h"
#include "Connection/ConnectionAPI.h"

// std header
#include <random>
#include <iostream>

#define DB_ERROR_MESSAGE_ALREADY_EXISTS "already exists: generic server error"

int ServiceBase::initialize(const char * _ownIP, const char * _databaseIP, const char * _databasePWD)
{
	m_serviceURL  = _ownIP;
	m_databaseURL = _databaseIP;
	if (m_databaseURL.find("tls@") == 0)
	{
		m_databaseURL = m_databaseURL.substr(4);
	}
	m_databasePWD = _databasePWD;

	OT_LOG_I("Starting Authorisation Service");
	OT_LOG_D("Service address : " + m_serviceURL);
	OT_LOG_D("Database address: " + m_databaseURL);

	initializeDatabase();

	// Remove outdated sessions and start time
	std::thread workerThread(&ServiceBase::removeOldSessionsWorker, this);
	workerThread.detach();

	return ot::AppExitCode::Success;
}

const char *ServiceBase::getServiceURL()
{
	return m_serviceURL.c_str();
}

std::string ServiceBase::performAction(const char * _json, const char * _senderIP)
{	
	return dispatchAction(_json, ot::EXECUTE);
}

std::string ServiceBase::performActionOneWayTLS(const char * _json, const char * _senderIP)
{
	return dispatchAction(_json, ot::EXECUTE_ONE_WAY_TLS);
}

std::string ServiceBase::dispatchAction(const char * _json, ot::MessageType _messageType) {
	// Parse the json string and check its main syntax
	ot::JsonDocument actionDoc;
	actionDoc.fromJson(_json);

	if (!actionDoc.IsObject()) {
		OTAssert(0, "Received message is not an JSON object");
		return OT_ACTION_RETURN_INDICATOR_Error "Received message is not an JSON object";
	}
	if (!actionDoc.HasMember(OT_ACTION_MEMBER)) {
		OTAssert(0, "Received JSON Object does not contain the action member");
		return OT_ACTION_RETURN_INDICATOR_Error "Received JSON Object does not contain the action member";
	}
	if (!actionDoc[OT_ACTION_MEMBER].IsString()) {
		OTAssert(0, "Received JSON Object's action member is not a string");
		return OT_ACTION_RETURN_INDICATOR_Error "Received JSON Object's action member is not a string";
	}
	std::string action = actionDoc[OT_ACTION_MEMBER].GetString();

	// Ping may be multithreaded, others have to be handled one by one
	if (action == OT_ACTION_PING) { return OT_ACTION_PING; }

	try {
		std::lock_guard<std::mutex> mtxLock(m_mutex);

		OT_LOG("Dispatchig action: \"" + action + "\"", ot::messageTypeToLogFlag(_messageType));
		std::string response = dispatchAction(action, actionDoc, _messageType);
		OT_LOG(".. Completed: Dispatchig action: \"" + action + "\": Returning: \"" + response + "\"", ot::messageTypeToLogFlag(_messageType));
		return response;
	}
	catch (std::exception _e)
	{
		OT_LOG_E(_e.what());

		ot::JsonDocument json;
		json.AddMember("error", 1, json.GetAllocator());
		json.AddMember("description", ot::JsonString(_e.what(), json.GetAllocator()), json.GetAllocator());

		return json.toJson();
	}
	catch (...) {
		return OT_ACTION_RETURN_UnknownError;
	}
}

std::string ServiceBase::dispatchAction(const std::string& _action, const ot::JsonDocument& _actionDocument, ot::MessageType _messageType)
{
	//------------ USER FUNCTIONS THAT DO NOT NEED AUTHENTICATION ------------------------
	if (_action == OT_ACTION_LOGIN_ADMIN) { return handleAdminLogIn(_actionDocument.GetObject()); }
	else if (_action == OT_ACTION_LOGIN) { return handleLogIn(_actionDocument.GetObject()); }
	else if (_action == OT_ACTION_REGISTER) { return handleRegister(_actionDocument.GetObject()); }
	else if (_action == OT_ACTION_REFRESH_SESSION) { return handleRefreshSession(_actionDocument.GetObject()); }
	else if (_action == OT_ACTION_CMD_SetGlobalLogFlags) { return handleSetGlobalLoggingMode(_actionDocument.GetObject()); }

	//------------------------------------------------------------------------------------

	// Checking whether the logged in user is the one that he claims to be. All the requests must include loggedInUsername and loggedInUserPassword
	std::string loggedInUsername = ot::json::getString(_actionDocument, OT_PARAM_AUTH_LOGGED_IN_USERNAME);
	std::string loggedInUserPassword = ot::json::getString(_actionDocument, OT_PARAM_AUTH_LOGGED_IN_USER_PASSWORD);

	User loggedInUser;
	loggedInUser.username = loggedInUsername;
	
	if (!isAdminUser(loggedInUser))
	{
		bool logInSuccessful = MongoUserFunctions::authenticateUser(loggedInUsername, loggedInUserPassword, m_databaseURL, m_adminClient);

		if (!logInSuccessful)
		{
			assert(0);
			throw std::runtime_error("The User could not be authenticated! Please logout and attempt to log in again!");
		}

		loggedInUser = MongoUserFunctions::getUserDataThroughUsername(loggedInUsername, m_adminClient);
	}

	//------------ FUNCTIONS THAT NEED AUTHENTICATION ------------
	if (_action == OT_ACTION_GET_USER_DATA) { return handleGetUserData(_actionDocument.GetObject()); }
	else if (_action == OT_ACTION_GET_ALL_USERS) { return handleGetAllUsers(_actionDocument.GetObject()); }
	else if (_action == OT_ACTION_GET_ALL_USER_COUNT) { return handleGetAllUsersCount(_actionDocument.GetObject()); }
	else if (_action == OT_ACTION_CHANGE_USER_USERNAME) { return handleChangeUserNameByUser(_actionDocument.GetObject(), loggedInUser, loggedInUserPassword); }
	else if (_action == OT_ACTION_CHANGE_USER_PASSWORD) { return handleChangeUserPasswordByUser(_actionDocument.GetObject(), loggedInUser, loggedInUserPassword); }
	else if (_action == OT_ACTION_DELETE_USER) { return handleDeleteUser(_actionDocument.GetObject(), loggedInUser, loggedInUserPassword); }
	else if (_action == OT_ACTION_CHANGE_USERNAME) { return handleChangeUserNameByAdmin(_actionDocument.GetObject(), loggedInUser, loggedInUserPassword); }
	else if (_action == OT_ACTION_CHANGE_PASSWORD) { return handleChangeUserPasswordByAdmin(_actionDocument.GetObject(), loggedInUser, loggedInUserPassword); }
	else if (_action == OT_ACTION_CMD_GetSystemInformation) { return handleGetSystemInformation(_actionDocument.GetObject(), loggedInUser, loggedInUserPassword); }
	//------------ Group FUNCTIONS ------------
	else if (_action == OT_ACTION_CREATE_GROUP) { return handleCreateGroup(_actionDocument.GetObject(), loggedInUser); }
	else if (_action == OT_ACTION_GET_GROUP_DATA) { return handleGetGroupData(_actionDocument.GetObject()); }
	else if (_action == OT_ACTION_GET_ALL_USER_GROUPS) { return handleGetAllUserGroups(_actionDocument.GetObject(), loggedInUser); }
	else if (_action == OT_ACTION_GET_ALL_GROUPS) { return handleGetAllGroups(_actionDocument.GetObject(), loggedInUser); }
	else if (_action == OT_ACTION_GET_ALL_GROUP_COUNT) { return handleGetAllGroupsCount(_actionDocument.GetObject(), loggedInUser); }
	else if (_action == OT_ACTION_CHANGE_GROUP_NAME) { return handleChangeGroupName(_actionDocument.GetObject(), loggedInUser); }
	else if (_action == OT_ACTION_CHANGE_GROUP_OWNER) { return handleChangeGroupOwner(_actionDocument.GetObject(), loggedInUser); }
	else if (_action == OT_ACTION_ADD_USER_TO_GROUP) { return handleAddUserToGroup(_actionDocument.GetObject(), loggedInUser); }
	else if (_action == OT_ACTION_REMOVE_USER_FROM_GROUP) { return handleRemoveUserFromGroup(_actionDocument.GetObject(), loggedInUser); }
	else if (_action == OT_ACTION_REMOVE_GROUP) { return handleRemoveGroup(_actionDocument.GetObject(), loggedInUser); }
	//------------ Project FUNCTIONS ------------
	else if (_action == OT_ACTION_CREATE_PROJECT) { return handleCreateProject(_actionDocument.GetObject(), loggedInUser); }
	else if (_action == OT_ACTION_CMD_OpenNewProject) { return handleProjectOpened(_actionDocument.GetObject(), loggedInUser); }
	else if (_action == OT_ACTION_UPDATE_PROJECT_ADDITIONALINFO) { return handleUpdateAdditionalProjectInformation(_actionDocument.GetObject(), loggedInUser); }
	//                                                       v-- CAN BE PERFORMED BY THE UI CLIENT --v
	else if (_action == OT_ACTION_GET_PROJECT_DATA) { return handleGetProjectData(_actionDocument.GetObject()); }
	else if (_action == OT_ACTION_GET_ALL_PROJECT_INFO) { return handleGetProjectsInfo(_actionDocument.GetObject(), loggedInUser); }
	else if (_action == OT_ACTION_GET_ALL_USER_PROJECTS) { return handleGetAllUserProjects(_actionDocument.GetObject(), loggedInUser); }
	else if (_action == OT_ACTION_GET_ALL_PROJECTS) { return handleGetAllProjects(_actionDocument.GetObject(), loggedInUser); }
	else if (_action == OT_ACTION_GET_ALL_PROJECT_COUNT) { return handleGetAllProjectsCount(_actionDocument.GetObject(), loggedInUser); }
	else if (_action == OT_ACTION_GET_ALL_GROUP_PROJECTS) { return handleGetAllGroupProjects(_actionDocument.GetObject(), loggedInUser); }
	else if (_action == OT_ACTION_CHANGE_PROJECT_NAME) { return handleChangeProjectName(_actionDocument.GetObject(), loggedInUser); }
	else if (_action == OT_ACTION_CHANGE_PROJECT_OWNER) { return handleChangeProjectOwner(_actionDocument.GetObject(), loggedInUser); }
	else if (_action == OT_ACTION_ADD_GROUP_TO_PROJECT) { return handleAddGroupToProject(_actionDocument.GetObject(), loggedInUser); }
	else if (_action == OT_ACTION_REMOVE_GROUP_FROM_PROJECT) { return handleRemoveGroupFromProject(_actionDocument.GetObject(), loggedInUser); }
	else if (_action == OT_ACTION_REMOVE_PROJECT) { return handleRemoveProject(_actionDocument.GetObject(), loggedInUser); }
	else if (_action == OT_ACTION_CHECK_FOR_COLLECTION_EXISTENCE) { return handleCheckIfCollectionExists(_actionDocument.GetObject(), loggedInUser); }
	else
	{
		// This action is unknown
		assert(0);
		return OT_ACTION_RETURN_UnknownAction;
	}
}

// ###########################################################################################################################################################################################################################################################################################################################

// Action handler: No authentication required

std::string ServiceBase::handleAdminLogIn(const ot::ConstJsonObject& _actionDocument) {
	std::string username = ot::json::getString(_actionDocument, OT_PARAM_AUTH_USERNAME);
	std::string password = ot::json::getString(_actionDocument, OT_PARAM_AUTH_PASSWORD);
	bool encryptedPassword = ot::json::getBool(_actionDocument, OT_PARAM_AUTH_ENCRYPTED_PASSWORD);

	if (encryptedPassword)
	{
		password = ot::UserCredentials::decryptString(password);
	}

	bool successful = (username == m_dbUsername && password == ot::UserCredentials::decryptString(m_dbPassword));

	ot::JsonDocument json;
	json.AddMember(OT_ACTION_AUTH_SUCCESS, successful, json.GetAllocator());

	if (successful)
	{
		json.AddMember(OT_PARAM_AUTH_ENCRYPTED_PASSWORD, ot::JsonString(ot::UserCredentials::encryptString(password), json.GetAllocator()), json.GetAllocator());
	}

	return json.toJson();
}

std::string ServiceBase::handleLogIn(const ot::ConstJsonObject& _actionDocument) {
	std::string username = ot::json::getString(_actionDocument, OT_PARAM_AUTH_USERNAME);
	std::string password = ot::json::getString(_actionDocument, OT_PARAM_AUTH_PASSWORD);
	bool encryptedPassword = ot::json::getBool(_actionDocument, OT_PARAM_AUTH_ENCRYPTED_PASSWORD);

	if (encryptedPassword)
	{
		password = ot::UserCredentials::decryptString(password);
	}

	bool successful = MongoUserFunctions::authenticateUser(username, password, m_databaseURL, m_adminClient);

	ot::JsonDocument json;
	json.AddMember(OT_ACTION_AUTH_SUCCESS, successful, json.GetAllocator());
	
	if (successful)
	{
		std::string sessionName = MongoSessionFunctions::createSession(username, m_adminClient);
		std::string sessionPWD  = createRandomPassword();

		User user = MongoUserFunctions::getUserDataThroughUsername(username, m_adminClient);

		MongoUserFunctions::createTmpUser(sessionName, sessionPWD, user, m_adminClient, json);

		json.AddMember(OT_PARAM_AUTH_PASSWORD, ot::JsonString(password, json.GetAllocator()), json.GetAllocator());
		json.AddMember(OT_PARAM_AUTH_ENCRYPTED_PASSWORD, ot::JsonString(ot::UserCredentials::encryptString(password), json.GetAllocator()), json.GetAllocator());
	}

	return json.toJson();
}

std::string ServiceBase::handleRegister(const ot::ConstJsonObject& _actionDocument) {
	std::string username = ot::json::getString(_actionDocument, OT_PARAM_AUTH_USERNAME);
	std::string password = ot::json::getString(_actionDocument, OT_PARAM_AUTH_PASSWORD);

	if (username == getAdminUserName())
	{
		throw std::runtime_error("This user name is reserved for the database admin and cannot be used for a regular user!");
	}

	bool successful = MongoUserFunctions::registerUser(username, password, m_adminClient);

	ot::JsonDocument json;
	json.AddMember(OT_ACTION_AUTH_SUCCESS, successful, json.GetAllocator());
	return json.toJson();
}

std::string ServiceBase::handleRefreshSession(const ot::ConstJsonObject& _actionDocument) {
	std::string session = ot::json::getString(_actionDocument, OT_PARAM_DB_USERNAME);

	return MongoSessionFunctions::refreshSession(session, m_adminClient);
}

std::string ServiceBase::handleSetGlobalLoggingMode(const ot::ConstJsonObject& _actionDocument) {
	ot::ConstJsonArray flags = ot::json::getArray(_actionDocument, OT_ACTION_PARAM_Flags);
	ot::LogDispatcher::instance().setLogFlags(ot::logFlagsFromJsonArray(flags));
	return OT_ACTION_RETURN_VALUE_OK;
}

// ###########################################################################################################################################################################################################################################################################################################################

// Action handler: Authentication required: User functions

std::string ServiceBase::handleGetUserData(const ot::ConstJsonObject& _actionDocument) {
	std::string username = ot::json::getString(_actionDocument, OT_PARAM_AUTH_USERNAME);
	User usr = MongoUserFunctions::getUserDataThroughUsername(username, m_adminClient);

	return MongoUserFunctions::userToJson(usr);
}

std::string ServiceBase::handleGetAllUsers(const ot::ConstJsonObject& _actionDocument) {
	std::vector<User> allUsers = MongoUserFunctions::getAllUsers(m_adminClient);

	return MongoUserFunctions::usersToJson(allUsers);
}

std::string ServiceBase::handleGetAllUsersCount(const ot::ConstJsonObject& _actionDocument) {
	size_t count = MongoUserFunctions::getAllUserCount(m_adminClient);
	return std::to_string(count);
}

std::string ServiceBase::handleChangeUserNameByUser(const ot::ConstJsonObject& _actionDocument, const User& _loggedInUser, const std::string& _loggedInUserPassword) {
	std::string newUsername = ot::json::getString(_actionDocument, OT_PARAM_AUTH_NEW_USERNAME);
	bool successful = MongoUserFunctions::updateUserUsernameByName(_loggedInUser.username, newUsername, m_adminClient);

	ot::JsonDocument json;
	json.AddMember(OT_ACTION_AUTH_SUCCESS, successful, json.GetAllocator());
	return json.toJson();
}

std::string ServiceBase::handleChangeUserPasswordByUser(const ot::ConstJsonObject& _actionDocument, const User& _loggedInUser, const std::string& _loggedInUserPassword) {
	std::string password = ot::json::getString(_actionDocument, OT_PARAM_AUTH_PASSWORD);
	bool successful = MongoUserFunctions::changeUserPassword(_loggedInUser.username, password, m_adminClient);

	ot::JsonDocument json;
	json.AddMember(OT_ACTION_AUTH_SUCCESS, successful, json.GetAllocator());
	return json.toJson();
}

std::string ServiceBase::handleDeleteUser(const ot::ConstJsonObject& _actionDocument, User& _loggedInUser, const std::string& _loggedInUserPassword) {
	std::string usernameToDelete = ot::json::getString(_actionDocument, OT_PARAM_USERNAME_TO_DELETE);

	User toBeDeletedUser = MongoUserFunctions::getUserDataThroughUsername(usernameToDelete, m_adminClient);

	bool successful = false;
	// Check if the user wants do delete himself or if the call comes from the Admin User
	if (_loggedInUser.userId == toBeDeletedUser.userId || isAdminUser(_loggedInUser))
	{
		successful = MongoUserFunctions::removeUser(toBeDeletedUser, m_adminClient);
	}

	ot::JsonDocument json;
	json.AddMember(OT_ACTION_AUTH_SUCCESS, successful, json.GetAllocator());
	return json.toJson();
}

std::string ServiceBase::handleChangeUserNameByAdmin(const ot::ConstJsonObject& _actionDocument, User& _loggedInUser, const std::string& _loggedInUserPassword) {
	if (!isAdminUser(_loggedInUser))
	{
		throw std::runtime_error("The logged in user is not an admin user. Standard users can not change the username of an arbitrary user.");
	}

	std::string oldUsername = ot::json::getString(_actionDocument, OT_PARAM_AUTH_OLD_USERNAME);
	std::string newUsername = ot::json::getString(_actionDocument, OT_PARAM_AUTH_NEW_USERNAME);

	bool successful = MongoUserFunctions::updateUsername(oldUsername, newUsername, m_adminClient);

	ot::JsonDocument json;
	json.AddMember(OT_ACTION_AUTH_SUCCESS, successful, json.GetAllocator());
	return json.toJson();
}

std::string ServiceBase::handleChangeUserPasswordByAdmin(const ot::ConstJsonObject& _actionDocument, User& _loggedInUser, const std::string& _loggedInUserPassword) {
	if (!isAdminUser(_loggedInUser))
	{
		throw std::runtime_error("The logged in user is not an admin user. Standard users can not change the password of an arbitrary user.");
	}

	std::string userName = ot::json::getString(_actionDocument, OT_PARAM_AUTH_USERNAME);
	std::string newPassword = ot::json::getString(_actionDocument, OT_PARAM_AUTH_NEW_PASSWORD);

	bool successful = MongoUserFunctions::changeUserPassword(userName, newPassword, m_adminClient);

	ot::JsonDocument json;
	json.AddMember(OT_ACTION_AUTH_SUCCESS, successful, json.GetAllocator());
	return json.toJson();
}

std::string ServiceBase::handleGetSystemInformation(const ot::ConstJsonObject& _actionDocument, User& _loggedInUser, const std::string& _loggedInUserPassword) {
	if (!isAdminUser(_loggedInUser))
	{
		throw std::runtime_error("The logged in user is not an admin user. Standard users can not request system information.");
	}

	std::string gssUrl = ot::json::getString(_actionDocument, OT_ACTION_PARAM_SERVICE_URL);

	ot::JsonDocument requestDoc;
	requestDoc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_GetSystemInformation, requestDoc.GetAllocator()), requestDoc.GetAllocator());

	std::string response;
	if (!ot::msg::send(getServiceURL(), gssUrl, ot::EXECUTE, requestDoc.toJson(), response, ot::msg::defaultTimeout, ot::msg::DefaultFlagsNoExit)) {
		OT_LOG_E("Failed to retrieve information. Reason: Failed to send http request to GSS (URL = \"" + gssUrl + "\")");
		return "";
	}

	return response;
}

// ###########################################################################################################################################################################################################################################################################################################################

// Action handler: Authentication required: Group functions

std::string ServiceBase::handleCreateGroup(const ot::ConstJsonObject& _actionDocument, User& _loggedInUser) {
	std::string groupName = ot::json::getString(_actionDocument, OT_PARAM_AUTH_GROUP_NAME);

	MongoGroupFunctions::createGroup(groupName, _loggedInUser, m_adminClient);
	Group gr = MongoGroupFunctions::getGroupDataByName(groupName, m_adminClient);

	return MongoGroupFunctions::groupToJson(gr);
}

std::string ServiceBase::handleGetGroupData(const ot::ConstJsonObject& _actionDocument) {
	std::string groupName = ot::json::getString(_actionDocument, OT_PARAM_AUTH_GROUP_NAME);

	Group gr = MongoGroupFunctions::getGroupDataByName(groupName, m_adminClient);

	return MongoGroupFunctions::groupToJson(gr);
}

std::string ServiceBase::handleGetAllUserGroups(const ot::ConstJsonObject& _actionDocument, User& _loggedInUser) {
	std::vector<Group> groups = MongoGroupFunctions::getAllUserGroups(_loggedInUser, m_adminClient);

	return MongoGroupFunctions::groupsToJson(groups);
}

std::string ServiceBase::handleGetAllGroups(const ot::ConstJsonObject& _actionDocument, User& _loggedInUser) {
	if (!isAdminUser(_loggedInUser))
	{
		throw std::runtime_error("The logged in user is not an admin user. Standard users can not retrieve a list of all groups.");
	}

	std::vector<Group> groups = MongoGroupFunctions::getAllGroups(_loggedInUser, m_adminClient);

	return MongoGroupFunctions::groupsToJson(groups);
}

std::string ServiceBase::handleGetAllGroupsCount(const ot::ConstJsonObject& _actionDocument, User& _loggedInUser) {
	size_t count = MongoGroupFunctions::getAllGroupCount(_loggedInUser, m_adminClient);
	return std::to_string(count);
}

std::string ServiceBase::handleChangeGroupName(const ot::ConstJsonObject& _actionDocument, User& _loggedInUser) {
	std::string groupName = ot::json::getString(_actionDocument, OT_PARAM_AUTH_GROUP_NAME);
	std::string newGroupName = ot::json::getString(_actionDocument, OT_PARAM_AUTH_NEW_GROUP_NAME);

	Group gr = MongoGroupFunctions::getGroupDataByName(groupName, m_adminClient);

	bool successful = false;

	if (gr.ownerUserId == _loggedInUser.userId || isAdminUser(_loggedInUser))
	{
		successful = MongoGroupFunctions::changeGroupName(groupName, newGroupName, m_adminClient);
	}
	else
	{
		throw std::runtime_error("The logged in user is not the owner of this group! He cannot make the requested changes!");
	}

	ot::JsonDocument json;
	json.AddMember(OT_ACTION_AUTH_SUCCESS, successful, json.GetAllocator());
	return json.toJson();
}

std::string ServiceBase::handleChangeGroupOwner(const ot::ConstJsonObject& _actionDocument, User& _loggedInUser) {
	std::string groupName = ot::json::getString(_actionDocument, OT_PARAM_AUTH_GROUP_NAME);
	std::string newOwnerusername = ot::json::getString(_actionDocument, OT_PARAM_AUTH_GROUP_OWNER_NEW_USER_USERNAME);

	User newOwner = MongoUserFunctions::getUserDataThroughUsername(newOwnerusername, m_adminClient);

	Group group = MongoGroupFunctions::getGroupDataByName(groupName, m_adminClient);

	bool successful = false;
	if (group.ownerUserId == _loggedInUser.userId || isAdminUser(_loggedInUser))
	{
		successful = MongoGroupFunctions::changeGroupOwner(group, _loggedInUser, newOwner, m_adminClient);
	}
	else
	{
		throw std::runtime_error("The logged in user is not the owner of this group! He cannot make the requested changes!");
	}

	ot::JsonDocument json;
	json.AddMember(OT_ACTION_AUTH_SUCCESS, successful, json.GetAllocator());
	return json.toJson();
}

std::string ServiceBase::handleAddUserToGroup(const ot::ConstJsonObject& _actionDocument, User& _loggedInUser) {
	std::string username = ot::json::getString(_actionDocument, OT_PARAM_AUTH_USERNAME);
	std::string groupName = ot::json::getString(_actionDocument, OT_PARAM_AUTH_GROUP_NAME);
	Group gr = MongoGroupFunctions::getGroupDataByName(groupName, m_adminClient);


	bool successful = false;
	if (gr.ownerUserId == _loggedInUser.userId)
	{
		User usr = MongoUserFunctions::getUserDataThroughUsername(username, m_adminClient);
		successful = MongoGroupFunctions::addUserToGroup(usr, groupName, m_adminClient);
	}
	else
	{
		throw std::runtime_error("The logged in user is not the owner of this group! He cannot make the requested changes!");
	}

	ot::JsonDocument json;
	json.AddMember(OT_ACTION_AUTH_SUCCESS, successful, json.GetAllocator());
	return json.toJson();
}

std::string ServiceBase::handleRemoveUserFromGroup(const ot::ConstJsonObject& _actionDocument, User& _loggedInUser) {
	std::string username = ot::json::getString(_actionDocument, OT_PARAM_AUTH_USERNAME);
	std::string groupName = ot::json::getString(_actionDocument, OT_PARAM_AUTH_GROUP_NAME);
	Group gr = MongoGroupFunctions::getGroupDataByName(groupName, m_adminClient);

	bool successful = false;
	if (gr.ownerUserId == _loggedInUser.userId)
	{
		User usr = MongoUserFunctions::getUserDataThroughUsername(username, m_adminClient);
		successful = MongoGroupFunctions::removeUserFromGroup(usr, groupName, m_adminClient);
	}
	else
	{
		throw std::runtime_error("The logged in user is not the owner of this group! He cannot make the requested changes!");
	}

	ot::JsonDocument json;
	json.AddMember(OT_ACTION_AUTH_SUCCESS, successful, json.GetAllocator());
	return json.toJson();
}

std::string ServiceBase::handleRemoveGroup(const ot::ConstJsonObject& _actionDocument, User& _loggedInUser) {
	std::string groupName = ot::json::getString(_actionDocument, OT_PARAM_AUTH_GROUP_NAME);

	auto group = MongoGroupFunctions::getGroupDataByName(groupName, m_adminClient);

	auto groupOwnerId = group.ownerUserId;
	auto requestingUserId = _loggedInUser.userId;

	bool successful = false;
	if (groupOwnerId == requestingUserId || isAdminUser(_loggedInUser))
	{
		successful = MongoGroupFunctions::removeGroup(group, m_adminClient);
	}
	else
	{
		throw std::runtime_error("The logged in user is not the owner of this group! He cannot make the requested changes!");
	}

	ot::JsonDocument json;
	json.AddMember(OT_ACTION_AUTH_SUCCESS, successful, json.GetAllocator());
	return json.toJson();
}

// ###########################################################################################################################################################################################################################################################################################################################

// Action handler: Authentication required: Project functions

std::string ServiceBase::handleGetFilterOptions(const ot::ConstJsonObject& _actionDocument, User& _loggedInUser) {
	ot::ReturnMessage ret(ot::ReturnMessage::Ok);
	ot::JsonDocument optionsDoc;
	ot::ProjectFilterData filterOptions = MongoProjectFunctions::getProjectFilterOptions(_loggedInUser, m_adminClient);

	filterOptions.addToJsonObject(optionsDoc, optionsDoc.GetAllocator());
	ret.setWhat(optionsDoc.toJson());
	return ret.toJson();
}

std::string ServiceBase::handleCreateProject(const ot::ConstJsonObject& _actionDocument, User& _loggedInUser) {
	std::string projectName = ot::json::getString(_actionDocument, OT_PARAM_AUTH_PROJECT_NAME);
	std::string projectType = ot::json::getString(_actionDocument, OT_PARAM_AUTH_PROJECT_TYPE);

	Project createdProject = MongoProjectFunctions::createProject(projectName, projectType, _loggedInUser, m_adminClient);

	return ot::ReturnMessage(ot::ReturnMessage::Ok, createdProject.toProjectInformation().toJson()).toJson();
}

std::string ServiceBase::handleProjectOpened(const ot::ConstJsonObject& _actionDocument, User& _loggedInUser) {
	std::string projectName = ot::json::getString(_actionDocument, OT_PARAM_AUTH_PROJECT_NAME);

	MongoProjectFunctions::projectWasOpened(projectName, m_adminClient);

	return ot::ReturnMessage::toJson(ot::ReturnMessage::Ok);
}

std::string ServiceBase::handleUpdateAdditionalProjectInformation(const ot::ConstJsonObject& _actionDocument, User& _loggedInUser) {
	ot::ProjectInformation info(ot::json::getObject(_actionDocument, OT_ACTION_PARAM_Config));

	return MongoProjectFunctions::updateAdditionalProjectInformation(info, m_adminClient).toJson();
}

std::string ServiceBase::handleGetProjectData(const ot::ConstJsonObject& _actionDocument) {
	std::string projectName = ot::json::getString(_actionDocument, OT_PARAM_AUTH_PROJECT_NAME);

	try
	{
		Project proj = MongoProjectFunctions::getProject(projectName, m_adminClient);
		ot::ReturnMessage rMsg(ot::ReturnMessage::Ok, proj.toProjectInformation().toJson());
		return rMsg.toJson();
	}
	catch (std::runtime_error& error)
	{
		ot::ReturnMessage rMsg(ot::ReturnMessage::Failed, error.what());
		return rMsg.toJson();
	}
}

std::string ServiceBase::handleGetProjectsInfo(const ot::ConstJsonObject& _actionDocument, User& _loggedInUser) {
	auto projectNames = ot::json::getStringList(_actionDocument, OT_PARAM_AUTH_PROJECT_NAMES);
	return MongoProjectFunctions::getProjectsInfo(projectNames, m_adminClient);
}

std::string ServiceBase::handleGetAllUserProjects(const ot::ConstJsonObject& _actionDocument, User& _loggedInUser) {
	std::vector<Project> projects;
	int limit = ot::json::getInt(_actionDocument, OT_PARAM_AUTH_PROJECT_LIMIT);

	if (_actionDocument.HasMember(OT_PARAM_AUTH_PROJECT_FILTER)) {
		std::string filter = ot::json::getString(_actionDocument, OT_PARAM_AUTH_PROJECT_FILTER);
		projects = MongoProjectFunctions::getAllUserProjects(_loggedInUser, filter, limit, m_adminClient);
	}
	else if (_actionDocument.HasMember(OT_ACTION_PARAM_Config)) {
		ot::ProjectFilterData filterData(ot::json::getObject(_actionDocument, OT_ACTION_PARAM_Config));
		projects = MongoProjectFunctions::getAllUserProjects(_loggedInUser, filterData, limit, m_adminClient);
	}
	else {
		OT_LOG_E("No filter provided for getting all user projects. Returning empty list.");
	}

	return MongoProjectFunctions::projectsToJson(projects);
}

std::string ServiceBase::handleGetAllProjects(const ot::ConstJsonObject& _actionDocument, User& _loggedInUser) {
	if (!isAdminUser(_loggedInUser))
	{
		throw std::runtime_error("The logged in user is not an admin user. Standard users can not retrieve a list of all projects.");
	}

	std::string filter = ot::json::getString(_actionDocument, OT_PARAM_AUTH_PROJECT_FILTER);
	int limit = ot::json::getInt(_actionDocument, OT_PARAM_AUTH_PROJECT_LIMIT);

	std::vector<Project> projects = MongoProjectFunctions::getAllProjects(_loggedInUser, filter, limit, m_adminClient);

	return MongoProjectFunctions::projectsToJson(projects);
}

std::string ServiceBase::handleGetAllProjectsCount(const ot::ConstJsonObject& _actionDocument, User& _loggedInUser) {
	size_t count = MongoProjectFunctions::getAllProjectCount(_loggedInUser, m_adminClient);

	return std::to_string(count);
}

std::string ServiceBase::handleGetAllGroupProjects(const ot::ConstJsonObject& _actionDocument, User& _loggedInUser) {
	std::string groupName = ot::json::getString(_actionDocument, OT_PARAM_AUTH_GROUP_NAME);

	Group gr = MongoGroupFunctions::getGroupDataByName(groupName, m_adminClient);
	std::vector<Project> projectsGr = MongoProjectFunctions::getAllGroupProjects(gr, m_adminClient);

	return MongoProjectFunctions::projectsToJson(projectsGr);
}

std::string ServiceBase::handleChangeProjectName(const ot::ConstJsonObject& _actionDocument, User& _loggedInUser) {
	std::string projectName = ot::json::getString(_actionDocument, OT_PARAM_AUTH_PROJECT_NAME);
	std::string newProjectName = ot::json::getString(_actionDocument, OT_PARAM_AUTH_NEW_PROJECT_NAME);

	Project proj = MongoProjectFunctions::getProject(projectName, m_adminClient);

	if (proj.getUser().userId == _loggedInUser.userId || isAdminUser(_loggedInUser))
	{
		proj = MongoProjectFunctions::changeProjectName(proj, newProjectName, m_adminClient);
	}
	else
	{
		throw std::runtime_error("The logged in user is not the owner of this group! He cannot make the requested changes!");
	}

	return ot::ReturnMessage(ot::ReturnMessage::Ok, proj.toProjectInformation().toJson()).toJson();
}

std::string ServiceBase::handleChangeProjectOwner(const ot::ConstJsonObject& _actionDocument, User& _loggedInUser) {
	std::string projectName = ot::json::getString(_actionDocument, OT_PARAM_AUTH_PROJECT_NAME);
	std::string newOwnerUsername = ot::json::getString(_actionDocument, OT_PARAM_AUTH_NEW_PROJECT_OWNER);

	Project proj = MongoProjectFunctions::getProject(projectName, m_adminClient);
	User newOwner = MongoUserFunctions::getUserDataThroughUsername(newOwnerUsername, m_adminClient);

	if (proj.getUser().userId == _loggedInUser.userId || isAdminUser(_loggedInUser))
	{
		proj = MongoProjectFunctions::changeProjectOwner(proj, newOwner, m_adminClient);
	}
	else
	{
		throw std::runtime_error("The logged in user is not the owner of this Project! He cannot make the requested changes!");
	}

	return ot::ReturnMessage(ot::ReturnMessage::Ok, proj.toProjectInformation().toJson()).toJson();
}

std::string ServiceBase::handleAddGroupToProject(const ot::ConstJsonObject& _actionDocument, User& _loggedInUser) {
	std::string groupName = ot::json::getString(_actionDocument, OT_PARAM_AUTH_GROUP_NAME);
	std::string projectName = ot::json::getString(_actionDocument, OT_PARAM_AUTH_PROJECT_NAME);
	Group gr = MongoGroupFunctions::getGroupDataByName(groupName, m_adminClient);

	bool userIsGroupMember = false;
	for (const auto& groupUser : gr.users)
	{
		if (groupUser.userId == _loggedInUser.userId)
		{
			userIsGroupMember = true;
			break;
		}
	}

	bool successful = false;
	if (userIsGroupMember)
	{
		Project pr = MongoProjectFunctions::getProject(projectName, m_adminClient);
		successful = MongoProjectFunctions::addGroupToProject(gr, pr, m_adminClient);
	}
	else
	{
		throw std::runtime_error("The logged in user is not a member of this group! He cannot make the requested changes!");
	}

	ot::JsonDocument json;
	json.AddMember(OT_ACTION_AUTH_SUCCESS, successful, json.GetAllocator());
	return json.toJson();
}

std::string ServiceBase::handleRemoveGroupFromProject(const ot::ConstJsonObject& _actionDocument, User& _loggedInUser) {
	std::string groupName = ot::json::getString(_actionDocument, OT_PARAM_AUTH_GROUP_NAME);
	std::string projectName = ot::json::getString(_actionDocument, OT_PARAM_AUTH_PROJECT_NAME);
	Group gr = MongoGroupFunctions::getGroupDataByName(groupName, m_adminClient);

	bool successful = false;
	Project pr = MongoProjectFunctions::getProject(projectName, m_adminClient);
	if (pr.getUser().userId == _loggedInUser.userId) {
		successful = MongoProjectFunctions::removeGroupFromProject(gr, pr, m_adminClient);
	}
	else {
		throw std::runtime_error("The logged in user is not the owner of this Project! He cannot make the requested changes!");
	}

	ot::JsonDocument json;
	json.AddMember(OT_ACTION_AUTH_SUCCESS, successful, json.GetAllocator());
	return json.toJson();
}

std::string ServiceBase::handleRemoveProject(const ot::ConstJsonObject& _actionDocument, User& _loggedInUser) {
	std::string projectName = ot::json::getString(_actionDocument, OT_PARAM_AUTH_PROJECT_NAME);

	Project pr = MongoProjectFunctions::getProject(projectName, m_adminClient);

	bool successful = false;
	if (pr.getUser().userId == _loggedInUser.userId || isAdminUser(_loggedInUser))
	{
		successful = MongoProjectFunctions::removeProject(pr, m_adminClient);
	}
	else
	{
		throw std::runtime_error("The logged in user is not the owner of this Project! He cannot make the requested changes!");
	}

	ot::JsonDocument json;
	json.AddMember(OT_ACTION_AUTH_SUCCESS, successful, json.GetAllocator());
	return json.toJson();
}

std::string ServiceBase::handleCheckIfCollectionExists(const ot::ConstJsonObject& _actionDocument, User& _loggedInUser)
{
	std::string collectionName = ot::json::getString(_actionDocument, OT_PARAM_AUTH_COLLECTION_NAME);
	bool exist = MongoProjectFunctions::checkForCollectionExistence(collectionName, m_adminClient);
	
	ot::JsonDocument json;
	json.AddMember(OT_PARAM_AUTH_COLLECTION_EXISTS, exist, json.GetAllocator());
	return json.toJson();
}

// ###########################################################################################################################################################################################################################################################################################################################

// Private: Initialization functions

void ServiceBase::initializeDatabase() {
	// NEEDED AND MUST BE EXECUTED ONCE
	mongocxx::instance inst{};

	// Initializing the Admin Client at the time of starting the service.

	// Maybe fetch the admin Credentials through the environment 

	m_dbUsername = getAdminUserName();

	if (!m_databasePWD.empty()) {
		m_dbPassword = m_databasePWD;
	}
	else {
		m_dbPassword = ot::UserCredentials::encryptString("admin");
	}

	try {
		std::string uriStr = getMongoURL(m_databaseURL, m_dbUsername, ot::UserCredentials::decryptString(m_dbPassword));
		mongocxx::uri uri(uriStr);
		m_adminClient = mongocxx::client(uri);
	}
	catch (std::exception err) {
		OT_LOG_E("Cannot establish MongoDB connection: " + std::string(err.what())); // todo: should retry or exit?
		exit(ot::AppExitCode::DataBaseConnectionFailed);
	}

	// Add indexes for all 

	mongocxx::options::index uniqueIndexOptions{};
	uniqueIndexOptions.unique(true);

	mongocxx::options::index nonUniqueIndexOptions{};
	nonUniqueIndexOptions.unique(false);

	/* HANDLING PROJECT CATALOG ROLE AND INDEXES */
	try {
		MongoRoleFunctions::createInitialProjectRole(m_adminClient);
	}
	catch (std::runtime_error err) {
		std::string errMsg(err.what());
		if (errMsg.find(DB_ERROR_MESSAGE_ALREADY_EXISTS) != std::string::npos && errMsg.find(DB_ERROR_MESSAGE_ALREADY_EXISTS) != errMsg.length() - strlen(DB_ERROR_MESSAGE_ALREADY_EXISTS)) {
			OT_LOG_E("Error creating initial project role in database: " + std::string(err.what()));
		}
	}

	try {
		// Role so that each user can list the project DB collections
		MongoRoleFunctions::createInitialProjectDbListCollectionsRole(m_adminClient);
	}
	catch (std::runtime_error err) {
		std::string errMsg(err.what());
		if (errMsg.find(DB_ERROR_MESSAGE_ALREADY_EXISTS) != std::string::npos && errMsg.find(DB_ERROR_MESSAGE_ALREADY_EXISTS) != errMsg.length() - strlen(DB_ERROR_MESSAGE_ALREADY_EXISTS)) {
			OT_LOG_E("Error creating initial project list role in database: " + std::string(err.what()));
		}
	}

	try {
		uniqueIndexOptions.name("project_name_unique");
		m_adminClient[MongoConstants::PROJECTS_DB][MongoConstants::PROJECT_CATALOG_COLLECTION]
			.create_index(bsoncxx::builder::basic::make_document(bsoncxx::builder::basic::kvp("project_name", 1)), uniqueIndexOptions);
	}
	catch (std::runtime_error err) {
		OT_LOG_E("Error creating db index project_name_unique: " + std::string(err.what()));
	}

	try {
		nonUniqueIndexOptions.name("project_type_index");
		m_adminClient[MongoConstants::PROJECTS_DB][MongoConstants::PROJECT_CATALOG_COLLECTION]
			.create_index(bsoncxx::builder::basic::make_document(bsoncxx::builder::basic::kvp("project_type", 1)), nonUniqueIndexOptions);
	}
	catch (std::runtime_error err) {
		OT_LOG_E("Error creating db index project_type_index: " + std::string(err.what()));
	}

	try {
		nonUniqueIndexOptions.name("created_by_index");
		m_adminClient[MongoConstants::PROJECTS_DB][MongoConstants::PROJECT_CATALOG_COLLECTION]
			.create_index(bsoncxx::builder::basic::make_document(bsoncxx::builder::basic::kvp("created_by", 1)), nonUniqueIndexOptions);
	}
	catch (std::runtime_error err) {
		OT_LOG_E("Error creating db index created_by_index: " + std::string(err.what()));
	}

	try {
		uniqueIndexOptions.name("project_collection_name_unique");
		m_adminClient[MongoConstants::PROJECTS_DB][MongoConstants::PROJECT_CATALOG_COLLECTION]
			.create_index(bsoncxx::builder::basic::make_document(bsoncxx::builder::basic::kvp("collection_name", 1)), uniqueIndexOptions);
	}
	catch (std::runtime_error err) {
		OT_LOG_E("Error creating db index project_collection_name_unique: " + std::string(err.what()));
	}
	
	try {
		uniqueIndexOptions.name("project_role_name_unique");
		m_adminClient[MongoConstants::PROJECTS_DB][MongoConstants::PROJECT_CATALOG_COLLECTION]
			.create_index(bsoncxx::builder::basic::make_document(bsoncxx::builder::basic::kvp("project_role_name", 1)), uniqueIndexOptions);
	}
	catch (std::runtime_error err) {
		OT_LOG_E("Error creating db index project_role_name_unique: " + std::string(err.what()));
	}

	try {
		nonUniqueIndexOptions.name("groups_index");
		m_adminClient[MongoConstants::PROJECTS_DB][MongoConstants::PROJECT_CATALOG_COLLECTION]
			.create_index(bsoncxx::builder::basic::make_document(bsoncxx::builder::basic::kvp("groups", 1)), nonUniqueIndexOptions);
	}
	catch (std::runtime_error err) {
		OT_LOG_E("Error creating db index groups_index: " + std::string(err.what()));
	}

	try {
		nonUniqueIndexOptions.name("tags_index");
		m_adminClient[MongoConstants::PROJECTS_DB][MongoConstants::PROJECT_CATALOG_COLLECTION]
			.create_index(bsoncxx::builder::basic::make_document(bsoncxx::builder::basic::kvp("tags", 1)), nonUniqueIndexOptions);
	}
	catch (std::runtime_error err) {
		OT_LOG_E("Error creating db index tags_index: " + std::string(err.what()));
	}

	try {
		nonUniqueIndexOptions.name("project_group_index");
		m_adminClient[MongoConstants::PROJECTS_DB][MongoConstants::PROJECT_CATALOG_COLLECTION]
			.create_index(bsoncxx::builder::basic::make_document(bsoncxx::builder::basic::kvp("project_group", 1)), nonUniqueIndexOptions);
	}
	catch (std::runtime_error err) {
		OT_LOG_E("Error creating db index project_group_index: " + std::string(err.what()));
	}

	/* HANDLING GROUP COLLECTION ROLE AND INDEXES */

	try {
		MongoRoleFunctions::createInitialGroupRole(m_adminClient);
	}
	catch (std::runtime_error err) {
		std::string errMsg(err.what());
		if (errMsg.find(DB_ERROR_MESSAGE_ALREADY_EXISTS) != std::string::npos && errMsg.find(DB_ERROR_MESSAGE_ALREADY_EXISTS) != errMsg.length() - strlen(DB_ERROR_MESSAGE_ALREADY_EXISTS)) {
			OT_LOG_E("Error creating initial group role in database: " + std::string(err.what()));
		}
	}

	try {
		uniqueIndexOptions.name("group_name_unique");
		m_adminClient[MongoConstants::PROJECTS_DB][MongoConstants::GROUPS_COLLECTION]
			.create_index(bsoncxx::builder::basic::make_document(bsoncxx::builder::basic::kvp("group_name", 1)), uniqueIndexOptions);
	}
	catch (std::runtime_error err) {
		OT_LOG_E("Error creating db index group_name_unique: " + std::string(err.what()));
	}



	/* HANDLING ALL ACCESSIBLE DATABASE ROLES  */

	try {
		MongoRoleFunctions::createInitialProjectTemplatesRole(m_adminClient);
	}
	catch (std::runtime_error err) {
		std::string errMsg(err.what());
		if (errMsg.find(DB_ERROR_MESSAGE_ALREADY_EXISTS) != std::string::npos && errMsg.find(DB_ERROR_MESSAGE_ALREADY_EXISTS) != errMsg.length() - strlen(DB_ERROR_MESSAGE_ALREADY_EXISTS)) {
			OT_LOG_E("Error creating initial project templates role in database: " + std::string(err.what()));
		}
	}

	try {
		MongoRoleFunctions::createInitialProjectsLargeDataRole(m_adminClient);
	}
	catch (std::runtime_error err) {
		std::string errMsg(err.what());
		if (errMsg.find(DB_ERROR_MESSAGE_ALREADY_EXISTS) != std::string::npos && errMsg.find(DB_ERROR_MESSAGE_ALREADY_EXISTS) != errMsg.length() - strlen(DB_ERROR_MESSAGE_ALREADY_EXISTS)) {
			OT_LOG_E("Error creating initial projects large data role in database: " + std::string(err.what()));
		}
	}

	try {
		MongoRoleFunctions::createInitialSystemDbRole(m_adminClient);
	}
	catch (std::runtime_error err) {
		std::string errMsg(err.what());
		if (errMsg.find(DB_ERROR_MESSAGE_ALREADY_EXISTS) != std::string::npos && errMsg.find(DB_ERROR_MESSAGE_ALREADY_EXISTS) != errMsg.length() - strlen(DB_ERROR_MESSAGE_ALREADY_EXISTS)) {
			OT_LOG_E("Error creating initial system db role in database: " + std::string(err.what()));
		}
	}

	try {
		MongoRoleFunctions::createInitialSettingsDbRole(m_adminClient);
	}
	catch (std::runtime_error err) {
		std::string errMsg(err.what());
		if (errMsg.find(DB_ERROR_MESSAGE_ALREADY_EXISTS) != std::string::npos && errMsg.find(DB_ERROR_MESSAGE_ALREADY_EXISTS) != errMsg.length() - strlen(DB_ERROR_MESSAGE_ALREADY_EXISTS)) {
			OT_LOG_E("Error creating initial settings db role in database: " + std::string(err.what()));
		}
	}

	try {
		MongoRoleFunctions::createInitialLibrariesDbRole(m_adminClient);
	}
	catch (std::runtime_error err) {
		std::string errMsg(err.what());
		if (errMsg.find(DB_ERROR_MESSAGE_ALREADY_EXISTS) != std::string::npos && errMsg.find(DB_ERROR_MESSAGE_ALREADY_EXISTS) != errMsg.length() - strlen(DB_ERROR_MESSAGE_ALREADY_EXISTS)) {
			OT_LOG_E("Error creating initial libraries db role in database: " + std::string(err.what()));
		}
	}

	/* HANDLING USER ROLE AND INDEXES */

	try {
		MongoRoleFunctions::createInitialUserRole(m_adminClient);
	}
	catch (std::runtime_error err) {
		std::string errMsg(err.what());
		if (errMsg.find(DB_ERROR_MESSAGE_ALREADY_EXISTS) != std::string::npos && errMsg.find(DB_ERROR_MESSAGE_ALREADY_EXISTS) != errMsg.length() - strlen(DB_ERROR_MESSAGE_ALREADY_EXISTS)) {
			OT_LOG_E("Error creating initial user db role in database: " + std::string(err.what()));
		}
	}

	try {
		// Role so that the admin can list the user DB collections
		MongoRoleFunctions::createInitialUserDbListCollectionsRole(m_adminClient);
	}
	catch (std::runtime_error err) {
		std::string errMsg(err.what());
		if (errMsg.find(DB_ERROR_MESSAGE_ALREADY_EXISTS) != std::string::npos && errMsg.find(DB_ERROR_MESSAGE_ALREADY_EXISTS) != errMsg.length() - strlen(DB_ERROR_MESSAGE_ALREADY_EXISTS)) {
			OT_LOG_E("Error creating initial user list role in database: " + std::string(err.what()));
		}
	}

	try {
		uniqueIndexOptions.name("user_name_unique");
		m_adminClient[MongoConstants::USER_DB][MongoConstants::USER_CATALOG_COLLECTION]
			.create_index(bsoncxx::builder::basic::make_document(bsoncxx::builder::basic::kvp("user_name", 1)), uniqueIndexOptions);
	}
	catch (std::runtime_error err) {
		OT_LOG_E("Error creating db index user_name_unique: " + std::string(err.what()));
	}

	try {
		uniqueIndexOptions.name("user_id_unique");
		m_adminClient[MongoConstants::USER_DB][MongoConstants::USER_CATALOG_COLLECTION]
			.create_index(bsoncxx::builder::basic::make_document(bsoncxx::builder::basic::kvp("user_id", 1)), uniqueIndexOptions);
	}
	catch (std::runtime_error err) {
		OT_LOG_E("Error creating db index user_id_unique: " + std::string(err.what()));
	}
}

// ###########################################################################################################################################################################################################################################################################################################################

// Private: Helper

bool ServiceBase::isAdminUser(User& _loggedInUser) {
	return (_loggedInUser.username == getAdminUserName());
}

std::string ServiceBase::createRandomPassword()
{
	const int length = 30;

	// Define the list of possible characters
	const std::string CHARACTERS = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";

	// Create a random number generator
	std::random_device rd;
	std::mt19937 generator(rd());

	// Create a distribution to uniformly select from all
	// characters
	std::uniform_int_distribution<> distribution(0, (int) (CHARACTERS.size()) - 1);

	// Generate the random string
	std::string random_string;

	for (int i = 0; i < length; ++i) {
		random_string += CHARACTERS[distribution(generator)];
	}

	return random_string;
}

bool ServiceBase::removeOldSessionsWorker() {
	while (1) {
		MongoSessionFunctions::removeOldSessions(m_adminClient);

		using namespace std::chrono_literals;
		std::this_thread::sleep_for(3600s);  // Wait for one hour
	}
}
