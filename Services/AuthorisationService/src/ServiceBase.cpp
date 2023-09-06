#include "ServiceBase.h"
#include "MongoURL.h"

#include <iostream>

#include "OpenTwinCommunication/ActionTypes.h"
#include "OpenTwinCommunication/Msg.h"
#include "OpenTwinCore/rJSON.h"
#include "OpenTwinFoundation/UserCredentials.h"
#include "OpenTwinCore/Logger.h"
//#include "OpenTwinCore/TypeConversion.h"
#include "OpenTwinCore/otAssert.h"

#define DB_ERROR_MESSAGE_ALREADY_EXISTS "already exists: generic server error"

int ServiceBase::initialize(const char * _ownIP, const char * _databaseIP, const char * _databasePWD)
{
	serviceURL  = _ownIP;
	databaseURL = _databaseIP;
	databasePWD = _databasePWD;

	OT_LOG_I("Starting Authorisation Service");
	OT_LOG_D("Service address : " + serviceURL);
	OT_LOG_D("Database address: " + databaseURL);

	// NEEDED AND MUST BE EXECUTED ONCE
	mongocxx::instance inst{};

	// Initializing the Admin Client at the time of starting the service.

	// Maybe fetch the admin Credentials through the environment 

	dbUsername = "admin";

	if (!databasePWD.empty())
	{
		dbPassword = databasePWD;
	}
	else
	{
		dbPassword = ot::UserCredentials::encryptString("admin");
	}

	std::string uriStr = getMongoURL(databaseURL, dbUsername, ot::UserCredentials::decryptString(dbPassword));

	try
	{
		mongocxx::uri uri(uriStr);
		adminClient = mongocxx::client(uri);
	}
	catch (std::exception err)
	{
		OT_LOG_E("Cannot establish MongoDB connection: " + std::string(err.what())); // todo: should retry or exit?
		return -1;
	}

	try {
		adminUser = MongoUserFunctions::getUserDataThroughUsername(dbUsername, adminClient);
	}
	catch (std::exception err)
	{
		OT_LOG_E("Cannot get admin username: " + std::string(err.what())); // todo: should retry or exit?
		return -2;
	}

	// Add indexes for all 

	mongocxx::options::index index_options{};
	index_options.unique(true);

	/* HANDLING PROJECT CATALOG ROLE AND INDEXES */
	try
	{
		MongoRoleFunctions::createInitialProjectRole(adminClient);
	}
	catch (std::runtime_error err)
	{
		std::string errMsg(err.what());
		if (errMsg.find(DB_ERROR_MESSAGE_ALREADY_EXISTS) != std::string::npos && errMsg.find(DB_ERROR_MESSAGE_ALREADY_EXISTS) != errMsg.length() - strlen(DB_ERROR_MESSAGE_ALREADY_EXISTS)) {
			OT_LOG_E("Error creating initial project role in database: " + std::string(err.what()));
		}
	}

	try
	{
		// Role so that each user can list the project DB collections
		MongoRoleFunctions::createInitialProjectDbListCollectionsRole(adminClient);
	}
	catch (std::runtime_error err)
	{
		std::string errMsg(err.what());
		if (errMsg.find(DB_ERROR_MESSAGE_ALREADY_EXISTS) != std::string::npos && errMsg.find(DB_ERROR_MESSAGE_ALREADY_EXISTS) != errMsg.length() - strlen(DB_ERROR_MESSAGE_ALREADY_EXISTS)) {
			OT_LOG_E("Error creating initial project list role in database: " + std::string(err.what()));
		}
	}

	try
	{
		index_options.name("project_name_unique");
		adminClient[MongoConstants::PROJECTS_DB][MongoConstants::PROJECT_CATALOG_COLLECTION]
			.create_index(bsoncxx::builder::basic::make_document(bsoncxx::builder::basic::kvp("project_name", 1)), index_options);
	}
	catch (std::runtime_error err)
	{
		OT_LOG_E("Error creating db index project_name_unique: " + std::string(err.what()));
	}

	try
	{
		index_options.name("project_role_name_unique");
		adminClient[MongoConstants::PROJECTS_DB][MongoConstants::PROJECT_CATALOG_COLLECTION]
			.create_index(bsoncxx::builder::basic::make_document(bsoncxx::builder::basic::kvp("project_role_name", 1)), index_options);
	}
	catch (std::runtime_error err)
	{
		OT_LOG_E("Error creating db index project_role_name_unique: " + std::string(err.what()));
	}

	try
	{
		index_options.name("project_collection_name_unique");
		adminClient[MongoConstants::PROJECTS_DB][MongoConstants::PROJECT_CATALOG_COLLECTION]
			.create_index(bsoncxx::builder::basic::make_document(bsoncxx::builder::basic::kvp("collection_name", 1)), index_options);
	}
	catch (std::runtime_error err)
	{
		OT_LOG_E("Error creating db index project_collection_name_unique: " + std::string(err.what()));
	}


	/* HANDLING GROUP COLLECTION ROLE AND INDEXES */

	try
	{
		MongoRoleFunctions::createInitialGroupRole(adminClient);
	}
	catch (std::runtime_error err)
	{
		std::string errMsg(err.what());
		if (errMsg.find(DB_ERROR_MESSAGE_ALREADY_EXISTS) != std::string::npos && errMsg.find(DB_ERROR_MESSAGE_ALREADY_EXISTS) != errMsg.length() - strlen(DB_ERROR_MESSAGE_ALREADY_EXISTS)) {
			OT_LOG_E("Error creating initial group role in database: " + std::string(err.what()));
		}
	}

	try
	{
		index_options.name("group_name_unique");
		adminClient[MongoConstants::PROJECTS_DB][MongoConstants::GROUPS_COLLECTION]
			.create_index(bsoncxx::builder::basic::make_document(bsoncxx::builder::basic::kvp("group_name", 1)), index_options);
	}
	catch (std::runtime_error err)
	{
		OT_LOG_E("Error creating db index group_name_unique: " + std::string(err.what()));
	}



	/* HANDLING ALL ACCESSIBLE DATABASE ROLES  */

	try
	{
		MongoRoleFunctions::createInitialProjectTemplatesRole(adminClient);
	}
	catch (std::runtime_error err)
	{
		std::string errMsg(err.what());
		if (errMsg.find(DB_ERROR_MESSAGE_ALREADY_EXISTS) != std::string::npos && errMsg.find(DB_ERROR_MESSAGE_ALREADY_EXISTS) != errMsg.length() - strlen(DB_ERROR_MESSAGE_ALREADY_EXISTS)) {
			OT_LOG_E("Error creating initial project templates role in database: " + std::string(err.what()));
		}
	}

	try
	{
		MongoRoleFunctions::createInitialProjectsLargeDataRole(adminClient);
	}
	catch (std::runtime_error err)
	{
		std::string errMsg(err.what());
		if (errMsg.find(DB_ERROR_MESSAGE_ALREADY_EXISTS) != std::string::npos && errMsg.find(DB_ERROR_MESSAGE_ALREADY_EXISTS) != errMsg.length() - strlen(DB_ERROR_MESSAGE_ALREADY_EXISTS)) {
			OT_LOG_E("Error creating initial projects large data role in database: " + std::string(err.what()));
		}
	}

	try
	{
		MongoRoleFunctions::createInitialSystemDbRole(adminClient);
	}
	catch (std::runtime_error err)
	{
		std::string errMsg(err.what());
		if (errMsg.find(DB_ERROR_MESSAGE_ALREADY_EXISTS) != std::string::npos && errMsg.find(DB_ERROR_MESSAGE_ALREADY_EXISTS) != errMsg.length() - strlen(DB_ERROR_MESSAGE_ALREADY_EXISTS)) {
			OT_LOG_E("Error creating initial system db role in database: " + std::string(err.what()));
		}
	}

	try
	{
		MongoRoleFunctions::createInitialSettingsDbRole(adminClient);
	}
	catch (std::runtime_error err)
	{
		std::string errMsg(err.what());
		if (errMsg.find(DB_ERROR_MESSAGE_ALREADY_EXISTS) != std::string::npos && errMsg.find(DB_ERROR_MESSAGE_ALREADY_EXISTS) != errMsg.length() - strlen(DB_ERROR_MESSAGE_ALREADY_EXISTS)) {
			OT_LOG_E("Error creating initial settings db role in database: " + std::string(err.what()));
		}
	}




	return 0;
}

