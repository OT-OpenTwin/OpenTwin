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

#include <mongocxx/exception/operation_exception.hpp>

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

	bool createUserPermissionsRole(mongocxx::client& adminClient)
	{
		using bsoncxx::builder::basic::array;

		bsoncxx::builder::basic::array roles;
		roles.append(MongoConstants::PROJECT_CATALOG_ROLE);
		roles.append(MongoConstants::PROJECT_DB_LIST_COLLECTIONS_ROLE);
		roles.append(MongoConstants::GROUP_ROLE);
		roles.append(MongoConstants::PROJECT_TEMPLATES_ROLE);
		roles.append(MongoConstants::PROJECTS_LARGE_DATA_ROLE);
		roles.append(MongoConstants::SYSTEM_DB_ROLE);
		roles.append(MongoConstants::SETTINGS_DB_ROLE);
		roles.append(MongoConstants::LIBRARIES_DB_ROLE);

		bsoncxx::array::value userPermissionRoles = roles.extract();

		auto db = adminClient.database(MongoConstants::ADMIN_DB);

		try {
			value createCommand = document{}
				<< "createRole" << MongoConstants::USER_PERMISSIONS_ROLE
				<< "privileges" << open_array << close_array
				<< "roles" << userPermissionRoles.view()
				<< finalize;

			auto result = db.run_command(createCommand.view());
			return result.view()["ok"].get_double().value == 1.0;
		}
		catch (const mongocxx::operation_exception&) {
			value updateCommand = document{}
				<< "updateRole" << MongoConstants::USER_PERMISSIONS_ROLE
				<< "privileges" << open_array << close_array
				<< "roles" << userPermissionRoles.view()
				<< finalize;

			auto result = db.run_command(updateCommand.view());
			return result.view()["ok"].get_double().value == 1.0;
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

			<< open_document
			<< "resource"
			<< open_document
			<< "db" << MongoConstants::PROJECTS_DB
			<< "collection" << collectionName + ".transformed" // Project collection Name for transformed 
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

	void getListOfAllUserRoles(std::set<std::string>& userRoles, mongocxx::client& adminClient)
	{
		mongocxx::database db = adminClient.database(MongoConstants::ADMIN_DB);

		// Fetch all users of the current database
		auto usersInfo = db.run_command(
			bsoncxx::builder::stream::document{}
			<< "usersInfo" << 1
			<< bsoncxx::builder::stream::finalize
		);

		auto users = usersInfo.view()["users"].get_array().value;

		for (auto& userElement : users)
		{
			auto userDoc = userElement.get_document().value;

			auto roles = userDoc["roles"].get_array().value;

			for (auto& roleElement : roles)
			{
				auto roleDoc = roleElement.get_document().value;
				std::string currentRole = std::string(roleDoc["role"].get_string().value);

				if (currentRole.size() > 5 && currentRole.substr(0, 5) == "user-")
				{
					userRoles.emplace(currentRole);
				}
			}
		}
	}

	void revokeRoleFromRoles(const std::set<std::string>& roleNames, const std::string& roleName, mongocxx::client& adminClient)
	{
		mongocxx::database db = adminClient.database(MongoConstants::ADMIN_DB);

		for (const auto& targetRole : roleNames)
		{
			// Fetch current role details
			auto roleInfo = db.run_command(
				bsoncxx::builder::stream::document{}
				<< "rolesInfo"
				<< bsoncxx::builder::stream::open_document
				<< "role" << targetRole
				<< "db" << MongoConstants::ADMIN_DB
				<< bsoncxx::builder::stream::close_document
				<< "showPrivileges" << false
				<< "showBuiltinRoles" << false
				<< bsoncxx::builder::stream::finalize
			);

			bool roleFound = false;

			auto outerRoles = roleInfo.view()["roles"].get_array().value;

			for (auto&& outerRoleElem : outerRoles) {
				auto outerRoleDoc = outerRoleElem.get_document().value;

				auto innerRolesElem = outerRoleDoc["roles"];
				if (innerRolesElem && innerRolesElem.type() == bsoncxx::type::k_array) {
					auto innerRoles = innerRolesElem.get_array().value;

					for (auto&& innerRoleElem : innerRoles) {
						auto innerRoleDoc = innerRoleElem.get_document().value;

						auto currentRoleName = innerRoleDoc["role"].get_string().value;
						auto dbName = innerRoleDoc["db"].get_string().value;

						if (currentRoleName == roleName)
						{
							roleFound = true;
							break;
						}
					}
				}
			}			
		
			// Skip if role is not assigned
			if (!roleFound)
			{
				continue;
			}

			// Revoke role
			auto revokeCmd = bsoncxx::builder::stream::document{}
				<< "revokeRolesFromRole" << targetRole
				<< "roles"
				<< bsoncxx::builder::stream::open_array
				<< bsoncxx::builder::stream::open_document
				<< "role" << roleName
				<< "db" << MongoConstants::ADMIN_DB
				<< bsoncxx::builder::stream::close_document
				<< bsoncxx::builder::stream::close_array
				<< bsoncxx::builder::stream::finalize;

			auto result = db.run_command(revokeCmd.view());

			std::cout << "Role '" << roleName
				<< "' revoked from role: " << targetRole
				<< std::endl;
 		}
	}

	void grantRoleToRoles(const std::set<std::string>& roleNames, const std::string& roleName, mongocxx::client& adminClient)
	{
		mongocxx::database db = adminClient.database(MongoConstants::ADMIN_DB);

		for (const auto& targetRole : roleNames)
		{
			// Fetch current role details
			auto roleInfo = db.run_command(
				bsoncxx::builder::stream::document{}
				<< "rolesInfo"
				<< bsoncxx::builder::stream::open_document
				<< "role" << targetRole
				<< "db" << MongoConstants::ADMIN_DB
				<< bsoncxx::builder::stream::close_document
				<< "showPrivileges" << false
				<< "showBuiltinRoles" << false
				<< bsoncxx::builder::stream::finalize
			);

			auto roles = roleInfo.view()["roles"].get_array().value;
			if (roles.begin() == roles.end())
			{
				std::cout << "Role '" << targetRole << "' not found - skipping" << std::endl;
				continue;
			}

			auto inheritedRoles = roles.begin()->get_document().value["inheritedRoles"].get_array().value;

			// Check if the role is already assigned
			auto it = std::find_if(std::begin(inheritedRoles), std::end(inheritedRoles),
				[&](const bsoncxx::array::element& roleElement)
				{
					auto roleDoc = roleElement.get_document().value;
					std::string role = std::string(roleDoc["role"].get_string().value);
					return role == roleName;
				});

			// Skip if role is already assigned
			if (it != std::end(inheritedRoles))
			{
				std::cout << "Role '" << roleName
					<< "' already assigned to role '" << targetRole
					<< "' - skipping" << std::endl;
				continue;
			}

			// Grant role
			auto grantCmd = bsoncxx::builder::stream::document{}
				<< "grantRolesToRole" << targetRole
				<< "roles"
				<< bsoncxx::builder::stream::open_array
				<< bsoncxx::builder::stream::open_document
				<< "role" << roleName
				<< "db" << MongoConstants::ADMIN_DB
				<< bsoncxx::builder::stream::close_document
				<< bsoncxx::builder::stream::close_array
				<< bsoncxx::builder::stream::finalize;

			db.run_command(grantCmd.view());

			std::cout << "Role '" << roleName
				<< "' granted to role: " << targetRole
				<< std::endl;
		}
	}
}
