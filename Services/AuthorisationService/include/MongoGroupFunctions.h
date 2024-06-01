#pragma once


#include <iostream>
#include <cstdint>
#include <vector>
#include <bsoncxx/json.hpp>
#include <bsoncxx/types.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/stdx.hpp>
#include <mongocxx/uri.hpp>
#include <mongocxx/instance.hpp>
#include <bsoncxx/builder/stream/helpers.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/builder/stream/array.hpp>

#include "User.h"
#include "Group.h"

using bsoncxx::builder::stream::close_array;
using bsoncxx::builder::stream::close_document;
using bsoncxx::builder::stream::document;
using bsoncxx::builder::stream::finalize;
using bsoncxx::builder::stream::open_array;
using bsoncxx::builder::stream::open_document;
using bsoncxx::document::value;
using bsoncxx::document::view;
using bsoncxx::document::element;


/*
	1) Create Group
	2) Get All User Groups
	3) Get Group Data
	4) Edit group Data
	5) Add User to group
	6) Remove User from group
	7) Remove group
*/
namespace MongoGroupFunctions
{
	void createGroup(std::string groupName, User& creatingUser, mongocxx::client& adminClient);


	std::vector<Group> getAllUserGroups(User& loggedInUser, mongocxx::client& userClient);

	std::vector<Group> getAllGroups(User& loggedInUser, mongocxx::client& userClient);
	size_t getAllGroupCount(User& loggedInUser, mongocxx::client& userClient);

	Group getGroupDataByName(std::string groupName, mongocxx::client& userClient);

	Group getGroupDataById(std::string groupId, mongocxx::client& userClient);


	bool changeGroupName(std::string oldName, std::string newName, mongocxx::client& adminClient);

	bool changeGroupOwner(Group& group, User& oldUser, User& newUser, mongocxx::client& adminClient);

	bool addUserToGroup(User& loggedInUser, std::string groupName, mongocxx::client& adminClient);


	bool removeUserFromGroup(User& loggedInUser, std::string groupName, mongocxx::client& adminClient);

	bool removeGroup(Group& groupToBeRemoved, mongocxx::client& adminClient);

	std::string groupToJson(Group& grp);

	std::string groupsToJson(std::vector<Group>& groups);
	std::string generateUniqueGroupName(mongocxx::client& adminClient);

}