const char *ServiceBase::getServiceURL(void)
{
	return serviceURL.c_str();
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
	OT_rJSON_parseDOC(actionDoc, _json);
	if (!actionDoc.IsObject()) {
		otAssert(0, "Received message is not an JSON object");
		return OT_ACTION_RETURN_INDICATOR_Error "Received message is not an JSON object";
	}
	if (!actionDoc.HasMember(OT_ACTION_MEMBER)) {
		otAssert(0, "Received JSON Object does not contain the action member");
		return OT_ACTION_RETURN_INDICATOR_Error "Received JSON Object does not contain the action member";
	}
	if (!actionDoc[OT_ACTION_MEMBER].IsString()) {
		otAssert(0, "Received JSON Object's action member is not a string");
		return OT_ACTION_RETURN_INDICATOR_Error "Received JSON Object's action member is not a string";
	}
	std::string action = actionDoc[OT_ACTION_MEMBER].GetString();

	// Ping may be multithreaded, others have to be handled one by one
	if (action == OT_ACTION_PING) { return OT_ACTION_PING; }

	m_mutex.lock();

	try {
		OT_LOG("Dispatchig action: \"" + action + "\"", ot::messageTypeToLogFlag(_messageType));
		std::string response = dispatchAction(action, actionDoc, _messageType);
		OT_LOG(".. Completed: Dispatchig action: \"" + action + "\": Returning: \"" + response + "\"", ot::messageTypeToLogFlag(_messageType));
		m_mutex.unlock();
		return response;
	}
	catch (std::exception _e)
	{
		OT_LOG_E(_e.what());

		OT_rJSON_createDOC(json);
		ot::rJSON::add(json, "error", 1);
		ot::rJSON::add(json, "description", _e.what());

		m_mutex.unlock();
		return ot::rJSON::toJSON(json);
	}
	catch (...) {
		m_mutex.unlock();
		return OT_ACTION_RETURN_UnknownError;
	}
}

