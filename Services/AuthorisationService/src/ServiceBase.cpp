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
	serviceURL  = _ownIP;
	databaseURL = _databaseIP;
	if (databaseURL.find("tls@") == 0)
	{
		databaseURL = databaseURL.substr(4);
	}
	databasePWD = _databasePWD;

	OT_LOG_I("Starting Authorisation Service");
	OT_LOG_D("Service address : " + serviceURL);
	OT_LOG_D("Database address: " + databaseURL);

	// NEEDED AND MUST BE EXECUTED ONCE
	mongocxx::instance inst{};

	// Initializing the Admin Client at the time of starting the service.

	// Maybe fetch the admin Credentials through the environment 

	dbUsername = getAdminUserName();

	if (!databasePWD.empty())
	{
		dbPassword = databasePWD;
	}
	else
	{
		dbPassword = ot::UserCredentials::encryptString("admin");
	}

	try
	{
		std::string uriStr = getMongoURL(databaseURL, dbUsername, ot::UserCredentials::decryptString(dbPassword));
		mongocxx::uri uri(uriStr);
		adminClient = mongocxx::client(uri);
	}
	catch (std::exception err)
	{
		OT_LOG_E("Cannot establish MongoDB connection: " + std::string(err.what())); // todo: should retry or exit?
		exit(ot::AppExitCode::DataBaseConnectionFailed);
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

	try 
	{
		MongoRoleFunctions::createInitialLibrariesDbRole(adminClient);
	}
	catch (std::runtime_error err) {
		std::string errMsg(err.what());
		if (errMsg.find(DB_ERROR_MESSAGE_ALREADY_EXISTS) != std::string::npos && errMsg.find(DB_ERROR_MESSAGE_ALREADY_EXISTS) != errMsg.length() - strlen(DB_ERROR_MESSAGE_ALREADY_EXISTS)) {
			OT_LOG_E("Error creating initial libraries db role in database: " + std::string(err.what()));
		}
	}

	/* HANDLING USER ROLE AND INDEXES */

	try
	{
		MongoRoleFunctions::createInitialUserRole(adminClient);
	}
	catch (std::runtime_error err)
	{
		std::string errMsg(err.what());
		if (errMsg.find(DB_ERROR_MESSAGE_ALREADY_EXISTS) != std::string::npos && errMsg.find(DB_ERROR_MESSAGE_ALREADY_EXISTS) != errMsg.length() - strlen(DB_ERROR_MESSAGE_ALREADY_EXISTS)) {
			OT_LOG_E("Error creating initial user db role in database: " + std::string(err.what()));
		}
	}

	try
	{
		// Role so that the admin can list the user DB collections
		MongoRoleFunctions::createInitialUserDbListCollectionsRole(adminClient);
	}
	catch (std::runtime_error err)
	{
		std::string errMsg(err.what());
		if (errMsg.find(DB_ERROR_MESSAGE_ALREADY_EXISTS) != std::string::npos && errMsg.find(DB_ERROR_MESSAGE_ALREADY_EXISTS) != errMsg.length() - strlen(DB_ERROR_MESSAGE_ALREADY_EXISTS)) {
			OT_LOG_E("Error creating initial user list role in database: " + std::string(err.what()));
		}
	}

	try
	{
		index_options.name("user_name_unique");
		adminClient[MongoConstants::USER_DB][MongoConstants::USER_CATALOG_COLLECTION]
			.create_index(bsoncxx::builder::basic::make_document(bsoncxx::builder::basic::kvp("user_name", 1)), index_options);
	}
	catch (std::runtime_error err)
	{
		OT_LOG_E("Error creating db index user_name_unique: " + std::string(err.what()));
	}

	try
	{
		index_options.name("user_id_unique");
		adminClient[MongoConstants::USER_DB][MongoConstants::USER_CATALOG_COLLECTION]
			.create_index(bsoncxx::builder::basic::make_document(bsoncxx::builder::basic::kvp("user_id", 1)), index_options);
	}
	catch (std::runtime_error err)
	{
		OT_LOG_E("Error creating db index user_id_unique: " + std::string(err.what()));
	}

	// Remove outdated sessions and start time
	std::thread workerThread(&ServiceBase::removeOldSessionsWorker, this);
	workerThread.detach();

	return ot::AppExitCode::Success;
}

