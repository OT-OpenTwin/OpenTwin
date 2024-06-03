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

	std::string handleAdminLogIn(const ot::ConstJsonObject& _actionDocument);
	std::string handleLogIn(const ot::ConstJsonObject& _actionDocument);
	std::string handleRegister(const ot::ConstJsonObject& _actionDocument);

	// authentication needed: user functions

	std::string handleGetUserData(const ot::ConstJsonObject& _actionDocument);
	std::string handleGetAllUsers(const ot::ConstJsonObject& _actionDocument);
	std::string handleGetAllUsersCount(const ot::ConstJsonObject& _actionDocument);
	std::string handleChangeUserNameByUser(const ot::ConstJsonObject& _actionDocument, const User& _loggedInUser, const std::string& _loggedInUserPassword);
	std::string handleChangeUserPasswordByUser(const ot::ConstJsonObject& _actionDocument, const User& _loggedInUser, const std::string& _loggedInUserPassword);
	std::string handleDeleteUser(const ot::ConstJsonObject& _actionDocument, User& _loggedInUser, const std::string& _loggedInUserPassword);
	std::string handleChangeUserNameByAdmin(const ot::ConstJsonObject& _actionDocument, User& _loggedInUser, const std::string& _loggedInUserPassword);
	std::string handleChangeUserPasswordByAdmin(const ot::ConstJsonObject& _actionDocument, User& _loggedInUser, const std::string& _loggedInUserPassword);

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
	std::string handleGetAllProjectOwners(const ot::ConstJsonObject& _actionDocument, User& _loggedInUser);
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

