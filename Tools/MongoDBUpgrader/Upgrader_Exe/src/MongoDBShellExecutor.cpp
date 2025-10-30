// @otlicense
// File: MongoDBShellExecutor.cpp
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

#include "MongoDBShellExecutor.h"
#include "Logger.h"

#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/json.hpp>
#include <rapidjson/document.h>
#include "OTSystem/UrlEncoding.h"


MongoDBShellExecutor::MongoDBShellExecutor(const MongoDBSettings& _settings)
{
    Logger::INSTANCE().write("Starting shell executor.");
    Logger::INSTANCE().write("Admin psw: " + _settings.m_adminPsw);
    const std::string connectionString = "mongodb://"+ot::url::urlEncode("admin") + ":" + ot::url::urlEncode(_settings.m_adminPsw) + "@" + _settings.m_bindIP + ":" + _settings.m_port + "/?tls=true&tlsCertificateKeyFile=" + _settings.m_certKeyFile;
    Logger::INSTANCE().write("Connecting shell with MongoDB: " + connectionString);

    //const std::string temp = "mongodb://admin:admin@127.0.0.1:27017/?tls=true&tlsCertificateKeyFile=C:\\Users\\JWagner\\OpenTwin_Cert\\certificateKeyFile.pem";
    mongocxx::uri uri(connectionString);
    try
    {
        m_client = mongocxx::client(uri);
    }
    catch (const std::exception& e)
    {
        throw std::exception(("Establishing connection with MongoDB at " + _settings.m_bindIP + ":" + _settings.m_port + " failed, due to the exception: " + e.what()).c_str());
    }
}

int MongoDBShellExecutor::getFeatureCompatibilityVersion()
{
    auto admin_db = m_client["admin"];

    bsoncxx::builder::stream::document command_builder;
    command_builder << "getParameter" << 1;
    command_builder << "featureCompatibilityVersion" << 1;
    auto result = admin_db.run_command(command_builder.view());
    std::string resultStr = bsoncxx::to_json(result);
    rapidjson::Document resultJson(rapidjson::kObjectType);
    resultJson.Parse(resultStr.c_str());
    Logger::INSTANCE().write("CSV request response: " + resultStr + "\n");
    //{ "featureCompatibilityVersion" : { "version" : "7.0" }, "ok" : 1.0 }
    auto subDoc = resultJson["featureCompatibilityVersion"].GetObject();
    std::string versionString = subDoc["version"].GetString();
    size_t pos = versionString.find_first_of('.') - 1;
    pos = pos == 0 ? 1 : pos;
    std::string majorVersion = versionString.substr(0, pos);
    int versionInt = std::stoi(majorVersion);
    return versionInt;
}

void MongoDBShellExecutor::setFeatureCompatibilityVersion(const std::string& version)
{
    auto admin_db = m_client["admin"];

    const std::string majorAndMinorVersion = version.substr(0, version.find_last_of('.'));

    // Create the command document
    bsoncxx::builder::stream::document command_builder;
    command_builder << "setFeatureCompatibilityVersion" << majorAndMinorVersion;

    if (majorAndMinorVersion == "7.0")
    {
        command_builder << "confirm" << true;
    }

    // Run the command
    Logger::INSTANCE().write("Running MongoDB command " + bsoncxx::to_json(command_builder.view())+"\n");
    
    auto result = admin_db.run_command(command_builder.view());

    auto resultStr = bsoncxx::to_json(result);
    rapidjson::Document resultJson(rapidjson::kObjectType);
    resultJson.Parse(resultStr.c_str());
    assert(resultJson.HasMember("ok"));
    if (resultJson["ok"] != 1)
    {
        throw std::exception(("Failed to upgrade the database to version " + version + ". Database response: " + resultStr).c_str());
    }

    //Double check the version upgrade
    command_builder.clear();
    //{'getParameter': 1, 'featureCompatibilityVersion': 1}
    command_builder << "getParameter" << 1;
    command_builder << "featureCompatibilityVersion" << 1;
    result = admin_db.run_command(command_builder.view());
    resultStr = bsoncxx::to_json(result);
    Logger::INSTANCE().write("Answer to requested featureCompatibilityVersion: " + resultStr + "\n");
    resultJson.Parse(resultStr.c_str());
    assert(resultJson.HasMember("ok"));
    if (resultJson["ok"] == 1)
    {
        std::string newVersion = resultJson["featureCompatibilityVersion"].GetObject()["version"].GetString();
        if (newVersion != majorAndMinorVersion)
        {
            throw std::exception(("Failed to upgrade the database to version. Current featureCompatibilityVersion is " + newVersion).c_str());
        }
        else
        {
            Logger::INSTANCE().write("New feature compatibility version: " + newVersion + "\n");
        }
    }
    else
    {
        throw std::exception(("Failed to upgrade the database to version " + version + ". Database response: " + resultStr).c_str());
    }
    
    Logger::INSTANCE().write("Successfully upgraded the database to version: " + version + "\n");
}

void MongoDBShellExecutor::shutdownDatabase()
{
    auto admin_db = m_client["admin"];
    bsoncxx::builder::stream::document command_builder;
    command_builder << "shutdown" << 1
        << "force" << false;
          
    bsoncxx::v_noabi::document::view_or_value result;
    try
    {
        result = admin_db.run_command(command_builder.view());
    }
    catch (std::exception& e)
    {
        //Closing the application will cause a timeout exception
        std::string tem = e.what();
    }
    //std::string resultStr = bsoncxx::to_json(result);
    //std::cout << "Answer to requested shutdown of database: " << resultStr << "\n";
    //rapidjson::Document resultJson(rapidjson::kObjectType);
    //resultJson.Parse(resultStr.c_str());
    //assert(resultJson.HasMember("ok"));
    //if (resultJson["ok"] == 1)
    //{
    //    std::cout << "Database was shut down.\n";
    //}
    //else
    //{
    //    std::cout << "Failed to shut down database. Attempting to terminate process ...\n";
    //}
}
