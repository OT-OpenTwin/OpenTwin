// @otlicense
// File: User.h
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
#include <bsoncxx/document/view.hpp>


class User
{

public:
	User(const bsoncxx::v_noabi::document::view& _userData)
	{
		username = std::string(_userData["user_name"].get_utf8().value.data());
		userId = std::string(_userData["user_id"].get_utf8().value.data());
		roleName = std::string(_userData["user_role_name"].get_utf8().value.data());
		settingsCollectionName = std::string(_userData["settings_collection_name"].get_utf8().value.data());
	}
	User() = default;

	std::string userId = "";
	std::string username = "";
	std::string settingsCollectionName = "";
	std::string roleName = "";
};