std::string ServiceBase::dispatchAction(const std::string& _action, OT_rJSON_doc& _actionDocument, ot::MessageType _messageType)
{
	//------------ USER FUNCTIONS THAT DO NOT NEED AUTHENTICATION ------------------------
	if (_action == OT_ACTION_LOGIN_ADMIN) { return handleAdminLogIn(_actionDocument); }
	else if (_action == OT_ACTION_LOGIN) { return handleLogIn(_actionDocument); }
	else if (_action == OT_ACTION_REGISTER) { return handleRegister(_actionDocument); }
	//------------------------------------------------------------------------------------

	// Checking whether the logged in user is the one that he claims to be. All the requests must include loggedInUsername and loggedInUserPassword
	std::string loggedInUsername = ot::rJSON::getString(_actionDocument, OT_PARAM_AUTH_LOGGED_IN_USERNAME);
	std::string loggedInUserPassword = ot::rJSON::getString(_actionDocument, OT_PARAM_AUTH_LOGGED_IN_USER_PASSWORD);
	bool logInSuccessful = MongoUserFunctions::authenticateUser(loggedInUsername, loggedInUserPassword, databaseURL);

	if (!logInSuccessful)
	{
		assert(0);
		throw std::runtime_error("The User could not be authenticated! Please logout and attempt to log in again!");
	}

	User loggedInUser = MongoUserFunctions::getUserDataThroughUsername(loggedInUsername, adminClient);

	//------------ FUNCTIONS THAT NEED AUTHENTICATION ------------
	if (_action == OT_ACTION_GET_USER_DATA) { return handleGetUserData(_actionDocument); }
	else if (_action == OT_ACTION_GET_ALL_USERS) { return handleGetAllUsers(_actionDocument); }
	else if (_action == OT_ACTION_GET_ALL_USER_COUNT) { return handleGetAllUsersCount(_actionDocument); }
	else if (_action == OT_ACTION_CHANGE_USER_USERNAME) { return handleChangeUserNameByUser(_actionDocument, loggedInUser, loggedInUserPassword); }
	else if (_action == OT_ACTION_CHANGE_USER_PASSWORD) { return handleChangeUserPasswordByUser(_actionDocument, loggedInUser, loggedInUserPassword); }
	else if (_action == OT_ACTION_DELETE_USER) { return handleDeleteUser(_actionDocument, loggedInUser, loggedInUserPassword); }
	else if (_action == OT_ACTION_CHANGE_USERNAME) { return handleChangeUserNameByAdmin(_actionDocument, loggedInUser, loggedInUserPassword); }
	else if (_action == OT_ACTION_CHANGE_PASSWORD) { return handleChangeUserPasswordByAdmin(_actionDocument, loggedInUser, loggedInUserPassword); }
	//------------ Group FUNCTIONS ------------
	else if (_action == OT_ACTION_CREATE_GROUP) { return handleCreateGroup(_actionDocument, loggedInUser); }
	else if (_action == OT_ACTION_GET_GROUP_DATA) { return handleGetGroupData(_actionDocument); }
	else if (_action == OT_ACTION_GET_ALL_USER_GROUPS) { return handleGetAllUserGroups(_actionDocument, loggedInUser); }
	else if (_action == OT_ACTION_GET_ALL_GROUPS) { return handleGetAllGroups(_actionDocument, loggedInUser); }
	else if (_action == OT_ACTION_GET_ALL_GROUP_COUNT) { return handleGetAllGroupsCount(_actionDocument, loggedInUser); }
	else if (_action == OT_ACTION_CHANGE_GROUP_NAME) { return handleChangeGroupName(_actionDocument, loggedInUser); }
	else if (_action == OT_ACTION_CHANGE_GROUP_OWNER) { return handleChangeGroupOwner(_actionDocument, loggedInUser); }
	else if (_action == OT_ACTION_ADD_USER_TO_GROUP) { return handleAddUserToGroup(_actionDocument, loggedInUser); }
	else if (_action == OT_ACTION_REMOVE_USER_FROM_GROUP) { return handleRemoveUserFromGroup(_actionDocument, loggedInUser); }
	else if (_action == OT_ACTION_REMOVE_GROUP) { return handleRemoveGroup(_actionDocument, loggedInUser); }
	//------------ Project FUNCTIONS ------------
	else if (_action == OT_ACTION_CREATE_PROJECT) { return handleCreateProject(_actionDocument, loggedInUser); }
	//                                                       v-- CAN BE PERFORMED BY THE UI CLIENT --v
	else if (_action == OT_ACTION_GET_PROJECT_DATA) { return handleGetProjectData(_actionDocument); }
	else if (_action == OT_ACTION_GET_ALL_PROJECT_OWNERS) { return handleGetAllProjectOwners(_actionDocument, loggedInUser); }
	else if (_action == OT_ACTION_GET_ALL_USER_PROJECTS) { return handleGetAllUserProjects(_actionDocument, loggedInUser); }
	else if (_action == OT_ACTION_GET_ALL_PROJECTS) { return handleGetAllProjects(_actionDocument, loggedInUser); }
	else if (_action == OT_ACTION_GET_ALL_PROJECT_COUNT) { return handleGetAllProjectsCount(_actionDocument, loggedInUser); }
	else if (_action == OT_ACTION_GET_ALL_GROUP_PROJECTS) { return handleGetAllGroupProjects(_actionDocument, loggedInUser); }
	else if (_action == OT_ACTION_CHANGE_PROJECT_NAME) { return handleChangeProjectName(_actionDocument, loggedInUser); }
	else if (_action == OT_ACTION_CHANGE_PROJECT_OWNER) { return handleChangeProjectOwner(_actionDocument, loggedInUser); }
	else if (_action == OT_ACTION_ADD_GROUP_TO_PROJECT) { return handleAddGroupToProject(_actionDocument, loggedInUser); }
	else if (_action == OT_ACTION_REMOVE_GROUP_FROM_PROJECT) { return handleRemoveGroupFromProject(_actionDocument, loggedInUser); }
	else if (_action == OT_ACTION_REMOVE_PROJECT) { return handleRemoveProject(_actionDocument, loggedInUser); }
	else if (_action == OT_ACTION_CHECK_FOR_COLLECTION_EXISTENCE) { return handleCheckIfCollectionExists(_actionDocument, loggedInUser); }
	else
	{
		// This action is unknown
		assert(0);
		return OT_ACTION_RETURN_UnknownAction;
	}
}

