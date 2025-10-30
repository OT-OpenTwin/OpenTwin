// @otlicense
// File: ConnectionAPI.h
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
#include <string>

#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>

#include "ThreadLocalClientInitialiser.h"

namespace DataStorageAPI
{
	void callMe(void);

	class __declspec(dllexport) ConnectionAPI
	{
	public:
		static ConnectionAPI& getInstance();

		//! @brief Needs to be called before anything else is done with this class.
		//! @param serverURL 
		//! @param userName 
		//! @param userPassword 
		static void establishConnection(const std::string &serverURL, const std::string &userName, const std::string &userPassword);

		// This method needs to be called initially only once when the application runs.
		void setMongoInstance(std::ostream* logger);

		mongocxx::collection getCollection(std::string databaseName, std::string collectionName);
		bool checkCollectionExists(std::string databaseName, std::string collectionName);
		bool createCollection(std::string databaseName, std::string collectionName);
		bool deleteCollection(std::string databaseName, std::string collectionName);

		bool checkServerIsRunning(void);

		mongocxx::database getDatabase(std::string databaseName);
		int getSiteId() { return 1; } //Legacy and nothing realy happens with it. A ticket is in the backlog to clean that one up.
		static std::string getMongoURL(std::string databaseURL, std::string dbUsername, std::string dbPassword);
	private:
		
		static ThreadLocalClientInitialiser m_clientInitialiser;

		ConnectionAPI() = default;
		std::unique_ptr<mongocxx::instance> _mongoInstance = nullptr;
	};
};

