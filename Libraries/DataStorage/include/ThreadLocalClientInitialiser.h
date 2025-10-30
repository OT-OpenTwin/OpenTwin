// @otlicense
// File: ThreadLocalClientInitialiser.h
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
#include <mongocxx/client.hpp>

#include <string>

namespace DataStorageAPI
{
	class ThreadLocalClientInitialiser
	{
	public:
		void setCredentials(const std::string& _serverURL, const std::string& _userName, const std::string& _userPassword);

		mongocxx::client& getClient();
	private:
		std::string m_serverURL = "";
		std::string m_userName = "";
		std::string m_userPassword = "";

	};
}