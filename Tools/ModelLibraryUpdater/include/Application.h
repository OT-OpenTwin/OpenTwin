// @otlicense
// File: Application.h
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
#include <mongocxx/stdx.hpp>
#include <mongocxx/uri.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/exception/exception.hpp>

//std Header
#include <string>
#include <thread>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <algorithm>


class Application {
public:
	static Application* getInstance();

	static void deleteInstance(void);

	// Delete Copy & Move Constructors
	Application(const Application&) = delete;
	Application& operator=(const Application&) = delete;

	void start(const char * _databasePWD);


private:
	Application();
	~Application();

	std::string getMongoURL(std::string _databaseURL, std::string _dbUserName, std::string _dbPassword);
	std::string getAdminUserName() { return "admin"; }

	int connectToMongoDb(const char* _databasePWD);
	void syncAndUpdateLocalModelsWithDB(const std::string& modelFolderPath);
	std::string m_databaseURL = "127.0.0.1:27017tls@";
	std::string m_dbPassword;
	std::string m_dbUsername;
	mongocxx::client m_adminClient;
	const std::string m_dataBase = "Libraries";
	const std::string m_collectionName = "CircuitModels";
	std::string m_databasePWD;
	const std::string m_folderPath = "C:\\Arbeit_Programmierung\\repos\\OpenTwin\\Assets\\CircuitModels";


	static Application* instance;
	

};