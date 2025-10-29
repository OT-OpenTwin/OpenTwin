// @otlicense

#pragma once

#include <iostream>
#include <cstdint>
#include <iostream>
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

#include "OTCommunication/CommunicationTypes.h"

#include "MongoConstants.h"
#include "User.h"

using bsoncxx::builder::stream::close_array;
using bsoncxx::builder::stream::close_document;
using bsoncxx::builder::stream::document;
using bsoncxx::builder::stream::finalize;
using bsoncxx::builder::stream::open_array;
using bsoncxx::builder::stream::open_document;
using bsoncxx::document::value;
using bsoncxx::document::view;
using bsoncxx::document::element;


namespace MongoUserFunctions
{
	bool authenticateUser(std::string username, std::string password, std::string databaseUrl, mongocxx::client& adminClient);

	bool registerUser(std::string username, std::string password, mongocxx::client& adminClient, std::string oldSettingsCollectionName = "");

	std::string generateUserSettingsCollectionName(mongocxx::client& adminClient);

	User getUserDataThroughUsername(const std::string& username, mongocxx::client& adminClient);

	User getUserDataThroughId(std::string userId, mongocxx::client& adminClient);

	std::vector<User> getAllUsers(mongocxx::client& adminClient);
	size_t getAllUserCount(mongocxx::client& adminClient);

	bool removeUser(User& userToBeDeleted, mongocxx::client& adminClient);

	bool updateUserUsernameById(std::string userId, std::string newUsername, mongocxx::client& adminClient);

	bool updateUserUsernameByName(std::string oldUsername, std::string newUsername, mongocxx::client& adminClient);

	bool changeUserPassword(std::string username, std::string newPassword, mongocxx::client& adminClient);

	bool updateUsername(std::string oldUsername, std::string newUsername, mongocxx::client& adminClient);

	std::string userToJson(User& user);
	std::string usersToJson(std::vector<User>& users);

	std::string hashPassword(const std::string &password);

	bool doesUserExist(const std::string &userName, mongocxx::client& adminClient);

	void createTmpUser(std::string userName, std::string userPWD, User& _loggedInUser, mongocxx::client& adminClient, ot::JsonDocument &json);
	void removeTmpUser(std::string userName, mongocxx::client& adminClient);
}
