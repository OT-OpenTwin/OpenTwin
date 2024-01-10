#pragma once

#include <iostream>
#include <cstdint>
#include <vector>
#include <bsoncxx/json.hpp>
#include <bsoncxx/types.hpp>

#include "User.h"
#include "Group.h"

class Project
{

public:
	bsoncxx::oid _id;
	std::string name;
	std::string type;
	std::string roleName;
	std::string collectionName;
	bsoncxx::types::b_date createdOn{ std::chrono::system_clock::now() };
	User creatingUser;
	std::vector<Group> groups{};
	int version;
	bsoncxx::types::b_date lastAccessedOn{ std::chrono::system_clock::now() };

};

