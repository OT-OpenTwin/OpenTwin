// @otlicense
// File: MongoGroupFunctions.cpp
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

#include "MongoGroupFunctions.h"
#include "MongoRoleFunctions.h"
#include "MongoConstants.h"
#include "MongoUserFunctions.h"
#include "OTCore/JSON.h"

#include <sstream>

namespace MongoGroupFunctions
{
	void createGroup(std::string groupName, User& creatingUser, mongocxx::client& adminClient)
	{
		// 1) Create Group Docu (Name, RoleName, users of this Group[], owner/creator (ID))

		// 2) Create group Role and appoint this role to the creating user

		// 3) The way to connect a Group and a user is by adding the user to the users array of Group. And to appoint him the GROUP ROLE!

		// Check whether the group already exists
		mongocxx::database secondaryDb = adminClient.database(MongoConstants::PROJECTS_DB);
		mongocxx::collection groupsCollection = secondaryDb.collection(MongoConstants::GROUPS_COLLECTION);

		value find_group_query = document{}
			<< "group_name" << groupName
			<< finalize;

		auto groupOptional = groupsCollection.find_one(find_group_query.view());

		if (groupOptional)
		{
			throw std::runtime_error("A group already exists with this name.");
		}

		// Now we create a unique name for the group and use this one for the creation (MongoDB will automatically assign the 
		// id to the group based on this name)

		std::string groupID = generateUniqueGroupName(adminClient);
		std::string groupRoleName = groupID + "_role";

		MongoRoleFunctions::createGroupRole(groupRoleName, adminClient);
		MongoRoleFunctions::addRoleToUser(groupRoleName, creatingUser.username, adminClient);

		auto builder = document{};

		value group_value = builder
			<< "group_id" << groupID
			<< "group_name" << groupName
			<< "group_role_name" << groupRoleName
			<< "users"
			<< open_array
			<< creatingUser.userId
			<< close_array
			<< "owner_user_id" << creatingUser.userId
			<< finalize;

		groupsCollection.insert_one(group_value.view());
	}

	std::string generateUniqueGroupName(mongocxx::client& adminClient)
	{
		std::ostringstream groupName;

		bool done = false;

		while (!done)
		{
			std::time_t currentTime = std::time(nullptr);
			struct tm currentDate;
			localtime_s(&currentDate, &currentTime);
			int random = rand() % 100;

			groupName.clear();
			groupName << "Group-" << 1900 + currentDate.tm_year << currentDate.tm_mon + 1 << currentDate.tm_mday << "-"
				<< currentDate.tm_hour << currentDate.tm_min << currentDate.tm_sec << "-" << random;

			done = true;

			mongocxx::database secondaryDb = adminClient.database(MongoConstants::PROJECTS_DB);
			mongocxx::collection groupsCollection = secondaryDb.collection(MongoConstants::GROUPS_COLLECTION);

			value find_group_query = document{}
				<< "group_name" << groupName.str()
				<< finalize;

			auto groupOptional = groupsCollection.find_one(find_group_query.view());

			if (groupOptional)
			{
				done = false;
			}
		}

		return groupName.str();
	}

	std::vector<Group> getAllUserGroups(User& loggedInUser, mongocxx::client& adminClient)
	{
		mongocxx::database secondaryDb = adminClient.database(MongoConstants::PROJECTS_DB);
		mongocxx::collection groupsCollection = secondaryDb.collection(MongoConstants::GROUPS_COLLECTION);

		value filter = document{}
			<< "users" << loggedInUser.userId
			<< finalize;

		mongocxx::cursor cursor = groupsCollection.find(filter.view());

		std::vector<Group> groups{};
		for (auto doc : cursor)
		{
			const std::string groupName(doc["group_name"].get_utf8().value.data());
			Group tmpGroup = getGroupDataByName(groupName, adminClient);
			groups.push_back(tmpGroup);
		}

		return groups;
	}

	std::vector<Group> getAllGroups(User& loggedInUser, mongocxx::client& adminClient)
	{
		mongocxx::database secondaryDb = adminClient.database(MongoConstants::PROJECTS_DB);
		mongocxx::collection groupsCollection = secondaryDb.collection(MongoConstants::GROUPS_COLLECTION);

		value filter = document{} << finalize;

		mongocxx::cursor cursor = groupsCollection.find(filter.view());

		std::vector<Group> groups{};
		for (auto doc : cursor)
		{
			const std::string groupName(doc["group_name"].get_utf8().value.data());
			Group tmpGroup = getGroupDataByName(groupName, adminClient);
			groups.push_back(tmpGroup);
		}

		return groups;
	}

	size_t getAllGroupCount(User& loggedInUser, mongocxx::client& adminClient)
	{
		mongocxx::database secondaryDb = adminClient.database(MongoConstants::PROJECTS_DB);
		mongocxx::collection groupsCollection = secondaryDb.collection(MongoConstants::GROUPS_COLLECTION);

		size_t count = groupsCollection.count_documents({});

		return count;
	}


