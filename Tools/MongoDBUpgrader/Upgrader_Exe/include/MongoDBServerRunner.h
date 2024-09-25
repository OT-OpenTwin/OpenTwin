#pragma once

#include <string>
#include <boost/process/child.hpp>

class MongoDBServerRunner
{
public:
    MongoDBServerRunner(const std::string& _serverPath, const std::string& _configPath);

    void terminate();
    ~MongoDBServerRunner();
private:
    boost::process::child m_process;
};
