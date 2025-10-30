// @otlicense
// File: MongoRoleFunctions.cpp
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

#include "MongoRoleFunctions.h"
#include "MongoUserFunctions.h"
#include "Group.h"

namespace MongoRoleFunctions
{

	// Function that creates the Inital Role. To be used only once!
	bool createInitialProjectRole(mongocxx::client& adminClient)
	{
		/*
		{
			createRole: "projectRole",
			privileges: [
			 { resource: { db: "Projects", collection: "Projects" }, actions: [ "find", "insert", "update", "remove" ] },
			],
			roles: []
		}
		*/
		auto builder = document{};

		// This Role is to be used when the user is first created!
		value new_role_command = builder
			<< "createRole" << MongoConstants::PROJECT_CATALOG_ROLE
			<< "privileges"
			<< open_array

			<< open_document
			<< "resource"
			<< open_document
			<< "db" << MongoConstants::PROJECTS_DB
			<< "collection" << MongoConstants::PROJECT_CATALOG_COLLECTION
			<< close_document

			<< "actions"
			<< open_array
			<< "find"
			<< "insert"
			<< "update"
			<< "remove"
			<< "createIndex"
			<< "dropIndex"
			<< close_array
			<< close_document

			<< close_array
			<< "roles" << open_array << close_array // Inheriting roles, NONE
			<< finalize;

		view new_role_view = new_role_command.view();
		mongocxx::database db = adminClient.database("admin");


		value command_result = db.run_command(new_role_view);

		// Getting elements from this document:
		element el = command_result.view()["ok"];

		if (el.get_double() == 1)
		{
			return true;
		}

		return false;

	}

	bool createInitialProjectDbListCollectionsRole(mongocxx::client& adminClient)
	{
		
		auto builder = document{};

		// This Role is to be used when the user is first created!
		value new_role_command = builder
			<< "createRole" << MongoConstants::PROJECT_DB_LIST_COLLECTIONS_ROLE
			<< "privileges"
			<< open_array

			<< open_document
			<< "resource"
			<< open_document
			<< "db" << MongoConstants::PROJECTS_DB
			<< "collection" << ""
			<< close_document

			<< "actions"
			<< open_array
			<< "listIndexes"
			<< "listCollections"
			<< close_array
			<< close_document

			<< close_array
			<< "roles" << open_array << close_array // Inheriting roles, NONE
			<< finalize;

		view new_role_view = new_role_command.view();
		mongocxx::database db = adminClient.database("admin");


		value command_result = db.run_command(new_role_view);

		// Getting elements from this document:
		element el = command_result.view()["ok"];

		if (el.get_double() == 1)
		{
			return true;
		}

		return false;

	}

	bool createInitialUserRole(mongocxx::client& adminClient)
	{
		/*
		{
			createRole: "projectRole",
			privileges: [
			 { resource: { db: "Projects", collection: "Projects" }, actions: [ "find", "insert", "update", "remove" ] },
			],
			roles: []
		}
		*/
		auto builder = document{};

		// This Role is to be used when the user is first created!
		value new_role_command = builder
			<< "createRole" << MongoConstants::USER_CATALOG_ROLE
			<< "privileges"
			<< open_array

			<< open_document
			<< "resource"
			<< open_document
			<< "db" << MongoConstants::USER_DB
			<< "collection" << MongoConstants::PROJECT_CATALOG_COLLECTION
			<< close_document

			<< "actions"
			<< open_array
			<< "find"
			<< "insert"
			<< "update"
			<< "remove"
			<< "createIndex"
			<< "dropIndex"
			<< close_array
			<< close_document

			<< close_array
			<< "roles" << open_array << close_array // Inheriting roles, NONE
			<< finalize;

		view new_role_view = new_role_command.view();
		mongocxx::database db = adminClient.database("admin");


		value command_result = db.run_command(new_role_view);

		// Getting elements from this document:
		element el = command_result.view()["ok"];

		if (el.get_double() == 1)
		{
			return true;
		}

		return false;

	}

