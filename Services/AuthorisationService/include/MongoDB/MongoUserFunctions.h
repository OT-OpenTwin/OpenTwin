// @otlicense
// File: MongoUserFunctions.h
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
#include "Management/User.h"
#include "MongoDB/MongoConstants.h"

// OpenTwin header
#include "OTCommunication/CommunicationTypes.h"

// MongoDB header
#include <bsoncxx/json.hpp>
#include <bsoncxx/types.hpp>
#include <bsoncxx/builder/stream/array.hpp>
#include <bsoncxx/builder/stream/helpers.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <mongocxx/uri.hpp>
#include <mongocxx/stdx.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>

// std header
#include <vector>
#include <cstdint>
#include <iostream>

using bsoncxx::builder::stream::close_array;
using bsoncxx::builder::stream::close_document;
using bsoncxx::builder::stream::document;
using bsoncxx::builder::stream::finalize;
using bsoncxx::builder::stream::open_array;
using bsoncxx::builder::stream::open_document;
using bsoncxx::document::value;
using bsoncxx::document::view;
using bsoncxx::document::element;

class MongoUserFunctions
{
	OT_DECL_STATICONLY(MongoUserFunctions)
private:
	static std::string getOriginalUserName(const std::string& username, mongocxx::database& db);
	static bool checkMongodbLogin(const std::string& username, const std::string& password, const std::string& databaseUrl, std::string& originalUserName);
public:
	static bool authenticateUser(std::string username, std::string password, std::string databaseUrl, mongocxx::client& adminClient, std::string& originalUserName);

	static bool authenticateUserTroughSessionCredentials(const std::string& _originalUserName, const std::string& _sessionUserName, const std::string& _sessionPassword, mongocxx::client& adminClient);

	static bool registerUser(std::string username, std::string password, mongocxx::client& adminClient, std::string oldSettingsCollectionName = "");

	static std::string generateUserSettingsCollectionName(mongocxx::client& adminClient);

	static User getUserDataThroughUsername(const std::string& username, mongocxx::client& adminClient);

	static User getUserDataThroughId(std::string userId, mongocxx::client& adminClient);

	static std::vector<User> getAllUsers(mongocxx::client& adminClient);
	static size_t getAllUserCount(mongocxx::client& adminClient);

	static bool removeUser(User& userToBeDeleted, mongocxx::client& adminClient);

	static bool updateUserUsernameById(std::string userId, std::string newUsername, mongocxx::client& adminClient);

	static bool updateUserUsernameByName(std::string oldUsername, std::string newUsername, mongocxx::client& adminClient);

	static bool changeUserPassword(std::string username, std::string newPassword, mongocxx::client& adminClient);

	static bool updateUsername(std::string oldUsername, std::string newUsername, mongocxx::client& adminClient);

	static std::string userToJson(const User& user);
	static std::string usersToJson(const std::vector<User>& users);

	static std::string hashPassword(const std::string& password);

	static bool doesUserExist(const std::string& userName, mongocxx::client& adminClient);

	static void createTmpUser(std::string userName, std::string userPWD, User& _loggedInUser, std::string databaseUrl, mongocxx::client& adminClient, ot::JsonDocument& json);
	static void removeTmpUser(std::string userName, mongocxx::client& adminClient);

private:
	static std::set<std::string>& getAuthenticatedUserTokens();
	static std::map<std::string, std::string>& getAuthenticatedUserOriginalName();

};
