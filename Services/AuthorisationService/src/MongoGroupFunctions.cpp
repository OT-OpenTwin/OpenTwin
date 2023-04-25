
#include "MongoGroupFunctions.h"
#include "MongoRoleFunctions.h"
#include "MongoConstants.h"
#include "MongoUserFunctions.h"

#include <sstream>

/*
* The following functionalities must be imlemented:
* 1) Create group ✅
* 2) Get All User groups (Can be performed in the front end) ✅
* 3) Get group Data (can be performed in the front end) ✅
* 4) Add user to group ✅
* 5) Remove User from Group ✅
* 6) Remove group completely ✅
* 7) Change groupName  ✅
*/
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
			<< "group_name" << groupID
			<< "group_role_name" << groupRoleName
			<< "users"
			<< open_array
			<< creatingUser.getUserId()
			<< close_array
			<< "owner_user_id" << creatingUser.getUserId()
			<< finalize;

		groupsCollection.insert_one(group_value.view());

		// Finally, we rename the group to the requested name
		changeGroupName(groupID, groupName, adminClient);
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
			<< "users" << loggedInUser.getUserId()
			<< finalize;

		mongocxx::cursor cursor = groupsCollection.find(filter.view());

		std::vector<Group> groups{};
		for (auto doc : cursor)
		{
			Group tmpGroup = getGroupData(doc["group_name"].get_utf8().value.to_string(), adminClient);
			groups.push_back(std::move(tmpGroup));
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
			Group tmpGroup = getGroupData(doc["group_name"].get_utf8().value.to_string(), adminClient);
			groups.push_back(std::move(tmpGroup));
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


	Group getGroupData(std::string groupName, mongocxx::client& adminClient)
	{
		mongocxx::database secondaryDb = adminClient.database(MongoConstants::PROJECTS_DB);
		mongocxx::collection groupsCollection = secondaryDb.collection(MongoConstants::GROUPS_COLLECTION);

		value find_group_query = document{}
			<< "group_name" << groupName
			<< finalize;

		auto groupOptional = groupsCollection.find_one(find_group_query.view());

		if (!groupOptional)
		{
			throw std::runtime_error("Group not found");
		}

		auto groupValue = groupOptional.get();

		Group gr{};

		gr.setDocumentValue(groupValue);

		gr._id = groupValue.view()["_id"].get_oid().value;
		gr.name = groupValue.view()["group_name"].get_utf8().value.to_string();
		gr.roleName = groupValue.view()["group_role_name"].get_utf8().value.to_string();

		bsoncxx::types::b_binary ownerUserId = gr.getOwnerId();

		User ownerUser = MongoUserFunctions::getUserDataThroughId(ownerUserId, adminClient);
		gr.ownerUsername = ownerUser.username;

		auto userIdArr = groupValue.view()["users"].get_array().value;

		for (auto idEl : userIdArr)
		{
			bsoncxx::types::b_binary currentId = idEl.get_binary();

			User currentUser = MongoUserFunctions::getUserDataThroughId(currentId, adminClient);
			gr.users.push_back(currentUser);
			gr.userNames.push_back(currentUser.username);
		}


		return gr;
	}

	Group getGroupData(bsoncxx::oid& groupId, mongocxx::client& adminClient)
	{
		mongocxx::database secondaryDb = adminClient.database(MongoConstants::PROJECTS_DB);
		mongocxx::collection groupsCollection = secondaryDb.collection(MongoConstants::GROUPS_COLLECTION);

		value find_group_query = document{}
			<< "_id" << groupId
			<< finalize;

		auto groupOptional = groupsCollection.find_one(find_group_query.view());

		if (!groupOptional)
		{
			return Group{};
		}

		auto groupValue = groupOptional.get();

		Group gr{};

		gr.setDocumentValue(groupValue);

		gr._id = groupValue.view()["_id"].get_oid().value;
		gr.name = groupValue.view()["group_name"].get_utf8().value.to_string();
		gr.roleName = groupValue.view()["group_role_name"].get_utf8().value.to_string();

		bsoncxx::types::b_binary ownerUserId = gr.getOwnerId();

		User ownerUser = MongoUserFunctions::getUserDataThroughId(ownerUserId, adminClient);
		gr.ownerUsername = ownerUser.username;

		auto userIdArr = groupValue.view()["users"].get_array().value;

		for (auto idEl : userIdArr)
		{
			bsoncxx::types::b_binary currentId = idEl.get_binary();

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

		int32_t matchedCount = result.get().matched_count();
		int32_t modifiedCount = result.get().modified_count();

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
			<< "_id" << group._id
			<< finalize;

		value update = document{}
			<< "$set"
			<< open_document
			<< "owner_user_id" << newOwner.getUserId()
			<< close_document
			<< finalize;

		value insertUserInArrayUpdate = document{}
			<< "$push"
			<< open_document
			<< "users" << newOwner.getUserId()
			<< close_document
			<< finalize;


		auto result = groupsCollection.update_one(filter.view(), update.view());
		int32_t matchedCount = result.get().matched_count();
		int32_t modifiedCount = result.get().modified_count();


		bool isUserAlreadyInArray = (std::find(group.userNames.begin(), group.userNames.end(), newOwner.username) != group.userNames.end());

		if (!isUserAlreadyInArray)
		{
			result = groupsCollection.update_one(filter.view(), insertUserInArrayUpdate.view());

			matchedCount += result.get().matched_count();
			modifiedCount += result.get().modified_count();
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

		auto group = getGroupData(groupName, adminClient);


		// Very important, we add the user to the Group document and we add the Group's Role to the user also !!

		std::string groupRoleName = group.roleName;

		MongoRoleFunctions::addRoleToUser(groupRoleName, userToBeAdded.username, adminClient);

		value add_user_to_group_query = document{}
			<< "$push"
			<< open_document
			<< "users" << userToBeAdded.getUserId()
			<< close_document
			<< finalize;

		value find_group_query = document{}
			<< "group_name" << groupName
			<< finalize;


		bool isUserAlreadyInArray = (std::find(group.userNames.begin(), group.userNames.end(), userToBeAdded.username) != group.userNames.end());

		if (!isUserAlreadyInArray)
		{
			auto result = groupsCollection.update_one(find_group_query.view(), add_user_to_group_query.view());

			if (result.get().matched_count() == 1 && result.get().modified_count() == 1)
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

		auto group = getGroupData(groupName, adminClient);


		// WHAT HAPPENS WHEN THE USER IS THE OWNER???

		// Very important, we add the user to the Group document and we add the Group's Role to the user also !!

		std::string groupRoleName = group.roleName;
		MongoRoleFunctions::removeRoleFromUser(groupRoleName, userToBeRemoved.username, adminClient);

		value remove_user_from_group_query = document{}
			<< "$pull"
			<< open_document
			<< "users" << userToBeRemoved.getUserId()
			<< close_document
			<< finalize;

		value find_group_query = document{}
			<< "group_name" << groupName
			<< finalize;

		auto result = groupsCollection.update_one(find_group_query.view(), remove_user_from_group_query.view());

		if (result.get().matched_count() == 1 && result.get().modified_count() == 1)
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
			<< "_id" << groupToBeRemoved._id
			<< finalize;

		
		auto res = groupsCollection.delete_one(query.view());
		std::string groupRoleName = groupToBeRemoved.roleName;
		MongoRoleFunctions::removeRole(groupRoleName, adminClient);

		return res.get().deleted_count() == 1;
	}

	std::string groupToJson(Group& gr)
	{
		OT_rJSON_createDOC(json);
		ot::rJSON::add(json, "groupName", gr.name);
		ot::rJSON::add(json, "ownerUsername", gr.ownerUsername);
		ot::rJSON::add(json, "users", gr.userNames);

		return ot::rJSON::toJSON(json);
	}


	std::string groupsToJson(std::vector<Group>& groups)
	{
		std::list<std::string> jsonGroups{};

		for (auto group : groups)
		{
			jsonGroups.push_back(groupToJson(group));
		}

		OT_rJSON_createDOC(json);
		ot::rJSON::add(json, "groups", jsonGroups);
		return ot::rJSON::toJSON(json);
	}
}