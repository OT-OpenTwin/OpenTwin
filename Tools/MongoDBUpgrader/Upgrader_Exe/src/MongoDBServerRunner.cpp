#include "MongoDBServerRunner.h"
#include "WindowsUtilityFuctions.h"
#include "Logger.h"

#include <iostream>

MongoDBServerRunner::MongoDBServerRunner(const std::string& _serverPath, const std::string& _configPath)
{
    std::error_code boostErrorCode;
    Logger::INSTANCE().write("Starting mongodb server: " + _serverPath + " --config " + _configPath);
    m_process = boost::process::child(_serverPath, "--config", _configPath, boostErrorCode);
    if (boostErrorCode.value() != ERROR_SUCCESS)
    {
        throw std::exception(("Failed to start server process. Exitcode: " + boostErrorCode.message()).c_str());
    }

    std::this_thread::sleep_for(std::chrono::seconds(3));
    
    if (!m_process.running())
    {
        throw std::exception(std::string("MongoDB server crashed. Exit code: "+ std::to_string(m_process.exit_code())+ "\n").c_str());
    }
}

void MongoDBServerRunner::terminate()
{
    m_process.terminate();
    m_process.wait();
    Logger::INSTANCE().write("Server process terminated: " + wuf::getErrorMessage(m_process.exit_code()) + "\n");
}

MongoDBServerRunner::~MongoDBServerRunner()
{
    if (m_process.running())
    {
        m_process.terminate();
    }
}