	Group getGroupDataByName(std::string groupName, mongocxx::client& adminClient)
	{
		mongocxx::database secondaryDb = adminClient.database(MongoConstants::PROJECTS_DB);
		mongocxx::collection groupsCollection = secondaryDb.collection(MongoConstants::GROUPS_COLLECTION);

		value find_group_query = document{}
			<< "group_name" << groupName
			<< finalize;

		auto groupOptional = groupsCollection.find_one(find_group_query.view());

		if (!groupOptional.has_value())
		{
			throw std::runtime_error("Group not found");
		}

		auto groupValue = groupOptional.value();

		Group gr{};

		gr.id = std::string(groupValue.view()["group_id"].get_utf8().value.data());
		gr.name = std::string(groupValue.view()["group_name"].get_utf8().value.data());
		gr.roleName = std::string(groupValue.view()["group_role_name"].get_utf8().value.data());
		gr.ownerUserId = std::string(groupValue.view()["owner_user_id"].get_utf8().value.data());

		User ownerUser = MongoUserFunctions::getUserDataThroughId(gr.ownerUserId, adminClient);
		gr.ownerUsername = ownerUser.username;

		auto userIdArr = groupValue.view()["users"].get_array().value;

		for (const auto& idEl : userIdArr)
		{
			std::string currentId(idEl.get_utf8().value.data());

			User currentUser = MongoUserFunctions::getUserDataThroughId(currentId, adminClient);
			gr.users.push_back(currentUser);
			gr.userNames.push_back(currentUser.username);
		}

		return gr;
	}

	Group getGroupDataById(std::string groupId, mongocxx::client& adminClient)
	{
		mongocxx::database secondaryDb = adminClient.database(MongoConstants::PROJECTS_DB);
		mongocxx::collection groupsCollection = secondaryDb.collection(MongoConstants::GROUPS_COLLECTION);

		value find_group_query = document{}
			<< "group_id" << groupId
			<< finalize;

		auto groupOptional = groupsCollection.find_one(find_group_query.view());

		if (!groupOptional.has_value())
		{
			throw std::runtime_error("Group not found");
		}

		auto groupValue = groupOptional.value();

		Group gr{};

		gr.id = std::string(groupValue.view()["group_id"].get_utf8().value.data());
		gr.name = std::string(groupValue.view()["group_name"].get_utf8().value.data());
		gr.roleName = std::string(groupValue.view()["group_role_name"].get_utf8().value.data());
		gr.ownerUserId = std::string(groupValue.view()["owner_user_id"].get_utf8().value.data());

		User ownerUser = MongoUserFunctions::getUserDataThroughId(gr.ownerUserId, adminClient);
		gr.ownerUsername = ownerUser.username;

		auto userIdArr = groupValue.view()["users"].get_array().value;

		for (const auto& idEl : userIdArr)
		{
			std::string currentId(idEl.get_utf8().value.data());

			User currentUser = MongoUserFunctions::getUserDataThroughId(currentId, adminClient);
			gr.users.push_back(currentUser);
			gr.userNames.push_back(currentUser.username);
		}

		return gr;
	}

	bool changeGroupName(std::string oldName, std::string newName, mongocxx::client& adminClient)
	{
		value filter = document{}
			<< "group_name" << oldName
			<< finalize;

		value changes = document{}
			<< "$set"
			<< open_document
			<< "group_name" << newName
			<< close_document
			<< finalize;

		auto groupCollection = adminClient.database(MongoConstants::PROJECTS_DB)
			.collection(MongoConstants::GROUPS_COLLECTION);

		auto result = groupCollection.update_one(filter.view(), changes.view());
		if (!result.has_value())
		{
			throw std::exception("Group change failed.");
		}
		int32_t matchedCount = result.value().matched_count();
		int32_t modifiedCount = result.value().modified_count();

		if (matchedCount == 1 && modifiedCount == 1)
		{
			return true;
		}

		return false;
	}

	bool changeGroupOwner(Group& group, User& oldOwner, User& newOwner, mongocxx::client& adminClient)
	{
		mongocxx::database secondaryDb = adminClient.database(MongoConstants::PROJECTS_DB);
		mongocxx::collection groupsCollection = secondaryDb.collection(MongoConstants::GROUPS_COLLECTION);

		value filter = document{}
			<< "group_id" << group.id
			<< finalize;

		value update = document{}
			<< "$set"
			<< open_document
			<< "owner_user_id" << newOwner.userId
			<< close_document
			<< finalize;

		auto result = groupsCollection.update_one(filter.view(), update.view());
		if (!result.has_value())
		{
			throw std::exception("Changing group owner failed.");
		}
		int32_t matchedCount = result.value().matched_count();
		int32_t modifiedCount = result.value().modified_count();

		bool isUserAlreadyInArray = (std::find(group.userNames.begin(), group.userNames.end(), newOwner.username) != group.userNames.end());

		if (!isUserAlreadyInArray)
		{
			value insertUserInArrayUpdate = document{}
				<< "$push"
				<< open_document
				<< "users" << newOwner.userId
				<< close_document
				<< finalize;

			result = groupsCollection.update_one(filter.view(), insertUserInArrayUpdate.view());
			if (!result.has_value())
			{
				throw std::exception("Changing group owner failed.");
			}
			matchedCount += result.value().matched_count();
			modifiedCount += result.value().modified_count();
		}
		else
		{
			matchedCount++;
			modifiedCount++;
		}
		
		if (matchedCount == 2 && modifiedCount == 2)
		{
			MongoRoleFunctions::addRoleToUser(group.roleName, newOwner.username, adminClient);
			return true;
		}

		return false;
	}