// No authentication needed

std::string ServiceBase::handleAdminLogIn(OT_rJSON_doc& _actionDocument) {
	std::string username = ot::rJSON::getString(_actionDocument, OT_PARAM_AUTH_USERNAME);
	std::string password = ot::rJSON::getString(_actionDocument, OT_PARAM_AUTH_PASSWORD);
	bool encryptedPassword = ot::rJSON::getBool(_actionDocument, OT_PARAM_AUTH_ENCRYPTED_PASSWORD);

	if (encryptedPassword)
	{
		password = ot::UserCredentials::decryptString(password);
	}

	bool successful = (username == dbUsername && password == ot::UserCredentials::decryptString(dbPassword));

	OT_rJSON_createDOC(json);
	ot::rJSON::add(json, OT_ACTION_AUTH_SUCCESS, successful);

	if (successful)
	{
		ot::rJSON::add(json, OT_PARAM_AUTH_ENCRYPTED_PASSWORD, ot::UserCredentials::encryptString(password));
	}

	return ot::rJSON::toJSON(json);
}

std::string ServiceBase::handleLogIn(OT_rJSON_doc& _actionDocument) {
	std::string username = ot::rJSON::getString(_actionDocument, OT_PARAM_AUTH_USERNAME);
	std::string password = ot::rJSON::getString(_actionDocument, OT_PARAM_AUTH_PASSWORD);
	bool encryptedPassword = ot::rJSON::getBool(_actionDocument, OT_PARAM_AUTH_ENCRYPTED_PASSWORD);

	if (encryptedPassword)
	{
		password = ot::UserCredentials::decryptString(password);
	}

	bool successful = MongoUserFunctions::authenticateUser(username, password, databaseURL);

	OT_rJSON_createDOC(json);
	ot::rJSON::add(json, OT_ACTION_AUTH_SUCCESS, successful);

	if (successful)
	{
		ot::rJSON::add(json, OT_PARAM_AUTH_PASSWORD, password);
		ot::rJSON::add(json, OT_PARAM_AUTH_ENCRYPTED_PASSWORD, ot::UserCredentials::encryptString(password));
	}

	return ot::rJSON::toJSON(json);
}

std::string ServiceBase::handleRegister(OT_rJSON_doc& _actionDocument) {
	std::string username = ot::rJSON::getString(_actionDocument, OT_PARAM_AUTH_USERNAME);
	std::string password = ot::rJSON::getString(_actionDocument, OT_PARAM_AUTH_PASSWORD);

	bool successful = MongoUserFunctions::registerUser(username, password, adminClient);

	OT_rJSON_createDOC(registerJson);
	ot::rJSON::add(registerJson, "successful", successful);
	return ot::rJSON::toJSON(registerJson);
}

// authentication needed: user functions

std::string ServiceBase::handleGetUserData(OT_rJSON_doc& _actionDocument) {
	std::string username = ot::rJSON::getString(_actionDocument, OT_PARAM_AUTH_USERNAME);
	User usr = MongoUserFunctions::getUserDataThroughUsername(username, adminClient);

	return MongoUserFunctions::userToJson(usr);
}

