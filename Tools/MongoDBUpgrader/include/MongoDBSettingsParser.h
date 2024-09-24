#pragma once
#include <string>
#include <list>
#include "MongoDBSettings.h"

class MongoDBSettingsParser
{
public:
    MongoDBSettingsParser(const std::string _binPath);
    
    void requestAdminPassword();
    const MongoDBSettings& getMongoDBSettings() { return m_settings; }
    std::string getCurrentMongoDBServiceVersion();
    void extractDataFromConfig();
    std::string getUpdatedConfig();
    std::string getTempMongoServerConfPath();
    

private:

    MongoDBSettings m_settings;

    const std::string m_cfgKeyWordPort = "port";
    const std::string m_cfgKeyWordBindIP = "bindIp";
    const std::string m_cfgKeyWordCertKeyFile = "certificateKeyFile";
    const std::string m_cfgKeyWordDBPath = "dbPath";
    const std::string m_cfgKeyWordTLSUse = "tlsUseSystemCA";
    const std::string m_cfgKeyWordJournal = "journal";

    void extractPaths(const std::string& _binPath);
    const bool extractionSucceeded(std::string& _errorMessage);
    std::string removeTrailingComments(const std::string& _str);
    void addJournalEntriesToConfig(const std::list<std::string>& _journalEntries);
    void removeJournalEnabledEntry(std::list<std::string>& _journalEntries);
};

