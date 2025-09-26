#pragma once

#include <string>
#include <mutex>

#include "OTCommunication/CommunicationTypes.h"

#include <bsoncxx/json.hpp>
#include <bsoncxx/types.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/stdx.hpp>
#include <mongocxx/uri.hpp>
#include <mongocxx/instance.hpp>
#include <bsoncxx/builder/stream/helpers.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/builder/stream/array.hpp>

#include "MongoGroupFunctions.h"
#include "MongoConstants.h"
#include "MongoUserFunctions.h"
#include "MongoProjectFunctions.h"
#include "MongoRoleFunctions.h"
#include "Group.h"
#include "Project.h"
#include "User.h"

class ServiceBase
{
public:

	static ServiceBase& GetInstance() {
		// Allocate with `new` in case Singleton is not trivially destructible.
		static ServiceBase* serviceBase = new ServiceBase();
		return *serviceBase;
	}

	int initialize(const char * _ownIP, const char * _databaseIP, const char * _databasePWD);
	const char *getServiceURL(void);

	std::string performAction(const char * _json, const char * _senderIP);
	std::string performActionOneWayTLS(const char * _json, const char * _senderIP);

	std::string dispatchAction(const char * _json, ot::MessageType _messageType);

private:
	
	std::string dispatchAction(const std::string& _action, const ot::JsonDocument& _actionDocument, ot::MessageType _messageType);

	// No authentication needed

	//api @SECURITY TLS
	//api @Action OT_ACTION_LOGIN_ADMIN 
	//api @Brief Processes the login of an administrator.
	//api This is the detailed description.
	//api This is the second line of the detailed description.
	//api
	//api @NOTE This is an important note.
	//api This line belongs to the note block.
	//api @Endnote This is the third line of the detailed description.
	//api @warNing This is a warning.
	//api
	//api This line belongs to the warning block.
	//api @Endwarning This is the fourth line of the detailed description.
	//api This is the fifth line of the detailed description.
	//api @PARAM OT_PARAM_AUTH_USERNAME String User name
	//api This is the detailed description of OT_PARAM_AUTH_USERNAME.
	//api This is the second line of the detailed description of OT_PARAM_AUTH_USERNAME.
	//api @NOte This is a note in the detailed description of OT_PARAM_AUTH_USERNAME.
	//api This is the second line in the note block of OT_PARAM_AUTH_USERNAME.
	//api @WARNING This warning follows the note block.
	//api This is the second line in the warning block of OT_PARAM_AUTH_USERNAME.
	//api @endwarning This is the third line in the detailed description of OT_PARAM_AUTH_USERNAME.
	//api This is the fourth line in the detailed description of OT_PARAM_AUTH_USERNAME.
	//api @note This is a note.
	//api @endnote This is the fifth line.
	//api @param OT_PARAM_AUTH_PASSWORD String User password
	//api This is the detailed description of OT_PARAM_AUTH_PASSWORD.
	//api This is the second line of the detailed description of OT_PARAM_AUTH_PASSWORD.
	//api @param OT_PARAM_AUTH_ENCRYPTED_PASSWORD Boolean Is the password encrypted 
	//api This is the detailed description of OT_PARAM_AUTH_ENCRYPTED_PASSWORD.
	//api This is the second line of the detailed description of OT_PARAM_AUTH_ENCRYPTED_PASSWORD.
	//api @Rparam OT_ACTION_AUTH_SUCCESS Boolean Are the credentials correct
	//api This is the detailed description of OT_ACTION_AUTH_SUCCESS.
	//api This is the second line of the detailed description of OT_ACTION_AUTH_SUCCESS. 
	//api @rparam OT_PARAM_AUTH_ENCRYPTED_PASSWORD String Encrypted password (if successful)
	//api This is the detailed description of OT_PARAM_AUTH_ENCRYPTED_PASSWORD.
	//api This is the second line of the detailed description of OT_PARAM_AUTH_ENCRYPTED_PASSWORD.
	//api @NOTE This is a note in the detailed description of OT_PARAM_AUTH_ENCRYPTED_PASSWORD.
	//api This is the second line in the note block of OT_PARAM_AUTH_ENCRYPTED_PASSWORD.
	//api @Warning This warning follows the note block.
	//api This is the second line in the warning block of OT_PARAM_AUTH_ENCRYPTED_PASSWORD.
	//api @endwarning This is the third line in the note block of OT_PARAM_AUTH_ENCRYPTED_PASSWORD.
	//api This is the fourth line in the note block of OT_PARAM_AUTH_ENCRYPTED_PASSWORD.
	//api @return This is the response desciption of OT_ACTION_LOGIN_ADMIN.
	//api This is the second line of the response description of OT_ACTION_LOGIN_ADMIN.
	//api
	//api @note This is an important note of OT_ACTION_LOGIN_ADMIN.
	//api This line belongs to the note block.
	//api @endnote This is the third line of the response description of OT_ACTION_LOGIN_ADMIN.
	//api @Warning This is a warning of OT_ACTION_LOGIN_ADMIN.
	//api
	//api This line belongs to the warning block.
	//api @endwarning This is the fourth line of the response description of OT_ACTION_LOGIN_ADMIN.
	//api This is the fifth line of the response description of OT_ACTION_LOGIN_ADMIN.
	std::string handleAdminLogIn(const ot::ConstJsonObject& _actionDocument);
	
