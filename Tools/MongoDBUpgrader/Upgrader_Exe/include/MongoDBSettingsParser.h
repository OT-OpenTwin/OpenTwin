#pragma once
#pragma warning(disable : 4267)
#pragma warning(disable : 4244)

#include <string>
#include <list>
#include "MongoDBSettings.h"

class MongoDBSettingsParser
{
public:
    MongoDBSettingsParser(const std::string& _binPath);
    MongoDBSettingsParser(const std::string& _binPath, const std::string& _adminPsw);
    
    const MongoDBSettings& getMongoDBSettings() { return m_settings; }
    std::string getCurrentMongoDBServiceVersion();
    void extractDataFromConfig();
    std::string getUpdatedConfig();
    std::string getTempMongoServerConfPath();
    void createTempMongoServerConf(const std::string& _path);
    
private:

    MongoDBSettings m_settings;

    const std::string m_cfgKeyWordPort = "port";
    const std::string m_cfgKeyWordBindIP = "bindIp";
    const std::string m_cfgKeyWordCertKeyFile = "certificateKeyFile";
    const std::string m_cfgKeyWordDBPath = "dbPath";
    const std::string m_cfgKeyWordSysLog = "systemLog";
    const std::string m_cfgKeyWordSysPath = "path";
    const std::string m_cfgKeyWordTLSUse = "tlsUseSystemCA";
    const std::string m_cfgKeyWordJournal = "journal";

    void extractPaths(const std::string& _binPath);
    const bool extractionSucceeded(std::string& _errorMessage);
    std::string removeTrailingComments(const std::string& _str);
    void addJournalEntriesToConfig(const std::list<std::string>& _journalEntries);
    void removeJournalEnabledEntry(std::list<std::string>& _journalEntries);
};

