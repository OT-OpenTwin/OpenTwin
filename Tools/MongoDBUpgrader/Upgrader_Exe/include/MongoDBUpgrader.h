// @otlicense

#pragma once
#include "MongoDBSettings.h"

class MongoDBUpgrader
{
public:
    MongoDBUpgrader(const MongoDBSettings &_settings, const std::string& _tempCfgPath);

    int checkForFeatureCompatibilityVersion(int _startVersionMajor);

    void performUpgrade(int version);
    void performUpgrade4_2To4_4();
    static const int getSupportedMaxVersion() { return maxVersion; }

private:
    static const int maxVersion = 7;
    const MongoDBSettings& m_settings;
    const std::string m_cfgPath;

    void updateServerFCV(const std::string& _version);
    void runServer(const std::string _path);
};