	bool addUserToGroup(User& userToBeAdded, std::string groupName, mongocxx::client& adminClient)
	{
		mongocxx::database secondaryDb = adminClient.database(MongoConstants::PROJECTS_DB);
		mongocxx::collection groupsCollection = secondaryDb.collection(MongoConstants::GROUPS_COLLECTION);

		auto group = getGroupDataByName(groupName, adminClient);

		// Very important, we add the user to the Group document and we add the Group's Role to the user also !!

		std::string groupRoleName = group.roleName;

		MongoRoleFunctions::addRoleToUser(groupRoleName, userToBeAdded.username, adminClient);

		value add_user_to_group_query = document{}
			<< "$push"
			<< open_document
			<< "users" << userToBeAdded.userId
			<< close_document
			<< finalize;

		value find_group_query = document{}
			<< "group_name" << groupName
			<< finalize;


		bool isUserAlreadyInArray = (std::find(group.userNames.begin(), group.userNames.end(), userToBeAdded.username) != group.userNames.end());

		if (!isUserAlreadyInArray)
		{
			auto result = groupsCollection.update_one(find_group_query.view(), add_user_to_group_query.view());

			if (!result.has_value())
			{
				throw std::exception("Add usergroup failed.");
			}

			if (result.value().matched_count() == 1 && result.value().modified_count() == 1)
			{
				return true;
			}
		}

		return false;
	}


	bool removeUserFromGroup(User& userToBeRemoved, std::string groupName, mongocxx::client& adminClient)
	{
		mongocxx::database secondaryDb = adminClient.database(MongoConstants::PROJECTS_DB);
		mongocxx::collection groupsCollection = secondaryDb.collection(MongoConstants::GROUPS_COLLECTION);

		auto group = getGroupDataByName(groupName, adminClient);

		// WHAT HAPPENS WHEN THE USER IS THE OWNER???

		// Very important, we add the user to the Group document and we add the Group's Role to the user also !!

		std::string groupRoleName = group.roleName;
		MongoRoleFunctions::removeRoleFromUser(groupRoleName, userToBeRemoved.username, adminClient);

		value remove_user_from_group_query = document{}
			<< "$pull"
			<< open_document
			<< "users" << userToBeRemoved.userId
			<< close_document
			<< finalize;

		value find_group_query = document{}
			<< "group_name" << groupName
			<< finalize;

		auto result = groupsCollection.update_one(find_group_query.view(), remove_user_from_group_query.view());

		if (!result.has_value())
		{
			throw std::exception("Remove user from group failed.");
		}

		if (result.value().matched_count() == 1 && result.value().modified_count() == 1)
		{
			return true;
		}

		return false;

	}

	bool removeGroup(Group& groupToBeRemoved, mongocxx::client& adminClient)
	{
		mongocxx::database secondaryDb = adminClient.database(MongoConstants::PROJECTS_DB);
		mongocxx::collection groupsCollection = secondaryDb.collection(MongoConstants::GROUPS_COLLECTION);

		value query = document{}
			<< "group_id" << groupToBeRemoved.id
			<< finalize;

		auto res = groupsCollection.delete_one(query.view());

		if (!res.has_value())
		{
			throw std::exception("Remove group failed.");
		}
		std::string groupRoleName = groupToBeRemoved.roleName;
		MongoRoleFunctions::removeRole(groupRoleName, adminClient);

		return res.value().deleted_count() == 1;
	}

	std::string groupToJson(Group& gr)
	{
		ot::JsonDocument json;
		json.AddMember("groupName", ot::JsonString(gr.name, json.GetAllocator()), json.GetAllocator());
		json.AddMember("ownerUsername", ot::JsonString(gr.ownerUsername, json.GetAllocator()), json.GetAllocator());
		json.AddMember("users", ot::JsonArray(gr.userNames, json.GetAllocator()), json.GetAllocator());
		
		return json.toJson();
	}


	std::string groupsToJson(std::vector<Group>& groups)
	{
		std::list<std::string> jsonGroups{};

		for (auto group : groups)
		{
			jsonGroups.push_back(groupToJson(group));
		}

		ot::JsonDocument json;
		json.AddMember("groups", ot::JsonArray(jsonGroups, json.GetAllocator()), json.GetAllocator());
		
		return json.toJson();
	}
}