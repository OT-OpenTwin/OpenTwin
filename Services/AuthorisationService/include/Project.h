#pragma once

#include <iostream>
#include <cstdint>
#include <vector>
#include <bsoncxx/json.hpp>
#include <bsoncxx/types.hpp>

#include "User.h"
#include "Group.h"
#include "MongoUserFunctions.h"
#include "MongoGroupFunctions.h"

class Project
{

public:
	Project(const bsoncxx::v_noabi::document::view& _view, mongocxx::client& _userClient)
	{
		_id = _view["_id"].get_oid().value;
		name = std::string(_view["project_name"].get_utf8().value.data());
		
		roleName = std::string(_view["project_role_name"].get_utf8().value.data());
		collectionName = std::string(_view["collection_name"].get_utf8().value.data());
		createdOn = _view["created_on"].get_date();
		lastAccessedOn = _view["last_accessed_on"].get_date();

		std::string userId = std::string(_view["created_by"].get_utf8().value.data());

		creatingUser = MongoUserFunctions::getUserDataThroughId(userId, _userClient);

		auto groupsArr = _view["groups"].get_array().value;

		for (const auto& groupId : groupsArr)
		{
			std::string currId = std::string(groupId.get_utf8().value.data());

			Group currentGroup = MongoGroupFunctions::getGroupDataById(currId, _userClient);
			groups.push_back(currentGroup);
		}
	}
	Project() = default;

	bsoncxx::oid _id;
	std::string name = "";
	std::string type = "";
	std::string roleName ="";
	std::string collectionName = "";
	bsoncxx::types::b_date createdOn{ std::chrono::system_clock::now() };
	User creatingUser;
	std::vector<Group> groups{};
	int version = -1;
	bsoncxx::types::b_date lastAccessedOn{ std::chrono::system_clock::now() };


};

