#pragma once
#include <string>
struct MongoDBSettings
{
    std::string m_configFilePath = "";
    std::string m_configFileContent = "";

    std::string m_bindIP = "";
    std::string m_port = "";
    std::string m_adminPsw = "";
    std::string m_certKeyFile = "";
    std::string m_executablePath = "";
    std::string m_dataPath = "";

    std::string m_tlsUseSystemCA = "";
};
