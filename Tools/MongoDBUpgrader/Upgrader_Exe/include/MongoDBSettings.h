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
    std::string m_logPath = "";

    std::string m_tlsUseSystemCA = "";

    const std::string toString() const
    {
        std::string summary = "configurationFilePath: " + m_configFilePath + "\n"+
            "config file content: " + m_configFileContent + "\n" +
            "IP: " + m_bindIP + "\n" +
            "Port: " + m_port + "\n" +
            "cert file: " + m_certKeyFile + "\n" +
            "exe path: " + m_executablePath + "\n" +
            "data path: " + m_dataPath + "\n" +
            "log path: " + m_logPath +"\n";
        return summary;
    }

};
