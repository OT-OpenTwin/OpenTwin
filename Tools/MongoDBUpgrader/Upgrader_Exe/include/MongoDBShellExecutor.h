#pragma once
#include "MongoDBSettings.h"
#include <mongocxx/client.hpp>


class MongoDBShellExecutor
{
public:
    MongoDBShellExecutor(const MongoDBSettings& _settings);

    void setFeatureCompatibilityVersion(const std::string& version);
    void shutdownDatabase();

private:
    mongocxx::client m_client;
};