std::string ServiceBase::handleGetAllUsers(OT_rJSON_doc& _actionDocument) {
	std::vector<User> allUsers = MongoUserFunctions::getAllUsers(adminClient);

	return MongoUserFunctions::usersToJson(allUsers);
}

std::string ServiceBase::handleGetAllUsersCount(OT_rJSON_doc& _actionDocument) {
	size_t count = MongoUserFunctions::getAllUserCount(adminClient);
	return std::to_string(count);
}

std::string ServiceBase::handleChangeUserNameByUser(OT_rJSON_doc& _actionDocument, const User& _loggedInUser, const std::string& _loggedInUserPassword) {
	std::string newUsername = ot::rJSON::getString(_actionDocument, OT_PARAM_AUTH_NEW_USERNAME);
	bool successful = MongoUserFunctions::updateUserUsername(_loggedInUser.username, _loggedInUserPassword, newUsername, adminClient);

	OT_rJSON_createDOC(json);
	ot::rJSON::add(json, "successful", successful);
	return ot::rJSON::toJSON(json);
}

std::string ServiceBase::handleChangeUserPasswordByUser(OT_rJSON_doc& _actionDocument, const User& _loggedInUser, const std::string& _loggedInUserPassword) {
	std::string password = ot::rJSON::getString(_actionDocument, OT_PARAM_AUTH_PASSWORD);
	bool successful = MongoUserFunctions::changeUserPassword(_loggedInUser.username, password, adminClient);

	OT_rJSON_createDOC(json);
	ot::rJSON::add(json, "successful", successful);
	return ot::rJSON::toJSON(json);
}

std::string ServiceBase::handleDeleteUser(OT_rJSON_doc& _actionDocument, User& _loggedInUser, const std::string& _loggedInUserPassword) {
	std::string usernameToDelete = ot::rJSON::getString(_actionDocument, OT_PARAM_USERNAME_TO_DELETE);

	User toBeDeletedUser = MongoUserFunctions::getUserDataThroughUsername(usernameToDelete, adminClient);

	bool successful = false;
	// Check if the user wants do delete himself or if the call comes from the Admin User
	if (_loggedInUser.getUserId() == toBeDeletedUser.getUserId() || _loggedInUser.getUserId() == adminUser.getUserId())
	{
		successful = MongoUserFunctions::removeUser(toBeDeletedUser, adminClient);
	}

	OT_rJSON_createDOC(json);
	// todo: switch ok return OK
	ot::rJSON::add(json, "successful", successful);
	return ot::rJSON::toJSON(json);
}

std::string ServiceBase::handleChangeUserNameByAdmin(OT_rJSON_doc& _actionDocument, User& _loggedInUser, const std::string& _loggedInUserPassword) {
	if (_loggedInUser.getUserId() != adminUser.getUserId())
	{
		throw std::runtime_error("The logged in user is not an admin user. Standard users can not change the username of an arbitrary user.");
	}

	std::string oldUsername = ot::rJSON::getString(_actionDocument, OT_PARAM_AUTH_OLD_USERNAME);
	std::string newUsername = ot::rJSON::getString(_actionDocument, OT_PARAM_AUTH_NEW_USERNAME);

	bool successful = MongoUserFunctions::updateUsername(oldUsername, newUsername, adminClient);

	OT_rJSON_createDOC(json);
	ot::rJSON::add(json, "successful", successful);
	return ot::rJSON::toJSON(json);
}

std::string ServiceBase::handleChangeUserPasswordByAdmin(OT_rJSON_doc& _actionDocument, User& _loggedInUser, const std::string& _loggedInUserPassword) {
	if (_loggedInUser.getUserId() != adminUser.getUserId())
	{
		throw std::runtime_error("The logged in user is not an admin user. Standard users can not change the password of an arbitrary user.");
	}

	std::string userName = ot::rJSON::getString(_actionDocument, OT_PARAM_AUTH_USERNAME);
	std::string newPassword = ot::rJSON::getString(_actionDocument, OT_PARAM_AUTH_NEW_PASSWORD);

	bool successful = MongoUserFunctions::changeUserPassword(userName, newPassword, adminClient);

	OT_rJSON_createDOC(json);
	ot::rJSON::add(json, "successful", successful);
	return ot::rJSON::toJSON(json);
}

// authentication needed: group functions

std::string ServiceBase::handleCreateGroup(OT_rJSON_doc& _actionDocument, User& _loggedInUser) {
	std::string groupName = ot::rJSON::getString(_actionDocument, OT_PARAM_AUTH_GROUP_NAME);

	MongoGroupFunctions::createGroup(groupName, _loggedInUser, adminClient);
	Group gr = MongoGroupFunctions::getGroupData(groupName, adminClient);

	return MongoGroupFunctions::groupToJson(gr);
}

std::string ServiceBase::handleGetGroupData(OT_rJSON_doc& _actionDocument) {
	std::string groupName = ot::rJSON::getString(_actionDocument, OT_PARAM_AUTH_GROUP_NAME);

	Group gr = MongoGroupFunctions::getGroupData(groupName, adminClient);

	return MongoGroupFunctions::groupToJson(gr);
}