	bool createInitialUserDbListCollectionsRole(mongocxx::client& adminClient)
	{

		auto builder = document{};

		// This Role is to be used when the user is first created!
		value new_role_command = builder
			<< "createRole" << MongoConstants::USER_DB_LIST_CATALOG_ROLE
			<< "privileges"
			<< open_array

			<< open_document
			<< "resource"
			<< open_document
			<< "db" << MongoConstants::USER_DB
			<< "collection" << ""
			<< close_document

			<< "actions"
			<< open_array
			<< "listIndexes"
			<< "listCollections"
			<< close_array
			<< close_document

			<< close_array
			<< "roles" << open_array << close_array // Inheriting roles, NONE
			<< finalize;

		view new_role_view = new_role_command.view();
		mongocxx::database db = adminClient.database("admin");


		value command_result = db.run_command(new_role_view);

		// Getting elements from this document:
		element el = command_result.view()["ok"];

		if (el.get_double() == 1)
		{
			return true;
		}

		return false;

	}

	bool createInitialGroupRole(mongocxx::client& adminClient)
	{
		auto builder = document{};

		// This Role is to be used when the user is first created!
		value new_role_command = builder
			<< "createRole" << MongoConstants::GROUP_ROLE
			<< "privileges" << open_array

			<< open_document
			<< "resource"
			<< open_document
			<< "db" << MongoConstants::PROJECTS_DB
			<< "collection" << MongoConstants::GROUPS_COLLECTION
			<< close_document
			<< "actions"
			<< open_array
			<< "find"
			<< "insert"
			<< "update"
			<< "remove"
			<< "createIndex"
			<< "dropIndex"
			<< close_array
			<< close_document

			<< close_array
			<< "roles" << open_array << close_array
			<< finalize;

		view new_role_view = new_role_command.view();
		mongocxx::database db = adminClient.database(MongoConstants::ADMIN_DB);


		value command_result = db.run_command(new_role_view);

		// Getting elements from this document:
		element el = command_result.view()["ok"];

		if (el.get_double() == 1)
		{
			return true;
		}

		return false;
	}


	bool createInitialProjectTemplatesRole(mongocxx::client& adminClient)
	{
		return createAllAccessibleRole(MongoConstants::PROJECT_TEMPLATES_DB,
			MongoConstants::PROJECT_TEMPLATES_ROLE,
			adminClient);
	}

	bool createInitialProjectsLargeDataRole(mongocxx::client& adminClient)
	{
		return createAllAccessibleRole(MongoConstants::PROJECTS_LARGE_DATA_DB,
			MongoConstants::PROJECTS_LARGE_DATA_ROLE,
			adminClient);
	}

	bool createInitialSystemDbRole(mongocxx::client& adminClient)
	{
		return createAllAccessibleRole(MongoConstants::SYSTEM_DB,
			MongoConstants::SYSTEM_DB_ROLE,
			adminClient);
	}

	bool createInitialSettingsDbRole(mongocxx::client& adminClient)
	{
		return createAllAccessibleRole(MongoConstants::SETTINGS_DB,
			MongoConstants::SETTINGS_DB_ROLE,
			adminClient);
	}

	bool createInitialLibrariesDbRole(mongocxx::client& adminClient) {
		return createAllAccessibleRole(MongoConstants::LIBRARIES_DB,
			MongoConstants::LIBRARIES_DB_ROLE, adminClient);
	}

