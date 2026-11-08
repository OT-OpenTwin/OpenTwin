// @otlicense
// File: MongoGroupFunctions.h
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
#include "Management/Group.h"

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
#include <iostream>
#include <cstdint>
#include <vector>

using bsoncxx::builder::stream::close_array;
using bsoncxx::builder::stream::close_document;
using bsoncxx::builder::stream::document;
using bsoncxx::builder::stream::finalize;
using bsoncxx::builder::stream::open_array;
using bsoncxx::builder::stream::open_document;
using bsoncxx::document::value;
using bsoncxx::document::view;
using bsoncxx::document::element;

class MongoGroupFunctions
{
	OT_DECL_STATICONLY(MongoGroupFunctions)
public:
	static void createGroup(std::string groupName, User& creatingUser, mongocxx::client& adminClient);

	static std::vector<Group> getAllUserGroups(const User& loggedInUser, mongocxx::client& userClient);

	static std::vector<Group> getAllGroups(User& loggedInUser, mongocxx::client& userClient);

	static size_t getAllGroupCount(User& loggedInUser, mongocxx::client& userClient);

	static Group getGroupDataByName(std::string groupName, mongocxx::client& userClient);

	static Group getGroupDataById(std::string _groupId, mongocxx::client& _userClient);

	static Group getGroupDataFromGroupView(const bsoncxx::v_noabi::document::view& groupView, mongocxx::client& _adminClient);

	static bool changeGroupName(std::string oldName, std::string newName, mongocxx::client& adminClient);

	static bool changeGroupOwner(Group& group, User& oldUser, User& newUser, mongocxx::client& adminClient);

	static bool addUserToGroup(User& loggedInUser, std::string groupName, mongocxx::client& adminClient);

	static bool removeUserFromGroup(User& loggedInUser, std::string groupName, mongocxx::client& adminClient);

	static bool removeGroup(Group& groupToBeRemoved, mongocxx::client& adminClient);

	static std::string groupToJson(Group& grp);

	static std::string groupsToJson(std::vector<Group>& groups);

	static std::string generateUniqueGroupName(mongocxx::client& adminClient);

};