/*
 * Application.cpp
 *
 *  Created on: 21.05.2025
 *	Author: Sebastian Urmann
 *  Copyright (c)
 */

// Service header
#include "Application.h"

// OpenTwin header
#include "OTServiceFoundation/UserCredentials.h"
#include "Connection/ConnectionAPI.h"
#include "OTSystem/AppExitCodes.h"
#include "OTCore/Logger.h"

#include <bsoncxx/builder/stream/helpers.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/builder/stream/array.hpp>

// C++ header
#include <thread>


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

std::string Application::getMongoURL(std::string _databaseURL, std::string _dbUserName, std::string _dbPassword) {
    return DataStorageAPI::ConnectionAPI::getMongoURL(_databaseURL, _dbUserName, _dbPassword);
}

void Application::start(const char* _databasePWD) {

    OT_LOG_D("Starting initialisation");
    // Initialisation NEEDED!
    mongocxx::instance inst{};

    // First connect to database
    int status = connectToMongoDb(_databasePWD);
    OT_LOG_D("Status of connection: " + std::to_string(status));
    
    // Check if Libraries exist in db
    bool found = false;
    try {
        auto databases = m_adminClient.list_database_names();
        for (const auto& name : databases) {
            if (name == m_dataBase) {
                found = true;
                break;
            }
        }

        // If no db is found then add db and collection
        if (!found) {
            auto db = m_adminClient["Libraries"];

            bsoncxx::builder::stream::document doc;
            doc << "init" << true;

            db["Dummy"].insert_one(doc.view());
            //std::this_thread::sleep_for(std::chrono::seconds(10));
            db["Dummy"].drop();
        }
    
    }
    catch (const mongocxx::exception& e) {
        OT_LOG_E("MongoDB error: " + std::string(e.what()));
    }

    
}


Application::Application() {


}

Application::~Application() {

}