	bool createAllAccessibleRole(std::string dbName, std::string roleName, mongocxx::client& adminClient)
	{
		/*
		{
			createRole: "ProjectTemplatesRole",
			privileges: [
			 { resource: { db: "ProjectTemplates", collection: "" }, actions: [ "find", "insert", "update", "remove" ] },
			],
			roles: [
			{ role: "readWrite", db: "ProjectTemplates" }]
		}
		*/
		auto builder = document{};

		value new_role_command = builder
			<< "createRole" << roleName
			<< "privileges"
			<< open_array

			<< open_document
			<< "resource"
			<< open_document
			<< "db" << dbName
			<< "collection" << ""
			<< close_document

			<< "actions"
			<< open_array
			<< "find"
			<< "insert"
			<< "update"
			<< "remove"
			<< "listIndexes"
			<< "listCollections"
			<< "createIndex"
			<< "dropIndex"
			<< close_array
			<< close_document

			<< close_array
			<< "roles"
			<< open_array

			<< open_document
			<< "role" << "readWrite"
			<< "db" << dbName
			<< close_document

			<< close_array
			<< finalize;

		view new_role_view = new_role_command.view();
		mongocxx::database db = adminClient.database("admin");


		value command_result = db.run_command(new_role_view);

		// Getting elements from this document:
		element el = command_result.view()["ok"];

		if (el.get_double() == 1)
		{
			return true;
		}

		return false;
	}

	// When a new project (Collection) is created, a role for it must be created, else, nobody can access it.
	void createProjectRole(std::string roleName, std::string collectionName, mongocxx::client& adminClient)
	{
		auto builder = document{};

		value new_role_command = builder
			<< "createRole" << roleName
			<< "privileges"
			<< open_array

			<< open_document
			<< "resource"
			<< open_document
			<< "db" << MongoConstants::PROJECTS_DB
			<< "collection" << collectionName // Project collection Name
			<< close_document
			<< "actions"
			<< open_array
			<< "find"
			<< "insert"
			<< "update"
			<< "remove"
			<< "listIndexes"
			<< "listCollections"
			<< "createIndex"
			<< "dropIndex"
			<< close_array
			<< close_document

			<< open_document
			<< "resource"
			<< open_document
			<< "db" << MongoConstants::PROJECTS_DB
			<< "collection" << collectionName + ".files" // Project collection Name for grid fs files
			<< close_document
			<< "actions"
			<< open_array
			<< "find"
			<< "insert"
			<< "update"
			<< "remove"
			<< "listIndexes"
			<< "listCollections"
			<< "createIndex"
			<< "dropIndex"
			<< close_array
			<< close_document

			<< open_document
			<< "resource"
			<< open_document
			<< "db" << MongoConstants::PROJECTS_DB
			<< "collection" << collectionName + ".chunks" // Project collection Name for grid fs chunks
			<< close_document
			<< "actions"
			<< open_array
			<< "find"
			<< "insert"
			<< "update"
			<< "remove"
			<< "listIndexes"
			<< "listCollections"
			<< "createIndex"
			<< "dropIndex"
			<< close_array
			<< close_document

			<< open_document
			<< "resource"
			<< open_document
			<< "db" << MongoConstants::PROJECTS_DB
			<< "collection" << collectionName + ".results" // Project collection Name for results
			<< close_document
			<< "actions"
			<< open_array
			<< "find"
			<< "insert"
			<< "update"
			<< "remove"
			<< "listIndexes"
			<< "listCollections"
			<< "createIndex"
			<< "dropIndex"
			<< close_array
			<< close_document

			<< close_array
			<< "roles" << open_array << close_array
			<< finalize;

		view new_role_view = new_role_command.view();
		mongocxx::database db = adminClient.database(MongoConstants::ADMIN_DB);

		db.run_command(new_role_view);
	}


	// Creates a user role which has no privileges at all
	void createUserRole(std::string roleName, mongocxx::client& adminClient)
	{
		value new_role_command = document{}
			<< "createRole" << roleName
			<< "privileges" << open_array << close_array // No privileges
			<< "roles" << open_array << close_array // No inheriting roles for now.
			<< finalize;

		view new_role_view = new_role_command.view();
		mongocxx::database db = adminClient.database(MongoConstants::ADMIN_DB);

		db.run_command(new_role_view);
	}

