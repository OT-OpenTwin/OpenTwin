// @otlicense
// File: UserAPI.cpp
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
#include "stdafx.h"
#include "UserAPI.h"

#include <iostream>
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>

std::map<int, std::string> users = {
	{1, "User1"}, {2, "User2"}, {3, "User3"}
};

std::map<int, std::string> UserAPI::getAllUsers()
{
	try
	{
		mongocxx::client conn{ mongocxx::uri{} };
		bsoncxx::builder::stream::document document{};

		auto collection = conn["Tutorial"]["Users"];
		auto cursor = collection.find({});

		for (auto&& doc : cursor) {
			auto data = bsoncxx::to_json(doc);
			std::cout << data << std::endl;
		}
	}
	catch (std::exception& e)
	{
		std::cout << e.what();
	}
	return users;
}

std::string UserAPI::getUser(int userId)
{
	auto user = users.find(userId);
	if (user == users.end())
	{
		return "User Not found!";
	}
	return user->second;
}

std::map<int, std::string> UserAPI::saveUser(std::string userName)
{
	auto lastUser = users.rbegin();
	int newUserId = lastUser->first + 1;
	users.insert(std::make_pair(newUserId, userName));
	return users;
}

bool UserAPI::updateUser(int userId, std::string newName)
{
	auto user = users.find(userId);
	if (user == users.end())
	{
		return false;
	}

	user->second = newName;
	return true;
}

std::map<int, std::string> UserAPI::deleteUser(int userId)
{
	auto a = users.erase(userId);
	return users;
}