	//api @security TLS
	//api @action OT_ACTION_LOGIN
	//api @brief Processes the login of a user.
	//api This is the detailed description.
	//api This is the second line of the detailed description.
	//api @param OT_PARAM_AUTH_USERNAME String User name
	//api This is the detailed description of OT_PARAM_AUTH_USERNAME.
	//api This is the second line of the detailed description of OT_PARAM_AUTH_USERNAME.
	//api @note This is a note in the detailed description of OT_PARAM_AUTH_USERNAME.
	//api This is the second line in the note block of OT_PARAM_AUTH_USERNAME.
	//api @param OT_PARAM_AUTH_PASSWORD String User password
	//api @param OT_PARAM_AUTH_ENCRYPTED_PASSWORD Boolean Is the password encrypted 
	//api @rparam OT_ACTION_AUTH_SUCCESS Boolean Are the credentials correct
	//api @rparam OT_PARAM_AUTH_PASSWORD String Unencrypted password (if successful)
	//api @rparam OT_PARAM_AUTH_ENCRYPTED_PASSWORD String Encrypted password (if successful)
	//api This is the detailed description of OT_PARAM_AUTH_ENCRYPTED_PASSWORD.
	//api This is the second line of the detailed description of OT_PARAM_AUTH_ENCRYPTED_PASSWORD.
	//api @note This is a note in the detailed description of OT_PARAM_AUTH_ENCRYPTED_PASSWORD.
	//api This is the second line in the note block of OT_PARAM_AUTH_ENCRYPTED_PASSWORD.
	//api @warning This warning follows the note block.
	//api This is the second line in the warning block of OT_PARAM_AUTH_ENCRYPTED_PASSWORD.
	//api @detail This is the third line in the note block of OT_PARAM_AUTH_ENCRYPTED_PASSWORD.
	//api This is the fourth line in the note block of OT_PARAM_AUTH_ENCRYPTED_PASSWORD.
	//api @return This is the response desciption of OT_ACTION_LOGIN.
	//api This is the second line of the response description of OT_ACTION_LOGIN.
	//api
	//api @note This is an important note of OT_ACTION_LOGIN.
	//api This line belongs to the note block.
	//api @endnote This is the third line of the response description of OT_ACTION_LOGIN.
	//api @warning This is a warning of OT_ACTION_LOGIN.
	//api
	//api This line belongs to the warning block.
	//api @endwarning This is the fourth line of the response description of OT_ACTION_LOGIN.
	//api This is the fifth line of the response description of OT_ACTION_LOGIN.
	std::string handleLogIn(const ot::ConstJsonObject& _actionDocument);
	std::string handleRegister(const ot::ConstJsonObject& _actionDocument);
	std::string handleRefreshSession(const ot::ConstJsonObject& _actionDocument);
	std::string handleSetGlobalLoggingMode(const ot::ConstJsonObject& _actionDocument);

	// authentication needed: user functions

	std::string handleGetUserData(const ot::ConstJsonObject& _actionDocument);
	std::string handleGetAllUsers(const ot::ConstJsonObject& _actionDocument);
	std::string handleGetAllUsersCount(const ot::ConstJsonObject& _actionDocument);
	std::string handleChangeUserNameByUser(const ot::ConstJsonObject& _actionDocument, const User& _loggedInUser, const std::string& _loggedInUserPassword);
	std::string handleChangeUserPasswordByUser(const ot::ConstJsonObject& _actionDocument, const User& _loggedInUser, const std::string& _loggedInUserPassword);
	std::string handleDeleteUser(const ot::ConstJsonObject& _actionDocument, User& _loggedInUser, const std::string& _loggedInUserPassword);
	std::string handleChangeUserNameByAdmin(const ot::ConstJsonObject& _actionDocument, User& _loggedInUser, const std::string& _loggedInUserPassword);
	std::string handleChangeUserPasswordByAdmin(const ot::ConstJsonObject& _actionDocument, User& _loggedInUser, const std::string& _loggedInUserPassword);
	std::string handleGetSystemInformation(const ot::ConstJsonObject& _actionDocument, User& _loggedInUser, const std::string& _loggedInUserPassword);

	// authentication needed: group functions

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

	// authentication needed: project functions

	std::string handleCreateProject(const ot::ConstJsonObject& _actionDocument, User& _loggedInUser);
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

	// helper functions

	bool isAdminUser(User& _loggedInUser);
	std::string getAdminUserName() { return "admin"; }
	std::string createRandomPassword();
	bool removeOldSessionsWorker();

	std::string serviceURL;
	std::string databaseURL;
	std::string databasePWD;
	mongocxx::client adminClient;
	std::string dbUsername;
	std::string dbPassword;
	std::mutex m_mutex;

	ServiceBase() = default;

	// Delete copy/move so extra instances can't be created/moved.
	ServiceBase(const ServiceBase&) = delete;
	ServiceBase& operator=(const ServiceBase&) = delete;
	ServiceBase(ServiceBase&&) = delete;
	ServiceBase& operator=(ServiceBase&&) = delete;
};

