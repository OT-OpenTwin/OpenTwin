// @otlicense

#include "MongoDBUpgrader.h"
#include "SharedResources.h"
#include "MongoDBServerRunner.h"
#include "MongoDBShellExecutor.h"
#include "Logger.h"
#include "FileWriter.h"

#include <cassert>
#include <iostream>
#include <boost/process/v1/system.hpp>


MongoDBUpgrader::MongoDBUpgrader(const MongoDBSettings& _settings, const std::string& _tempCfgPath)
    : m_settings(_settings), m_cfgPath(_tempCfgPath)
{}

int MongoDBUpgrader::checkForFeatureCompatibilityVersion(int _startVersion)
{
    int foundFCV = 0;

    Logger::INSTANCE().write("Run mongoDB server with settings: " + m_settings.toString());

    for (int version = _startVersion; version <= maxVersion; version++)
    {
        std::string serverVersion("");
        if (version == 4)
        {
            serverVersion = CurrentPaths::INSTANCE().getMongoVersion4_4();
        }
        else if (version == 5)
        {
            serverVersion = CurrentPaths::INSTANCE().getMongoVersion5();
        }
        else if (version == 6)
        {
            serverVersion = CurrentPaths::INSTANCE().getMongoVersion6();
        }
        else if (version == 7)
        {
            serverVersion = CurrentPaths::INSTANCE().getMongoVersion7();
        }
        else
        {
            assert(maxVersion == 7);
        }
        const std::string mongoServerPath = CurrentPaths::INSTANCE().getMongoServerCollectionDirectory() + "\\"+ serverVersion + "\\mongod.exe";
        std::unique_ptr<MongoDBServerRunner> server;
        try
        {
            server = std::make_unique<MongoDBServerRunner>(mongoServerPath, m_cfgPath);
            try
            {
                MongoDBShellExecutor mongoOperator(m_settings);
                foundFCV = mongoOperator.getFeatureCompatibilityVersion();
                Logger::INSTANCE().write("Shut down database.\n");
                mongoOperator.shutdownDatabase();
                //std::this_thread::sleep_for(std::chrono::seconds(1));
                Logger::INSTANCE().write("Terminate process.\n");
                server->terminate();
                return foundFCV;
            }
            catch (std::exception& e)
            {
                Logger::INSTANCE().write("Failed to request the current server compatibility version. Error: " + std::string(e.what()));
            }
        }
        catch (std::exception& e)
        {
            Logger::INSTANCE().write("Failed to start the server of version " + serverVersion + ". Assuming a not matching feature compatibility version and proceeding with next server. Error message:"+std::string(e.what()) + "\n");
        }
    }
    return 0;
}

void MongoDBUpgrader::performUpgrade(int version)
{
    std::string serverVersion("");
    if (version == 5)
    {
        serverVersion = CurrentPaths::INSTANCE().getMongoVersion5();
    }
    else if (version == 6)
    {
        serverVersion = CurrentPaths::INSTANCE().getMongoVersion6();
    }
    else if (version == 7)
    {
        serverVersion = CurrentPaths::INSTANCE().getMongoVersion7();
    }
    else
    {
        assert(maxVersion == 7);
    }
    const std::string mongoServerPath = CurrentPaths::INSTANCE().getMongoServerCollectionDirectory();
    updateServerFCV(serverVersion);
}

void MongoDBUpgrader::performUpgrade4_2To4_4()
{
    const std::string serverVersion = CurrentPaths::INSTANCE().getMongoVersion4_4();
    updateServerFCV(serverVersion);
}

void MongoDBUpgrader::updateServerFCV(const std::string& _version)
{
    Logger::INSTANCE().write("Performing upgrade to version " + _version + "\n");
    const std::string mongoServerPath = CurrentPaths::INSTANCE().getMongoServerCollectionDirectory();
    std::string serverExecutablePath = mongoServerPath + "\\" + _version + "\\mongod.exe";
    
    MongoDBServerRunner mongoDBProcess(serverExecutablePath, m_cfgPath);
    MongoDBShellExecutor mongoOperator(m_settings);
    mongoOperator.setFeatureCompatibilityVersion(_version);
    mongoOperator.shutdownDatabase();
    //std::this_thread::sleep_for(std::chrono::seconds(1));
    mongoDBProcess.terminate();
}

void MongoDBUpgrader::runServer(const std::string _path)
{
    system(_path.c_str());
}
