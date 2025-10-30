// @otlicense
// File: Application.cpp
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

// Service header
#include "Application.h"

// OpenTwin header
#include "OTServiceFoundation/UserCredentials.h"
#include "Connection/ConnectionAPI.h"
#include "OTSystem/AppExitCodes.h"
#include "OTCore/LogDispatcher.h"


// Thirdparty header
#include <bsoncxx/builder/stream/helpers.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/builder/stream/array.hpp>
#include <bsoncxx/json.hpp>





Application* Application::instance = nullptr;

Application* Application::getInstance() {
	if (instance == nullptr) {
		instance = new Application();
	}

	return instance;
}

void Application::deleteInstance(void) {
	if (instance) {
		delete instance;
	}
	instance = nullptr;
}

int Application::connectToMongoDb(const char* _databasePWD) {
  

    m_dbUsername = getAdminUserName();
    m_databasePWD = _databasePWD;

    if (!m_databasePWD.empty()) {
        m_dbPassword = m_databasePWD;
    }
    else {
        m_dbPassword = ot::UserCredentials::encryptString("admin");
    }

    try {
        std::string uriStr = getMongoURL(m_databaseURL, m_dbUsername, ot::UserCredentials::decryptString(m_dbPassword));
        mongocxx::uri uri(uriStr);
        m_adminClient = mongocxx::client(uri);
    }
    catch (std::exception err) {
        exit(ot::AppExitCode::DataBaseConnectionFailed);
    }

    return ot::AppExitCode::Success;

}

void Application::syncAndUpdateLocalModelsWithDB(const std::string& _folderPath) {
    try {
        auto db = m_adminClient["Libraries"];
        auto collection = db["CircuitModels"];

        // First check if db exists
        bool found = false;
        auto databases = m_adminClient.list_database_names();
        for (const auto& name : databases) {
            if (name == m_dataBase) {
                found = true;
                break;
            }
        }

        if (!found) {
            OT_LOG_D("Database: " + m_dataBase + " does not exist");
        }

        // Check if collection exists
        bool colExist = false;
        auto collections = db.list_collection_names();
        for (const auto& name : collections) {
            if (name == m_collectionName) {
                colExist = true;
                break;
            }
        }

        if (!colExist) {
            OT_LOG_D("No Collection: " + m_collectionName + " exists");
        }

        //Go through local models
        for (const auto& entry : std::filesystem::directory_iterator(_folderPath)) {
            if (!entry.is_regular_file()) continue;

            std::string ext = entry.path().extension().string();

            if (ext != ".txt") {
                OT_LOG_E("Wrong file extension");
                continue;
            }

            std::ifstream file(entry.path());
            if (!file) {
                OT_LOG_E( "Can not open file: " + entry.path().string());
                continue;
            }

            std::string fileName = entry.path().filename().string();
            bsoncxx::document::value filter = bsoncxx::builder::stream::document{}
                << "Filename" << fileName
                << bsoncxx::builder::stream::finalize;

            if (found && colExist) {
                if (collection.find_one(filter.view())) {
                    OT_LOG_D("CircuitModel :" + fileName + " already exists");
                    continue;
                }
            }

            std::stringstream buffer;
            buffer << file.rdbuf();
            std::string content = buffer.str();

            // Add new
            bsoncxx::document::value doc = bsoncxx::builder::stream::document{}
                << "content" << content
                << "filename" << fileName
                << bsoncxx::builder::stream::finalize;

            collection.insert_one(doc.view());
            OT_LOG_D("Added circuit model");
            
        }
    }
    catch (const mongocxx::exception& e) {
       OT_LOG_E("MongoDB Error: " + std::string(e.what()));
    }
}

std::string Application::getMongoURL(std::string _databaseURL, std::string _dbUserName, std::string _dbPassword) {
    return DataStorageAPI::ConnectionAPI::getMongoURL(_databaseURL, _dbUserName, _dbPassword);
}

void Application::start(const char* _databasePWD) {

    OT_LOG_D("Starting initialization");
    // Initialisation NEEDED!
    mongocxx::instance inst{};

    // First connect to database
    int status = connectToMongoDb(_databasePWD);
    OT_LOG_D("Status of connection: " + std::to_string(status));
    
    // Check if Libraries exist in db
    try {
        syncAndUpdateLocalModelsWithDB(m_folderPath);
    } catch (const mongocxx::exception& e) {
        OT_LOG_E("MongoDB error: " + std::string(e.what()));
    } 
}


Application::Application() {


}

Application::~Application() {

}
