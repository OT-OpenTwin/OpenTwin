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

#include "MongoConstants.h"
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


namespace MongoRoleFunctions
{
	// Function that creates the Inital Role. To be used only once!
	bool createInitialProjectRole(mongocxx::client& adminClient);
	bool createInitialProjectDbListCollectionsRole(mongocxx::client& adminClient);

	bool createInitialUserRole(mongocxx::client& adminClient);
	bool createInitialUserDbListCollectionsRole(mongocxx::client& adminClient);

	bool createInitialGroupRole(mongocxx::client& adminClient);

	bool createInitialProjectTemplatesRole(mongocxx::client& adminClient);
	bool createInitialProjectsLargeDataRole(mongocxx::client& adminClient);
	bool createInitialSystemDbRole(mongocxx::client& adminClient);
	bool createInitialSettingsDbRole(mongocxx::client& adminClient);
	bool createInitialLibrariesDbRole(mongocxx::client& adminClient);

	bool createAllAccessibleRole(std::string dbName, std::string roleName, mongocxx::client& adminClient);


	// When a new project (Collection) is created, a role for it must be created, else, nobody can access it.
	void createProjectRole(std::string roleName, std::string collectionName, mongocxx::client& adminClient);

	// When a new user is created, a role for it must be created, else, nobody can access it.
	void createUserRole(std::string roleName, mongocxx::client& adminClient);
	void addRoleToUserRole(std::string roleName, User& user, mongocxx::client& adminClient);
	void addRoleToUserRole(std::string roleName, std::string userRoleName, mongocxx::client& adminClient);
	void removeRoleFromUserRole(std::string roleName, User& user, mongocxx::client& adminClient);

	// Creates a group role which has no privileges at all
	void createGroupRole(std::string roleName, mongocxx::client& adminClient);

	// Adds a role to an already existing group role to inherit 
	void addRoleToGroupRole(std::string roleName, Group& group, mongocxx::client& adminClient);

	void removeRoleFromGroupRole(std::string roleName, Group& group, mongocxx::client& adminClient);

	void addRoleToUser(std::string roleName, std::string username, mongocxx::client& adminClient);

	void removeRoleFromUser(std::string roleName, std::string username, mongocxx::client& adminClient);


	void removeRole(std::string roleName, mongocxx::client& adminClient);
}