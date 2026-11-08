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

// Auth header
#include "MongoDB/MongoGroupFunctions.h"
#include "MongoDB/MongoRoleFunctions.h"
#include "MongoDB/MongoConstants.h"
#include "MongoDB/MongoUserFunctions.h"

// OpenTwin header
#include "OTCore/JSON/JSON.h"

// std header
#include <sstream>

void MongoGroupFunctions::createGroup(std::string groupName, User& creatingUser, mongocxx::client& adminClient)
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
	MongoRoleFunctions::addRoleToUser(groupRoleName, creatingUser.getName(), adminClient);

	auto builder = document{};

	value group_value = builder
		<< "group_id" << groupID
		<< "group_name" << groupName
		<< "group_role_name" << groupRoleName
		<< "users"
		<< open_array
		<< creatingUser.getID()
		<< close_array
		<< "owner_user_id" << creatingUser.getID()
		<< finalize;

	groupsCollection.insert_one(group_value.view());
}

std::string MongoGroupFunctions::generateUniqueGroupName(mongocxx::client& adminClient)
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

std::vector<Group> MongoGroupFunctions::getAllUserGroups(const User& loggedInUser, mongocxx::client& adminClient)
{
	mongocxx::database secondaryDb = adminClient.database(MongoConstants::PROJECTS_DB);
	mongocxx::collection groupsCollection = secondaryDb.collection(MongoConstants::GROUPS_COLLECTION);

	value filter = document{}
		<< "users" << loggedInUser.getID()
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

std::vector<Group> MongoGroupFunctions::getAllGroups(User& loggedInUser, mongocxx::client& adminClient)
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

size_t MongoGroupFunctions::getAllGroupCount(User& loggedInUser, mongocxx::client& adminClient)
{
	mongocxx::database secondaryDb = adminClient.database(MongoConstants::PROJECTS_DB);
	mongocxx::collection groupsCollection = secondaryDb.collection(MongoConstants::GROUPS_COLLECTION);

	size_t count = groupsCollection.count_documents({});

	return count;
}

Group MongoGroupFunctions::getGroupDataByName(std::string groupName, mongocxx::client& adminClient)
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

	const auto& groupValue = groupOptional.value();

	return getGroupDataFromGroupView(groupValue.view(), adminClient);
}

Group MongoGroupFunctions::getGroupDataById(std::string _groupId, mongocxx::client& _adminClient)
{
	mongocxx::database secondaryDb = _adminClient.database(MongoConstants::PROJECTS_DB);
	mongocxx::collection groupsCollection = secondaryDb.collection(MongoConstants::GROUPS_COLLECTION);

	value find_group_query = document{}
		<< "group_id" << _groupId
		<< finalize;

	auto groupOptional = groupsCollection.find_one(find_group_query.view());

	if (!groupOptional.has_value())
	{
		throw std::runtime_error("Group not found");
	}

	const auto& groupValue = groupOptional.value();

	return getGroupDataFromGroupView(groupValue.view(), _adminClient);
}

Group MongoGroupFunctions::getGroupDataFromGroupView(const bsoncxx::v_noabi::document::view& _groupView, mongocxx::client& _adminClient)
{
	std::string groupOwnerUserId(_groupView["owner_user_id"].get_utf8().value.data());
	User ownerUser = MongoUserFunctions::getUserDataThroughId(groupOwnerUserId, _adminClient);

	std::string groupId(_groupView["group_id"].get_utf8().value.data());
	std::string groupName(_groupView["group_name"].get_utf8().value.data());
	std::string groupRoleName(_groupView["group_role_name"].get_utf8().value.data());

	std::string groupOwnerUserName = ownerUser.getName();

	Group gr(
		std::move(groupId),
		std::move(groupName),
		std::move(groupRoleName),
		std::move(groupOwnerUserId),
		std::move(groupOwnerUserName)
	);

	auto userIdArr = _groupView["users"].get_array().value;

	for (const auto& idEl : userIdArr)
	{
		std::string currentId(idEl.get_utf8().value.data());

		User currentUser = MongoUserFunctions::getUserDataThroughId(currentId, _adminClient);
		gr.addUser(std::move(currentUser));
	}

	return gr;
}

bool MongoGroupFunctions::changeGroupName(std::string oldName, std::string newName, mongocxx::client& adminClient)
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