std::string ServiceBase::handleGetAllUserGroups(OT_rJSON_doc& _actionDocument, User& _loggedInUser) {
	std::vector<Group> groups = MongoGroupFunctions::getAllUserGroups(_loggedInUser, adminClient);

	return MongoGroupFunctions::groupsToJson(groups);
}

std::string ServiceBase::handleGetAllGroups(OT_rJSON_doc& _actionDocument, User& _loggedInUser) {
	if (_loggedInUser.getUserId() != adminUser.getUserId())
	{
		throw std::runtime_error("The logged in user is not an admin user. Standard users can not retrieve a list of all groups.");
	}

	std::vector<Group> groups = MongoGroupFunctions::getAllGroups(_loggedInUser, adminClient);

	return MongoGroupFunctions::groupsToJson(groups);
}

std::string ServiceBase::handleGetAllGroupsCount(OT_rJSON_doc& _actionDocument, User& _loggedInUser) {
	size_t count = MongoGroupFunctions::getAllGroupCount(_loggedInUser, adminClient);
	return std::to_string(count);
}

std::string ServiceBase::handleChangeGroupName(OT_rJSON_doc& _actionDocument, User& _loggedInUser) {
	std::string groupName = ot::rJSON::getString(_actionDocument, OT_PARAM_AUTH_GROUP_NAME);
	std::string newGroupName = ot::rJSON::getString(_actionDocument, OT_PARAM_AUTH_NEW_GROUP_NAME);

	Group gr = MongoGroupFunctions::getGroupData(groupName, adminClient);

	bool successful = false;

	if (gr.getOwnerId() == _loggedInUser.getUserId() || _loggedInUser.getUserId() == adminUser.getUserId())
	{
		successful = MongoGroupFunctions::changeGroupName(groupName, newGroupName, adminClient);
	}
	else
	{
		throw std::runtime_error("The logged in user is not the owner of this group! He cannot make the requested changes!");
	}

	OT_rJSON_createDOC(json);
	ot::rJSON::add(json, "successful", successful);
	return ot::rJSON::toJSON(json);
}

std::string ServiceBase::handleChangeGroupOwner(OT_rJSON_doc& _actionDocument, User& _loggedInUser) {
	std::string groupName = ot::rJSON::getString(_actionDocument, OT_PARAM_AUTH_GROUP_NAME);
	std::string newOwnerusername = ot::rJSON::getString(_actionDocument, OT_PARAM_AUTH_GROUP_OWNER_NEW_USER_USERNAME);

	User newOwner = MongoUserFunctions::getUserDataThroughUsername(newOwnerusername, adminClient);

	Group group = MongoGroupFunctions::getGroupData(groupName, adminClient);

	bool successful = false;
	if (group.getOwnerId() == _loggedInUser.getUserId() || _loggedInUser.getUserId() == adminUser.getUserId())
	{
		successful = MongoGroupFunctions::changeGroupOwner(group, _loggedInUser, newOwner, adminClient);
	}
	else
	{
		throw std::runtime_error("The logged in user is not the owner of this group! He cannot make the requested changes!");
	}


	OT_rJSON_createDOC(json);
	ot::rJSON::add(json, "successful", successful);
	return ot::rJSON::toJSON(json);
}

std::string ServiceBase::handleAddUserToGroup(OT_rJSON_doc& _actionDocument, User& _loggedInUser) {
	std::string username = ot::rJSON::getString(_actionDocument, OT_PARAM_AUTH_USERNAME);
	std::string groupName = ot::rJSON::getString(_actionDocument, OT_PARAM_AUTH_GROUP_NAME);
	Group gr = MongoGroupFunctions::getGroupData(groupName, adminClient);


	bool successful = false;
	if (gr.getOwnerId() == _loggedInUser.getUserId())
	{
		User usr = MongoUserFunctions::getUserDataThroughUsername(username, adminClient);
		successful = MongoGroupFunctions::addUserToGroup(usr, groupName, adminClient);
	}
	else
	{
		throw std::runtime_error("The logged in user is not the owner of this group! He cannot make the requested changes!");
	}


	OT_rJSON_createDOC(json);
	ot::rJSON::add(json, "successful", successful);
	return ot::rJSON::toJSON(json);
}

std::string ServiceBase::handleRemoveUserFromGroup(OT_rJSON_doc& _actionDocument, User& _loggedInUser) {
	std::string username = ot::rJSON::getString(_actionDocument, OT_PARAM_AUTH_USERNAME);
	std::string groupName = ot::rJSON::getString(_actionDocument, OT_PARAM_AUTH_GROUP_NAME);
	Group gr = MongoGroupFunctions::getGroupData(groupName, adminClient);

	bool successful = false;
	if (gr.getOwnerId() == _loggedInUser.getUserId())
	{
		User usr = MongoUserFunctions::getUserDataThroughUsername(username, adminClient);
		successful = MongoGroupFunctions::removeUserFromGroup(usr, groupName, adminClient);
	}
	else
	{
		throw std::runtime_error("The logged in user is not the owner of this group! He cannot make the requested changes!");
	}


	OT_rJSON_createDOC(json);
	ot::rJSON::add(json, "successful", successful);
	return ot::rJSON::toJSON(json);
}

