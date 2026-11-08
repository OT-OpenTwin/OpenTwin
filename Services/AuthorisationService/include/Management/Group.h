// @otlicense
// File: Group.h
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
#include "User.h"

// std header
#include <list>
#include <vector>

class Group
{
	OT_DECL_DEFCOPY(Group)
	OT_DECL_DEFMOVE(Group)
public:
	explicit Group() = default;
	explicit Group(std::string&& _id, std::string&& _name, std::string&& _roleName, std::string&& _ownerUserId, std::string&& _ownerUsername)
		: m_id(std::move(_id)), m_name(std::move(_name)), m_roleName(std::move(_roleName)), m_ownerUserId(std::move(_ownerUserId)), m_ownerUsername(std::move(_ownerUsername))
	{}
	~Group() = default;

	void setID(const std::string& _id) { m_id = _id; };
	const std::string& getID() const { return m_id; };
	
	void setName(const std::string& _name) { m_name = _name; };
	const std::string& getName() const { return m_name; };

	void setRoleName(const std::string& _roleName) { m_roleName = _roleName; };
	const std::string& getRoleName() const { return m_roleName; };

	void setOwnerUserId(const std::string& _ownerUserId) { m_ownerUserId = _ownerUserId; };
	const std::string& getOwnerUserId() const { return m_ownerUserId; };

	void addUser(User&& _user) { m_users.push_back(std::move(_user)); m_userNames.push_back(m_users.back().getName()); };
	void addUser(const User& _user) { m_users.push_back(_user); m_userNames.push_back(_user.getName()); };
	const std::vector<User>& getUsers() const { return m_users; };
	const std::list<std::string>& getUserNames() const { return m_userNames; };

	void setOwnerUsername(const std::string& _username) { m_ownerUsername = _username; };
	std::string getOwnerUsername() const { return m_ownerUsername; };

private:
	std::string m_id;
	std::string m_name;
	std::string m_roleName;
	std::string m_ownerUserId;
	std::vector<User> m_users;
	std::list<std::string> m_userNames;

	std::string m_ownerUsername;
};