bool MongoGroupFunctions::changeGroupOwner(Group& group, User& oldOwner, User& newOwner, mongocxx::client& adminClient)
{
	mongocxx::database secondaryDb = adminClient.database(MongoConstants::PROJECTS_DB);
	mongocxx::collection groupsCollection = secondaryDb.collection(MongoConstants::GROUPS_COLLECTION);

	value filter = document{}
		<< "group_id" << group.getID()
		<< finalize;

	value update = document{}
		<< "$set"
		<< open_document
		<< "owner_user_id" << newOwner.getID()
		<< close_document
		<< finalize;

	auto result = groupsCollection.update_one(filter.view(), update.view());
	if (!result.has_value())
	{
		throw std::exception("Changing group owner failed.");
	}
	int32_t matchedCount = result.value().matched_count();
	int32_t modifiedCount = result.value().modified_count();

	bool isUserAlreadyInArray = (std::find(group.getUserNames().begin(), group.getUserNames().end(), newOwner.getName()) != group.getUserNames().end());

	if (!isUserAlreadyInArray)
	{
		value insertUserInArrayUpdate = document{}
			<< "$push"
			<< open_document
			<< "users" << newOwner.getID()
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
		MongoRoleFunctions::addRoleToUser(group.getRoleName(), newOwner.getName(), adminClient);
		return true;
	}

	return false;
}

bool MongoGroupFunctions::addUserToGroup(User& userToBeAdded, std::string groupName, mongocxx::client& adminClient)
{
	mongocxx::database secondaryDb = adminClient.database(MongoConstants::PROJECTS_DB);
	mongocxx::collection groupsCollection = secondaryDb.collection(MongoConstants::GROUPS_COLLECTION);

	auto group = getGroupDataByName(groupName, adminClient);

	// Very important, we add the user to the Group document and we add the Group's Role to the user also !!

	std::string groupRoleName = group.getRoleName();

	MongoRoleFunctions::addRoleToUser(groupRoleName, userToBeAdded.getName(), adminClient);

	value add_user_to_group_query = document{}
		<< "$push"
		<< open_document
		<< "users" << userToBeAdded.getID()
		<< close_document
		<< finalize;

	value find_group_query = document{}
		<< "group_name" << groupName
		<< finalize;


	const auto& userNames = group.getUserNames();
	bool isUserAlreadyInArray = (std::find(userNames.begin(), userNames.end(), userToBeAdded.getName()) != userNames.end());

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

bool MongoGroupFunctions::removeUserFromGroup(User& userToBeRemoved, std::string groupName, mongocxx::client& adminClient)
{
	mongocxx::database secondaryDb = adminClient.database(MongoConstants::PROJECTS_DB);
	mongocxx::collection groupsCollection = secondaryDb.collection(MongoConstants::GROUPS_COLLECTION);

	auto group = getGroupDataByName(groupName, adminClient);

	// WHAT HAPPENS WHEN THE USER IS THE OWNER???

	// Very important, we add the user to the Group document and we add the Group's Role to the user also !!

	std::string groupRoleName = group.getRoleName();
	MongoRoleFunctions::removeRoleFromUser(groupRoleName, userToBeRemoved.getName(), adminClient);

	value remove_user_from_group_query = document{}
		<< "$pull"
		<< open_document
		<< "users" << userToBeRemoved.getID()
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

bool MongoGroupFunctions::removeGroup(Group& groupToBeRemoved, mongocxx::client& adminClient)
{
	mongocxx::database secondaryDb = adminClient.database(MongoConstants::PROJECTS_DB);
	mongocxx::collection groupsCollection = secondaryDb.collection(MongoConstants::GROUPS_COLLECTION);

	value query = document{}
		<< "group_id" << groupToBeRemoved.getID()
		<< finalize;

	auto res = groupsCollection.delete_one(query.view());

	if (!res.has_value())
	{
		throw std::exception("Remove group failed.");
	}
	std::string groupRoleName = groupToBeRemoved.getRoleName();
	MongoRoleFunctions::removeRole(groupRoleName, adminClient);

	return res.value().deleted_count() == 1;
}

std::string MongoGroupFunctions::groupToJson(Group& gr)
{
	ot::JsonDocument json;
	json.AddMember("groupName", ot::JsonString(gr.getName(), json.GetAllocator()), json.GetAllocator());
	json.AddMember("ownerUsername", ot::JsonString(gr.getOwnerUsername(), json.GetAllocator()), json.GetAllocator());
	json.AddMember("users", ot::JsonArray(gr.getUserNames(), json.GetAllocator()), json.GetAllocator());

	return json.toJson();
}

std::string MongoGroupFunctions::groupsToJson(std::vector<Group>& groups)
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
