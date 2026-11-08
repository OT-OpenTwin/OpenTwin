// @otlicense
// File: MongoRoleFunctions.h
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
#include "Management/Group.h"
#include "MongoDB/MongoConstants.h"

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
#include <set>
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

class MongoRoleFunctions
{
	OT_DECL_STATICONLY(MongoRoleFunctions)
public:
	// Function that creates the Inital Role. To be used only once!
	static bool createInitialProjectRole(mongocxx::client& adminClient);
	static bool createInitialProjectDbListCollectionsRole(mongocxx::client& adminClient);

	static bool createUserPermissionsRole(mongocxx::client& adminClient);
	static bool createInitialUserRole(mongocxx::client& adminClient);
	static bool createInitialUserDbListCollectionsRole(mongocxx::client& adminClient);

	static bool createInitialGroupRole(mongocxx::client& adminClient);

	static bool createInitialProjectTemplatesRole(mongocxx::client& adminClient);
	static bool createInitialProjectsLargeDataRole(mongocxx::client& adminClient);
	static bool createInitialSystemDbRole(mongocxx::client& adminClient);
	static bool createInitialSettingsDbRole(mongocxx::client& adminClient);
	static bool createInitialLibrariesDbRole(mongocxx::client& adminClient);

	static bool createAllAccessibleRole(std::string dbName, std::string roleName, mongocxx::client& adminClient);


	// When a new project (Collection) is created, a role for it must be created, else, nobody can access it.
	static void createProjectRole(std::string roleName, std::string collectionName, mongocxx::client& adminClient);

	// When a new user is created, a role for it must be created, else, nobody can access it.
	static void createUserRole(std::string roleName, mongocxx::client& adminClient);
	static void addRoleToUserRole(std::string roleName, User& user, mongocxx::client& adminClient);
	static void addRoleToUserRole(std::string roleName, std::string userRoleName, mongocxx::client& adminClient);
	static void removeRoleFromUserRole(std::string roleName, User& user, mongocxx::client& adminClient);

	// Creates a group role which has no privileges at all
	static void createGroupRole(std::string roleName, mongocxx::client& adminClient);

	// Adds a role to an already existing group role to inherit 
	static void addRoleToGroupRole(std::string roleName, Group& group, mongocxx::client& adminClient);

	static void removeRoleFromGroupRole(std::string roleName, Group& group, mongocxx::client& adminClient);

	static void addRoleToUser(std::string roleName, std::string username, mongocxx::client& adminClient);

	static void removeRoleFromUser(std::string roleName, std::string username, mongocxx::client& adminClient);

	static void removeRole(std::string roleName, mongocxx::client& adminClient);

	static void getListOfAllUserRoles(std::set<std::string>& userRoles, mongocxx::client& adminClient);
	static void revokeRoleFromRoles(const std::set<std::string>& roleNames, const std::string& roleName, mongocxx::client& adminClient);
	static void grantRoleToRoles(const std::set<std::string>& roleNames, const std::string& roleName, mongocxx::client& adminClient);

};