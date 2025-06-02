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

        for (const auto& entry : std::filesystem::directory_iterator(_folderPath)) {
            if (!entry.is_regular_file()) continue;

            std::string ext = entry.path().extension().string();
            for (auto& c : ext) c = static_cast<char>(toupper(c));
            std::string extUpper = ".SP3";
            for (auto& c : extUpper) c = static_cast<char>(toupper(c));

            if (ext != extUpper) continue;

            std::ifstream file(entry.path());
            if (!file) {
                OT_LOG_E( "Can not open file: " + entry.path().string());
                continue;
            }

            std::stringstream buffer;
            buffer << file.rdbuf();
            std::string content = buffer.str();

            std::string modelName = extractName(content);
            if (modelName == "UnknownModel") {
               OT_LOG_E(  "Modelname extraction failed: " + entry.path().string());
                continue;
            }


            std::string modelType = parseSpiceTypeToString(extractType(content));

            // Check if model exits
            auto filter = bsoncxx::builder::stream::document{} << "Name" << modelName << bsoncxx::builder::stream::finalize;
            auto existing = collection.find_one(filter.view());

            if (existing) {
                continue;
            }
            else {
                // Add new
                bsoncxx::document::value doc = bsoncxx::builder::stream::document{}
                    << "Name" << modelName
                    << "Type" << modelType
                    << "content" << content
                    << "filename" << entry.path().filename().string()
                    << bsoncxx::builder::stream::finalize;

                collection.insert_one(doc.view());
                OT_LOG_D("Added model: " + modelName);
            }
        }
    }
    catch (const mongocxx::exception& e) {
       OT_LOG_E("MongoDB Error: " + std::string(e.what()));
    }
}

std::string Application::readFile(const std::filesystem::path& _filePath) {
    std::ifstream file(_filePath);
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

std::string Application::extractType(const std::string& content) {
    std::istringstream stream(content);
    std::string line;
    while (std::getline(stream, line)) {
        if (line.find(".MODEL") != std::string::npos) {
            std::istringstream ls(line);
            std::string dot_model, name, type;
            ls >> dot_model >> name >> type;
            return type;
        }
    }
    return "Unknown";
}

std::string Application::extractName(const std::string& content) {
    std::istringstream stream(content);
    std::string line;

    while (std::getline(stream, line)) {
        // Erase spaces
        line.erase(0, line.find_first_not_of(" \t"));

        // Ignore comments and empty lines
        if (line.empty() || line[0] == '*') continue;

        // Extract modelname out of .Model
        if (line.find(".MODEL") == 0 || line.find(".model") == 0) {
            std::istringstream ls(line);
            std::string dot_model, name;
            ls >> dot_model >> name;
            return name;
        }

        // Extract modelname out of .SUBCKT
        if (line.find(".SUBCKT") == 0 || line.find(".subckt") == 0) {
            std::istringstream ls(line);
            std::string dot_subckt, name;
            ls >> dot_subckt >> name;
            return name;
        }
    }

    return "UnknownModel";
}

std::string Application::parseSpiceTypeToString(const std::string& _type) {
    std::string output = _type;
    std::transform(output.begin(), output.end(), output.begin(), ::toupper);

    if (output == "D") return "Diode";

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
    bool found = false;
    try {
        auto databases = m_adminClient.list_database_names();
        for (const auto& name : databases) {
            if (name == m_dataBase) {
                found = true;
                break;
            }
        }
        syncAndUpdateLocalModelsWithDB(m_folderPath);
    }
    catch (const mongocxx::exception& e) {
        OT_LOG_E("MongoDB error: " + std::string(e.what()));
    } 
}


Application::Application() {


}

Application::~Application() {

}
