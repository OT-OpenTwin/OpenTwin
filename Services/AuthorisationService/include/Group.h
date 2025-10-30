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

#include <iostream>
#include <cstdint>
#include <vector>
#include <list>
#include <bsoncxx/json.hpp>
#include <bsoncxx/types.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/stdx.hpp>
#include <mongocxx/uri.hpp>
#include <mongocxx/instance.hpp>
#include <bsoncxx/builder/stream/helpers.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/builder/stream/array.hpp>

#include "User.h"

class Group
{
public:
//	bsoncxx::document::value document_value{ bsoncxx::builder::basic::document{}.extract() };

//	bsoncxx::oid _id;
	std::string id;
	std::string name;
	std::string roleName;
	std::string ownerUserId;
	std::vector<User> users{};
	std::list<std::string> userNames{};

	std::string ownerUsername;
};