	// Adds a role to an already existing user role to inherit 
	void addRoleToUserRole(std::string roleName, User& user, mongocxx::client& adminClient)
	{
		std::string userRoleName = user.roleName;

		addRoleToUserRole(roleName, userRoleName, adminClient);
	}

	void addRoleToUserRole(std::string roleName, std::string userRoleName, mongocxx::client& adminClient)
	{
		value grant_roles_command = document{}
			<< "grantRolesToRole" << userRoleName
			<< "roles"
			<< open_array
			<< open_document
			<< "role" << roleName
			<< "db" << MongoConstants::ADMIN_DB
			<< close_document
			<< close_array
			<< finalize;

		adminClient.database(MongoConstants::ADMIN_DB).run_command(grant_roles_command.view());
	}

	void removeRoleFromUserRole(std::string roleName, User& user, mongocxx::client& adminClient)
	{
		std::string userRoleName = user.roleName;

		value grant_roles_command = document{}
			<< "revokeRolesFromRole" << userRoleName
			<< "roles"
			<< open_array
			<< open_document
			<< "role" << roleName
			<< "db" << MongoConstants::ADMIN_DB
			<< close_document
			<< close_array
			<< finalize;

		adminClient.database(MongoConstants::ADMIN_DB).run_command(grant_roles_command.view());
	}

	// Creates a group role which has no privileges at all
	void createGroupRole(std::string roleName, mongocxx::client& adminClient)
	{
		value new_role_command = document{}
			<< "createRole" << roleName
			<< "privileges" << open_array << close_array // No privileges
			<< "roles" << open_array << close_array // No inheriting roles for now.
			<< finalize;

		view new_role_view = new_role_command.view();
		mongocxx::database db = adminClient.database(MongoConstants::ADMIN_DB);

		db.run_command(new_role_view);
	}


	// Adds a role to an already existing group role to inherit 
	void addRoleToGroupRole(std::string roleName, Group& group, mongocxx::client& adminClient)
	{
		std::string groupRoleName = group.roleName;

		value grant_roles_command = document{}
			<< "grantRolesToRole" << groupRoleName
			<< "roles"
			<< open_array
			<< open_document
			<< "role" << roleName
			<< "db" << MongoConstants::ADMIN_DB
			<< close_document
			<< close_array
			<< finalize;

		adminClient.database(MongoConstants::ADMIN_DB).run_command(grant_roles_command.view());

	}


	void removeRoleFromGroupRole(std::string roleName, Group& group, mongocxx::client& adminClient)
	{

		std::string groupRoleName = group.roleName;

		value grant_roles_command = document{}
			<< "revokeRolesFromRole" << groupRoleName
			<< "roles"
			<< open_array
			<< open_document
			<< "role" << roleName
			<< "db" << MongoConstants::ADMIN_DB
			<< close_document
			<< close_array
			<< finalize;

		adminClient.database(MongoConstants::ADMIN_DB).run_command(grant_roles_command.view());

	}


	void addRoleToUser(std::string roleName, std::string username, mongocxx::client& adminClient)
	{
		User user = MongoUserFunctions::getUserDataThroughUsername(username, adminClient);

		addRoleToUserRole(roleName, user, adminClient);
	}

	void removeRoleFromUser(std::string roleName, std::string username, mongocxx::client& adminClient)
	{
		User user = MongoUserFunctions::getUserDataThroughUsername(username, adminClient);

		removeRoleFromUserRole(roleName, user, adminClient);
	}

	void removeRole(std::string roleName, mongocxx::client& adminClient)
	{
		value drop_role_command = document{}
			<< "dropRole" << roleName
			<< finalize;

		view command_view = drop_role_command.view();

		value command_result = adminClient.database(MongoConstants::ADMIN_DB).run_command(command_view);
	}
}