std::string ServiceBase::handleRemoveGroup(OT_rJSON_doc& _actionDocument, User& _loggedInUser) {
	std::string groupName = ot::rJSON::getString(_actionDocument, OT_PARAM_AUTH_GROUP_NAME);

	auto group = MongoGroupFunctions::getGroupData(groupName, adminClient);

	auto groupOwnerId = group.getOwnerId();
	auto requestingUserId = _loggedInUser.getUserId();

	bool successful = false;
	if (groupOwnerId == requestingUserId || adminUser.getUserId() == requestingUserId)
	{
		successful = MongoGroupFunctions::removeGroup(group, adminClient);
	}
	else
	{
		throw std::runtime_error("The logged in user is not the owner of this group! He cannot make the requested changes!");
	}


	OT_rJSON_createDOC(json);
	ot::rJSON::add(json, "successful", successful);
	return ot::rJSON::toJSON(json);
}

// authentication needed: project functions

std::string ServiceBase::handleCreateProject(OT_rJSON_doc& _actionDocument, User& _loggedInUser) {
	std::string projectName = ot::rJSON::getString(_actionDocument, OT_PARAM_AUTH_PROJECT_NAME);

	Project createdProject = MongoProjectFunctions::createProject(projectName, _loggedInUser, adminClient);

	return MongoProjectFunctions::projectToJson(createdProject);
}

std::string ServiceBase::handleGetProjectData(OT_rJSON_doc& _actionDocument) {
	std::string projectName = ot::rJSON::getString(_actionDocument, OT_PARAM_AUTH_PROJECT_NAME);

	Project proj = MongoProjectFunctions::getProject(projectName, adminClient);

	return MongoProjectFunctions::projectToJson(proj);
}

std::string ServiceBase::handleGetAllProjectOwners(OT_rJSON_doc& _actionDocument, User& _loggedInUser) {
	auto projectNames = ot::rJSON::getStringList(_actionDocument, OT_PARAM_AUTH_PROJECT_NAMES);
	return MongoProjectFunctions::getProjectOwners(projectNames, adminClient);
}

std::string ServiceBase::handleGetAllUserProjects(OT_rJSON_doc& _actionDocument, User& _loggedInUser) {
	std::string filter = ot::rJSON::getString(_actionDocument, OT_PARAM_AUTH_PROJECT_FILTER);
	int limit = ot::rJSON::getInt(_actionDocument, OT_PARAM_AUTH_PROJECT_LIMIT);

	std::vector<Project> projects = MongoProjectFunctions::getAllUserProjects(_loggedInUser, filter, limit, adminClient);

	return MongoProjectFunctions::projectsToJson(projects);
}

std::string ServiceBase::handleGetAllProjects(OT_rJSON_doc& _actionDocument, User& _loggedInUser) {
	if (_loggedInUser.getUserId() != adminUser.getUserId())
	{
		throw std::runtime_error("The logged in user is not an admin user. Standard users can not retrieve a list of all projects.");
	}

	std::string filter = ot::rJSON::getString(_actionDocument, OT_PARAM_AUTH_PROJECT_FILTER);
	int limit = ot::rJSON::getInt(_actionDocument, OT_PARAM_AUTH_PROJECT_LIMIT);

	std::vector<Project> projects = MongoProjectFunctions::getAllProjects(_loggedInUser, filter, limit, adminClient);

	return MongoProjectFunctions::projectsToJson(projects);
}

std::string ServiceBase::handleGetAllProjectsCount(OT_rJSON_doc& _actionDocument, User& _loggedInUser) {
	size_t count = MongoProjectFunctions::getAllProjectCount(_loggedInUser, adminClient);

	return std::to_string(count);
}

std::string ServiceBase::handleGetAllGroupProjects(OT_rJSON_doc& _actionDocument, User& _loggedInUser) {
	std::string groupName = ot::rJSON::getString(_actionDocument, OT_PARAM_AUTH_GROUP_NAME);

	Group gr = MongoGroupFunctions::getGroupData(groupName, adminClient);
	std::vector<Project> projectsGr = MongoProjectFunctions::getAllGroupProjects(gr, adminClient);

	return MongoProjectFunctions::projectsToJson(projectsGr);
}

std::string ServiceBase::handleChangeProjectName(OT_rJSON_doc& _actionDocument, User& _loggedInUser) {
	std::string projectName = ot::rJSON::getString(_actionDocument, OT_PARAM_AUTH_PROJECT_NAME);
	std::string newProjectName = ot::rJSON::getString(_actionDocument, OT_PARAM_AUTH_NEW_PROJECT_NAME);

	Project proj = MongoProjectFunctions::getProject(projectName, adminClient);

	if (proj.creatingUser.getUserId() == _loggedInUser.getUserId() || _loggedInUser.getUserId() == adminUser.getUserId())
	{
		proj = MongoProjectFunctions::changeProjectName(proj, newProjectName, adminClient);
	}
	else
	{
		throw std::runtime_error("The logged in user is not the owner of this group! He cannot make the requested changes!");
	}

	return MongoProjectFunctions::projectToJson(proj);
}