bool ServiceBase::removeOldSessionsWorker()
{
	while (1)
	{
		MongoSessionFunctions::removeOldSessions(adminClient);

		using namespace std::chrono_literals;
		std::this_thread::sleep_for(3600s);  // Wait for one hour
	}
}

bool ServiceBase::isAdminUser(User& _loggedInUser)
{
	return (_loggedInUser.username == getAdminUserName());
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
		bool logInSuccessful = MongoUserFunctions::authenticateUser(loggedInUsername, loggedInUserPassword, databaseURL, adminClient);

		if (!logInSuccessful)
		{
			assert(0);
			throw std::runtime_error("The User could not be authenticated! Please logout and attempt to log in again!");
		}

		loggedInUser = MongoUserFunctions::getUserDataThroughUsername(loggedInUsername, adminClient);
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

// No authentication needed

std::string ServiceBase::handleAdminLogIn(const ot::ConstJsonObject& _actionDocument) {
	std::string username = ot::json::getString(_actionDocument, OT_PARAM_AUTH_USERNAME);
	std::string password = ot::json::getString(_actionDocument, OT_PARAM_AUTH_PASSWORD);
	bool encryptedPassword = ot::json::getBool(_actionDocument, OT_PARAM_AUTH_ENCRYPTED_PASSWORD);

	if (encryptedPassword)
	{
		password = ot::UserCredentials::decryptString(password);
	}

	bool successful = (username == dbUsername && password == ot::UserCredentials::decryptString(dbPassword));

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

	bool successful = MongoUserFunctions::authenticateUser(username, password, databaseURL, adminClient);

	ot::JsonDocument json;
	json.AddMember(OT_ACTION_AUTH_SUCCESS, successful, json.GetAllocator());
	
	if (successful)
	{
		std::string sessionName = MongoSessionFunctions::createSession(username, adminClient);
		std::string sessionPWD  = createRandomPassword();

		User user = MongoUserFunctions::getUserDataThroughUsername(username, adminClient);

		MongoUserFunctions::createTmpUser(sessionName, sessionPWD, user, adminClient, json);

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

	bool successful = MongoUserFunctions::registerUser(username, password, adminClient);

	ot::JsonDocument json;
	json.AddMember(OT_ACTION_AUTH_SUCCESS, successful, json.GetAllocator());
	return json.toJson();
}

std::string ServiceBase::handleRefreshSession(const ot::ConstJsonObject& _actionDocument) {
	std::string session = ot::json::getString(_actionDocument, OT_PARAM_DB_USERNAME);

	return MongoSessionFunctions::refreshSession(session, adminClient);
}

std::string ServiceBase::handleSetGlobalLoggingMode(const ot::ConstJsonObject& _actionDocument) {
	ot::ConstJsonArray flags = ot::json::getArray(_actionDocument, OT_ACTION_PARAM_Flags);
	ot::LogDispatcher::instance().setLogFlags(ot::logFlagsFromJsonArray(flags));
	return OT_ACTION_RETURN_VALUE_OK;
}

// authentication needed: user functions

std::string ServiceBase::handleGetUserData(const ot::ConstJsonObject& _actionDocument) {
	std::string username = ot::json::getString(_actionDocument, OT_PARAM_AUTH_USERNAME);
	User usr = MongoUserFunctions::getUserDataThroughUsername(username, adminClient);

	return MongoUserFunctions::userToJson(usr);
}

std::string ServiceBase::handleGetAllUsers(const ot::ConstJsonObject& _actionDocument) {
	std::vector<User> allUsers = MongoUserFunctions::getAllUsers(adminClient);

	return MongoUserFunctions::usersToJson(allUsers);
}

std::string ServiceBase::handleGetAllUsersCount(const ot::ConstJsonObject& _actionDocument) {
	size_t count = MongoUserFunctions::getAllUserCount(adminClient);
	return std::to_string(count);
}

std::string ServiceBase::handleChangeUserNameByUser(const ot::ConstJsonObject& _actionDocument, const User& _loggedInUser, const std::string& _loggedInUserPassword) {
	std::string newUsername = ot::json::getString(_actionDocument, OT_PARAM_AUTH_NEW_USERNAME);
	bool successful = MongoUserFunctions::updateUserUsernameByName(_loggedInUser.username, newUsername, adminClient);

	ot::JsonDocument json;
	json.AddMember(OT_ACTION_AUTH_SUCCESS, successful, json.GetAllocator());
	return json.toJson();
}

std::string ServiceBase::handleChangeUserPasswordByUser(const ot::ConstJsonObject& _actionDocument, const User& _loggedInUser, const std::string& _loggedInUserPassword) {
	std::string password = ot::json::getString(_actionDocument, OT_PARAM_AUTH_PASSWORD);
	bool successful = MongoUserFunctions::changeUserPassword(_loggedInUser.username, password, adminClient);

	ot::JsonDocument json;
	json.AddMember(OT_ACTION_AUTH_SUCCESS, successful, json.GetAllocator());
	return json.toJson();
}

std::string ServiceBase::handleDeleteUser(const ot::ConstJsonObject& _actionDocument, User& _loggedInUser, const std::string& _loggedInUserPassword) {
	std::string usernameToDelete = ot::json::getString(_actionDocument, OT_PARAM_USERNAME_TO_DELETE);

	User toBeDeletedUser = MongoUserFunctions::getUserDataThroughUsername(usernameToDelete, adminClient);

	bool successful = false;
	// Check if the user wants do delete himself or if the call comes from the Admin User
	if (_loggedInUser.userId == toBeDeletedUser.userId || isAdminUser(_loggedInUser))
	{
		successful = MongoUserFunctions::removeUser(toBeDeletedUser, adminClient);
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

	bool successful = MongoUserFunctions::updateUsername(oldUsername, newUsername, adminClient);

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

	bool successful = MongoUserFunctions::changeUserPassword(userName, newPassword, adminClient);

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

// authentication needed: group functions

std::string ServiceBase::handleCreateGroup(const ot::ConstJsonObject& _actionDocument, User& _loggedInUser) {
	std::string groupName = ot::json::getString(_actionDocument, OT_PARAM_AUTH_GROUP_NAME);

	MongoGroupFunctions::createGroup(groupName, _loggedInUser, adminClient);
	Group gr = MongoGroupFunctions::getGroupDataByName(groupName, adminClient);

	return MongoGroupFunctions::groupToJson(gr);
}

std::string ServiceBase::handleGetGroupData(const ot::ConstJsonObject& _actionDocument) {
	std::string groupName = ot::json::getString(_actionDocument, OT_PARAM_AUTH_GROUP_NAME);

	Group gr = MongoGroupFunctions::getGroupDataByName(groupName, adminClient);

	return MongoGroupFunctions::groupToJson(gr);
}

std::string ServiceBase::handleGetAllUserGroups(const ot::ConstJsonObject& _actionDocument, User& _loggedInUser) {
	std::vector<Group> groups = MongoGroupFunctions::getAllUserGroups(_loggedInUser, adminClient);

	return MongoGroupFunctions::groupsToJson(groups);
}

std::string ServiceBase::handleGetAllGroups(const ot::ConstJsonObject& _actionDocument, User& _loggedInUser) {
	if (!isAdminUser(_loggedInUser))
	{
		throw std::runtime_error("The logged in user is not an admin user. Standard users can not retrieve a list of all groups.");
	}

	std::vector<Group> groups = MongoGroupFunctions::getAllGroups(_loggedInUser, adminClient);

	return MongoGroupFunctions::groupsToJson(groups);
}

std::string ServiceBase::handleGetAllGroupsCount(const ot::ConstJsonObject& _actionDocument, User& _loggedInUser) {
	size_t count = MongoGroupFunctions::getAllGroupCount(_loggedInUser, adminClient);
	return std::to_string(count);
}

std::string ServiceBase::handleChangeGroupName(const ot::ConstJsonObject& _actionDocument, User& _loggedInUser) {
	std::string groupName = ot::json::getString(_actionDocument, OT_PARAM_AUTH_GROUP_NAME);
	std::string newGroupName = ot::json::getString(_actionDocument, OT_PARAM_AUTH_NEW_GROUP_NAME);

	Group gr = MongoGroupFunctions::getGroupDataByName(groupName, adminClient);

	bool successful = false;

	if (gr.ownerUserId == _loggedInUser.userId || isAdminUser(_loggedInUser))
	{
		successful = MongoGroupFunctions::changeGroupName(groupName, newGroupName, adminClient);
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

	User newOwner = MongoUserFunctions::getUserDataThroughUsername(newOwnerusername, adminClient);

	Group group = MongoGroupFunctions::getGroupDataByName(groupName, adminClient);

	bool successful = false;
	if (group.ownerUserId == _loggedInUser.userId || isAdminUser(_loggedInUser))
	{
		successful = MongoGroupFunctions::changeGroupOwner(group, _loggedInUser, newOwner, adminClient);
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
	Group gr = MongoGroupFunctions::getGroupDataByName(groupName, adminClient);


	bool successful = false;
	if (gr.ownerUserId == _loggedInUser.userId)
	{
		User usr = MongoUserFunctions::getUserDataThroughUsername(username, adminClient);
		successful = MongoGroupFunctions::addUserToGroup(usr, groupName, adminClient);
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
	Group gr = MongoGroupFunctions::getGroupDataByName(groupName, adminClient);

	bool successful = false;
	if (gr.ownerUserId == _loggedInUser.userId)
	{
		User usr = MongoUserFunctions::getUserDataThroughUsername(username, adminClient);
		successful = MongoGroupFunctions::removeUserFromGroup(usr, groupName, adminClient);
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

	auto group = MongoGroupFunctions::getGroupDataByName(groupName, adminClient);

	auto groupOwnerId = group.ownerUserId;
	auto requestingUserId = _loggedInUser.userId;

	bool successful = false;
	if (groupOwnerId == requestingUserId || isAdminUser(_loggedInUser))
	{
		successful = MongoGroupFunctions::removeGroup(group, adminClient);
	}
	else
	{
		throw std::runtime_error("The logged in user is not the owner of this group! He cannot make the requested changes!");
	}

	ot::JsonDocument json;
	json.AddMember(OT_ACTION_AUTH_SUCCESS, successful, json.GetAllocator());
	return json.toJson();
}

// authentication needed: project functions

std::string ServiceBase::handleCreateProject(const ot::ConstJsonObject& _actionDocument, User& _loggedInUser) {
	std::string projectName = ot::json::getString(_actionDocument, OT_PARAM_AUTH_PROJECT_NAME);
	std::string projectType = ot::json::getString(_actionDocument, OT_PARAM_AUTH_PROJECT_TYPE);

	Project createdProject = MongoProjectFunctions::createProject(projectName, projectType, _loggedInUser, adminClient);

	return ot::ReturnMessage(ot::ReturnMessage::Ok, createdProject.toProjectInformation().toJson()).toJson();
}

std::string ServiceBase::handleGetProjectData(const ot::ConstJsonObject& _actionDocument) {
	std::string projectName = ot::json::getString(_actionDocument, OT_PARAM_AUTH_PROJECT_NAME);

	try
	{
		Project proj = MongoProjectFunctions::getProject(projectName, adminClient);
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
	return MongoProjectFunctions::getProjectsInfo(projectNames, adminClient);
}

std::string ServiceBase::handleGetAllUserProjects(const ot::ConstJsonObject& _actionDocument, User& _loggedInUser) {
	std::string filter = ot::json::getString(_actionDocument, OT_PARAM_AUTH_PROJECT_FILTER);
	int limit = ot::json::getInt(_actionDocument, OT_PARAM_AUTH_PROJECT_LIMIT);

	std::vector<Project> projects = MongoProjectFunctions::getAllUserProjects(_loggedInUser, filter, limit, adminClient);

	return MongoProjectFunctions::projectsToJson(projects);
}

std::string ServiceBase::handleGetAllProjects(const ot::ConstJsonObject& _actionDocument, User& _loggedInUser) {
	if (!isAdminUser(_loggedInUser))
	{
		throw std::runtime_error("The logged in user is not an admin user. Standard users can not retrieve a list of all projects.");
	}

	std::string filter = ot::json::getString(_actionDocument, OT_PARAM_AUTH_PROJECT_FILTER);
	int limit = ot::json::getInt(_actionDocument, OT_PARAM_AUTH_PROJECT_LIMIT);

	std::vector<Project> projects = MongoProjectFunctions::getAllProjects(_loggedInUser, filter, limit, adminClient);

	return MongoProjectFunctions::projectsToJson(projects);
}

std::string ServiceBase::handleGetAllProjectsCount(const ot::ConstJsonObject& _actionDocument, User& _loggedInUser) {
	size_t count = MongoProjectFunctions::getAllProjectCount(_loggedInUser, adminClient);

	return std::to_string(count);
}

std::string ServiceBase::handleGetAllGroupProjects(const ot::ConstJsonObject& _actionDocument, User& _loggedInUser) {
	std::string groupName = ot::json::getString(_actionDocument, OT_PARAM_AUTH_GROUP_NAME);

	Group gr = MongoGroupFunctions::getGroupDataByName(groupName, adminClient);
	std::vector<Project> projectsGr = MongoProjectFunctions::getAllGroupProjects(gr, adminClient);

	return MongoProjectFunctions::projectsToJson(projectsGr);
}

std::string ServiceBase::handleChangeProjectName(const ot::ConstJsonObject& _actionDocument, User& _loggedInUser) {
	std::string projectName = ot::json::getString(_actionDocument, OT_PARAM_AUTH_PROJECT_NAME);
	std::string newProjectName = ot::json::getString(_actionDocument, OT_PARAM_AUTH_NEW_PROJECT_NAME);

	Project proj = MongoProjectFunctions::getProject(projectName, adminClient);

	if (proj.getUser().userId == _loggedInUser.userId || isAdminUser(_loggedInUser))
	{
		proj = MongoProjectFunctions::changeProjectName(proj, newProjectName, adminClient);
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

	Project proj = MongoProjectFunctions::getProject(projectName, adminClient);
	User newOwner = MongoUserFunctions::getUserDataThroughUsername(newOwnerUsername, adminClient);

	if (proj.getUser().userId == _loggedInUser.userId || isAdminUser(_loggedInUser))
	{
		proj = MongoProjectFunctions::changeProjectOwner(proj, newOwner, adminClient);
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
	Group gr = MongoGroupFunctions::getGroupDataByName(groupName, adminClient);

	bool userIsGroupMember = false;
	for (auto groupUser : gr.users)
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
		Project pr = MongoProjectFunctions::getProject(projectName, adminClient);
		successful = MongoProjectFunctions::addGroupToProject(gr, pr, adminClient);
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
	Group gr = MongoGroupFunctions::getGroupDataByName(groupName, adminClient);

	bool successful = false;
	if (gr.ownerUserId == _loggedInUser.userId)
	{
		Project pr = MongoProjectFunctions::getProject(projectName, adminClient);
		successful = MongoProjectFunctions::removeGroupFromProject(gr, pr, adminClient);
	}
	else
	{
		throw std::runtime_error("The logged in user is not the owner of this group! He cannot make the requested changes!");
	}

	ot::JsonDocument json;
	json.AddMember(OT_ACTION_AUTH_SUCCESS, successful, json.GetAllocator());
	return json.toJson();
}

std::string ServiceBase::handleRemoveProject(const ot::ConstJsonObject& _actionDocument, User& _loggedInUser) {
	std::string projectName = ot::json::getString(_actionDocument, OT_PARAM_AUTH_PROJECT_NAME);

	Project pr = MongoProjectFunctions::getProject(projectName, adminClient);

	bool successful = false;
	if (pr.getUser().userId == _loggedInUser.userId || isAdminUser(_loggedInUser))
	{
		successful = MongoProjectFunctions::removeProject(pr, adminClient);
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
	bool exist = MongoProjectFunctions::checkForCollectionExistence(collectionName, adminClient);
	
	ot::JsonDocument json;
	json.AddMember(OT_PARAM_AUTH_COLLECTION_EXISTS, exist, json.GetAllocator());
	return json.toJson();
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
