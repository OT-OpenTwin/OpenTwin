#include "MongoDBUpgrader.h"
#include "SharedResources.h"
#include "MongoDBServerRunner.h"
#include "MongoDBShellExecutor.h"
#include "Logger.h"
#include "FileWriter.h"
#include <cassert>
#include <iostream>

MongoDBUpgrader::MongoDBUpgrader(const MongoDBSettings& _settings, const std::string& _tempCfgPath)
    : m_settings(_settings), m_cfgPath(_tempCfgPath)
{}

void MongoDBUpgrader::performUpgrade(int version)
{
    std::string serverVersion("");
    if (version == 5)
    {
        serverVersion = "5.0.28";
    }
    else if (version == 6)
    {
        serverVersion = "6.0.16";
    }
    else if (version == 7)
    {
        serverVersion = "7.0.14";
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
    const std::string serverVersion = CurrentPaths::INSTANCE().getMongoVersion4();
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