std::string ServiceBase::handleChangeProjectOwner(OT_rJSON_doc& _actionDocument, User& _loggedInUser) {
	std::string projectName = ot::rJSON::getString(_actionDocument, OT_PARAM_AUTH_PROJECT_NAME);
	std::string newOwnerUsername = ot::rJSON::getString(_actionDocument, OT_PARAM_AUTH_NEW_PROJECT_OWNER);

	Project proj = MongoProjectFunctions::getProject(projectName, adminClient);
	User newOwner = MongoUserFunctions::getUserDataThroughUsername(newOwnerUsername, adminClient);

	if (proj.creatingUser.getUserId() == _loggedInUser.getUserId() || _loggedInUser.getUserId() == adminUser.getUserId())
	{
		proj = MongoProjectFunctions::changeProjectOwner(proj, newOwner, adminClient);
	}
	else
	{
		throw std::runtime_error("The logged in user is not the owner of this Project! He cannot make the requested changes!");
	}

	return MongoProjectFunctions::projectToJson(proj);
}

std::string ServiceBase::handleAddGroupToProject(OT_rJSON_doc& _actionDocument, User& _loggedInUser) {
	std::string groupName = ot::rJSON::getString(_actionDocument, OT_PARAM_AUTH_GROUP_NAME);
	std::string projectName = ot::rJSON::getString(_actionDocument, OT_PARAM_AUTH_PROJECT_NAME);
	Group gr = MongoGroupFunctions::getGroupData(groupName, adminClient);

	bool userIsGroupMember = false;
	for (auto groupUser : gr.users)
	{
		if (groupUser.getUserId() == _loggedInUser.getUserId())
		{
			userIsGroupMember = true;
			break;
		}
	}

	bool successful = false;
	if (userIsGroupMember)
	{
		Project pr = MongoProjectFunctions::getProject(projectName, adminClient);
		successful = MongoProjectFunctions::addGroupToProject(gr, pr, adminClient);
	}
	else
	{
		throw std::runtime_error("The logged in user is not a member of this group! He cannot make the requested changes!");
	}

	OT_rJSON_createDOC(json);
	ot::rJSON::add(json, "successful", successful);
	return ot::rJSON::toJSON(json);
}

std::string ServiceBase::handleRemoveGroupFromProject(OT_rJSON_doc& _actionDocument, User& _loggedInUser) {
	std::string groupName = ot::rJSON::getString(_actionDocument, OT_PARAM_AUTH_GROUP_NAME);
	std::string projectName = ot::rJSON::getString(_actionDocument, OT_PARAM_AUTH_PROJECT_NAME);
	Group gr = MongoGroupFunctions::getGroupData(groupName, adminClient);

	bool successful = false;
	if (gr.getOwnerId() == _loggedInUser.getUserId())
	{
		Project pr = MongoProjectFunctions::getProject(projectName, adminClient);
		successful = MongoProjectFunctions::removeGroupFromProject(gr, pr, adminClient);
	}
	else
	{
		throw std::runtime_error("The logged in user is not the owner of this group! He cannot make the requested changes!");
	}


	OT_rJSON_createDOC(json);
	ot::rJSON::add(json, "successful", successful);
	return ot::rJSON::toJSON(json);
}

std::string ServiceBase::handleRemoveProject(OT_rJSON_doc& _actionDocument, User& _loggedInUser) {
	std::string projectName = ot::rJSON::getString(_actionDocument, OT_PARAM_AUTH_PROJECT_NAME);

	Project pr = MongoProjectFunctions::getProject(projectName, adminClient);

	bool successful = false;
	if (pr.creatingUser.getUserId() == _loggedInUser.getUserId() || _loggedInUser.getUserId() == adminUser.getUserId())
	{
		successful = MongoProjectFunctions::removeProject(pr, adminClient);
	}
	else
	{
		throw std::runtime_error("The logged in user is not the owner of this Project! He cannot make the requested changes!");
	}

	OT_rJSON_createDOC(json);
	ot::rJSON::add(json, "successful", successful);
	return ot::rJSON::toJSON(json);
}

std::string ServiceBase::handleCheckIfCollectionExists(OT_rJSON_doc& _actionDocument, User& _loggedInUser)
{
	std::string collectionName = ot::rJSON::getString(_actionDocument, OT_PARAM_AUTH_COLLECTION_NAME);
	bool exist = MongoProjectFunctions::checkForCollectionExistence(collectionName, adminClient);
	
	OT_rJSON_createDOC(json);
	ot::rJSON::add(json, OT_PARAM_AUTH_COLLECTION_EXISTS, exist);
	return ot::rJSON::toJSON(json);
}
