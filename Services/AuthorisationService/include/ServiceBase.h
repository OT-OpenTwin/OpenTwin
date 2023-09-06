#pragma once

#include <string>
#include <mutex>

#include "OpenTwinCore/rJSON.h"
#include "OpenTwinCommunication/CommunicationTypes.h"

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
	
	std::string dispatchAction(const std::string& _action, OT_rJSON_doc& _actionDocument, ot::MessageType _messageType);

	// No authentication needed

	std::string handleAdminLogIn(OT_rJSON_doc& _actionDocument);
	std::string handleLogIn(OT_rJSON_doc& _actionDocument);
	std::string handleRegister(OT_rJSON_doc& _actionDocument);

	// authentication needed: user functions

	std::string handleGetUserData(OT_rJSON_doc& _actionDocument);
	std::string handleGetAllUsers(OT_rJSON_doc& _actionDocument);
	std::string handleGetAllUsersCount(OT_rJSON_doc& _actionDocument);
	std::string handleChangeUserNameByUser(OT_rJSON_doc& _actionDocument, const User& _loggedInUser, const std::string& _loggedInUserPassword);
	std::string handleChangeUserPasswordByUser(OT_rJSON_doc& _actionDocument, const User& _loggedInUser, const std::string& _loggedInUserPassword);
	std::string handleDeleteUser(OT_rJSON_doc& _actionDocument, User& _loggedInUser, const std::string& _loggedInUserPassword);
	std::string handleChangeUserNameByAdmin(OT_rJSON_doc& _actionDocument, User& _loggedInUser, const std::string& _loggedInUserPassword);
	std::string handleChangeUserPasswordByAdmin(OT_rJSON_doc& _actionDocument, User& _loggedInUser, const std::string& _loggedInUserPassword);

	// authentication needed: group functions

	std::string handleCreateGroup(OT_rJSON_doc& _actionDocument, User& _loggedInUser);
	std::string handleGetGroupData(OT_rJSON_doc& _actionDocument);
	std::string handleGetAllUserGroups(OT_rJSON_doc& _actionDocument, User& _loggedInUser);
	std::string handleGetAllGroups(OT_rJSON_doc& _actionDocument, User& _loggedInUser);
	std::string handleGetAllGroupsCount(OT_rJSON_doc& _actionDocument, User& _loggedInUser);
	std::string handleChangeGroupName(OT_rJSON_doc& _actionDocument, User& _loggedInUser);
	std::string handleChangeGroupOwner(OT_rJSON_doc& _actionDocument, User& _loggedInUser);
	std::string handleAddUserToGroup(OT_rJSON_doc& _actionDocument, User& _loggedInUser);
	std::string handleRemoveUserFromGroup(OT_rJSON_doc& _actionDocument, User& _loggedInUser);
	std::string handleRemoveGroup(OT_rJSON_doc& _actionDocument, User& _loggedInUser);

	// authentication needed: project functions

	std::string handleCreateProject(OT_rJSON_doc& _actionDocument, User& _loggedInUser);
	std::string handleGetProjectData(OT_rJSON_doc& _actionDocument);
	std::string handleGetAllProjectOwners(OT_rJSON_doc& _actionDocument, User& _loggedInUser);
	std::string handleGetAllUserProjects(OT_rJSON_doc& _actionDocument, User& _loggedInUser);
	std::string handleGetAllProjects(OT_rJSON_doc& _actionDocument, User& _loggedInUser);
	std::string handleGetAllProjectsCount(OT_rJSON_doc& _actionDocument, User& _loggedInUser);
	std::string handleGetAllGroupProjects(OT_rJSON_doc& _actionDocument, User& _loggedInUser);
	std::string handleChangeProjectName(OT_rJSON_doc& _actionDocument, User& _loggedInUser);
	std::string handleChangeProjectOwner(OT_rJSON_doc& _actionDocument, User& _loggedInUser);
	std::string handleAddGroupToProject(OT_rJSON_doc& _actionDocument, User& _loggedInUser);
	std::string handleRemoveGroupFromProject(OT_rJSON_doc& _actionDocument, User& _loggedInUser);
	std::string handleRemoveProject(OT_rJSON_doc& _actionDocument, User& _loggedInUser);
	std::string handleCheckIfCollectionExists(OT_rJSON_doc& _actionDocument, User& _loggedInUser);


	std::string serviceURL;
	std::string databaseURL;
	std::string databasePWD;
	mongocxx::client adminClient;
	std::string dbUsername;
	std::string dbPassword;
	User adminUser;
	std::mutex m_mutex;

	ServiceBase() = default;

	// Delete copy/move so extra instances can't be created/moved.
	ServiceBase(const ServiceBase&) = delete;
	ServiceBase& operator=(const ServiceBase&) = delete;
	ServiceBase(ServiceBase&&) = delete;
	ServiceBase& operator=(ServiceBase&&) = delete;
};

