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

// OpenTwin header
#include "OTCore/CoreTypes.h"

// MongoDB header
#include <bsoncxx/document/view.hpp>

// std header
#include <vector>
#include <cstdint>
#include <iostream>

class User
{
	OT_DECL_DEFCOPY(User)
	OT_DECL_DEFMOVE(User)
public:
	explicit User() = default;
	explicit User(const std::string& _name);
	explicit User(const bsoncxx::v_noabi::document::view& _userData);

	inline const std::string& getID() const { return m_id; };
	inline const std::string& getName() const { return m_name; };
	inline const std::string& getSettingsCollectionName() const { return m_settingsCollectionName; };
	inline const std::string& getRoleName() const { return m_roleName; };

private:
	std::string m_id;
	std::string m_name;
	std::string m_settingsCollectionName;
	std::string m_roleName;
};
