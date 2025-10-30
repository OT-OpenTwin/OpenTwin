// @otlicense

#include "MongoDBSettingsParser.h"
#include "FileWriter.h"
#include "SharedResources.h"
#include "Logger.h"


#include <boost/process.hpp>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <cctype>

//C4244, C4267 Boost warning to ignore

MongoDBSettingsParser::MongoDBSettingsParser(const std::string& _binPath, const std::string& _adminPsw)
    :MongoDBSettingsParser(_binPath)
{
    m_settings.m_adminPsw = _adminPsw;
}

MongoDBSettingsParser::MongoDBSettingsParser(const std::string& _binPath)
{
    extractPaths(_binPath);
}

std::string MongoDBSettingsParser::getCurrentMongoDBServiceVersion()
{
    boost::process::ipstream stream;
    boost::process::child process(m_settings.m_executablePath, "--version", boost::process::std_out > stream);
    process.wait();
    std::string line;
    
    std::getline(stream, line);
    //First line of return: db version v<Version>
    const int pos = 12;
    const std::string testTerm = line.substr(0, pos);
    if (testTerm != "db version v")
    {
        throw "Failed to determine the version of the current MongoDB installation.\n";
    }
    const std::string currentMongoVersion = line.substr(pos);
    return currentMongoVersion.substr(0, currentMongoVersion.size()-1);
}

void MongoDBSettingsParser::extractDataFromConfig()
{
    Logger::INSTANCE().write("Extracting data from config file \"" + m_settings.m_configFilePath + "\"\n");
    if (!std::filesystem::exists(m_settings.m_configFilePath)) {
        throw std::runtime_error(("Extracted filepath of the configuration file, used by the MongoDB service, is not valid. Extracted filepath: " +m_settings.m_configFilePath).c_str());
    }
    std::ifstream configFile(m_settings.m_configFilePath);
    if (!configFile.is_open()) {
        throw std::runtime_error("Failed to open config file");
    }
    try
    {
        // Now we search for the relevant information.
        std::string line;
        size_t journalNumberOfIntentations(0), sysLogNumberOfIntentations(0);
        std::list<std::string> journalSectionBuffer;
        while (std::getline(configFile, line))
        {
            //First we seperate potential comments from the line
            std::string cleanedLine = removeTrailingComments(line);
           
            //All entries of the journal section are buffered
            if (journalNumberOfIntentations == 0 && sysLogNumberOfIntentations == 0)
            {
                //Currently not in the journal section
                if (cleanedLine.find(m_cfgKeyWordPort) != std::string::npos)
                {
                    size_t serperatorPos = cleanedLine.find(':');
                    m_settings.m_port = cleanedLine.substr(serperatorPos + 2);
                }
                if (cleanedLine.find(m_cfgKeyWordBindIP) != std::string::npos)
                {
                    size_t serperatorPos = cleanedLine.find(':');
                    m_settings.m_bindIP = cleanedLine.substr(serperatorPos + 2);
                }
                if (cleanedLine.find(m_cfgKeyWordCertKeyFile) != std::string::npos)
                {
                    size_t serperatorPos = cleanedLine.find(':');
                    m_settings.m_certKeyFile = cleanedLine.substr(serperatorPos + 2);
                }
                if (cleanedLine.find(m_cfgKeyWordDBPath) != std::string::npos)
                {
                    size_t serperatorPos = cleanedLine.find(':');
                    m_settings.m_dataPath = cleanedLine.substr(serperatorPos + 2);
                }
                //setParameter:
                //  tlsUseSystemCA: true
                if (cleanedLine.find(m_cfgKeyWordTLSUse) != std::string::npos)
                {
                    size_t serperatorPos = cleanedLine.find(':');
                    m_settings.m_tlsUseSystemCA = cleanedLine.substr(serperatorPos + 2);
                }
                if (cleanedLine.find(m_cfgKeyWordJournal) != std::string::npos)
                {
                    journalNumberOfIntentations = cleanedLine.find(m_cfgKeyWordJournal);
                }
                if (cleanedLine.find(m_cfgKeyWordSysLog) != std::string::npos)
                {
                    sysLogNumberOfIntentations = 2;
                }
            }
            else if(journalNumberOfIntentations != 0)
            {
                //The parser is currently in the journal section
                int currentIntentationLvl = static_cast<int>(cleanedLine.find_first_not_of(' '));
                if (currentIntentationLvl == std::string::npos)
                {
                    currentIntentationLvl = 0;
                }
                if (currentIntentationLvl <= journalNumberOfIntentations)
                {
                    //We left the journal section
                    journalNumberOfIntentations = 0;
                }
                else
                {
                    journalSectionBuffer.push_back(line);
                }
            }
            else if (sysLogNumberOfIntentations != 0)
            {
                int currentIntentationLvl = static_cast<int>(cleanedLine.find_first_not_of(' '));
                if (currentIntentationLvl == std::string::npos)
                {
                    currentIntentationLvl = 0;
                }
                if (currentIntentationLvl < sysLogNumberOfIntentations)
                {
                    sysLogNumberOfIntentations = 0;
                }
                if(cleanedLine.find(m_cfgKeyWordSysPath) != std::string::npos)
                { 
                    size_t serperatorPos = cleanedLine.find(':');
                    m_settings.m_logPath = cleanedLine.substr(serperatorPos + 2);
                    sysLogNumberOfIntentations = 0;
                }
            }

            //If we are not in the journal section, we add all existing lines, including comments
            if (journalNumberOfIntentations == 0)
            {
                m_settings.m_configFileContent += line + "\n";
            }
        }

        std::string errrorMessage("");
        if (!extractionSucceeded(errrorMessage))
        {
            throw std::exception(errrorMessage.c_str());
        }

        //Now we deal with the journal section. The "enabled" entry is disabled from version 7 on, thus we neew to filter it out
        removeJournalEnabledEntry(journalSectionBuffer);
        //If there is still another entry in the journal section, we add the entries back into the config
        addJournalEntriesToConfig(journalSectionBuffer);
    }
    catch (std::exception& e)
    {
        configFile.close();
        throw std::exception(std::string("Error while extracting information from the cfg: " + std::string(e.what())).c_str());
    }
}

std::string MongoDBSettingsParser::getUpdatedConfig()
{
    if (m_settings.m_tlsUseSystemCA == "")
    {
        std::string extendedConfFile = m_settings.m_configFileContent + "\nsetParameter:\n"
            "  tlsUseSystemCA: true";
        return extendedConfFile;
    }
    else
    {
        Logger::INSTANCE().write("It was detected that the configuration file dinied the use of the system CA. Note, that this may lead to issues with this application and/or OpenTwin.");
        return m_settings.m_configFileContent;
    }
}

std::string MongoDBSettingsParser::getTempMongoServerConfPath()
{
    const std::string& mongoDir = CurrentPaths::INSTANCE().getMongoServerCollectionDirectory();
    const std::string tempConfFilePath = mongoDir + "\\mongod.cfg";
    return tempConfFilePath;
}

void MongoDBSettingsParser::createTempMongoServerConf(const std::string& _path)
{
    FileWriter fileWriter(_path);

    if (m_settings.m_tlsUseSystemCA == "" || m_settings.m_tlsUseSystemCA == "false")
    {
        const std::string updatedCfg = getUpdatedConfig();
        fileWriter.write(updatedCfg);
    }
    else
    {
        fileWriter.write(m_settings.m_configFilePath);
    }
}

    


void MongoDBSettingsParser::extractPaths(const std::string& _binPath)

{
    size_t configPos = _binPath.find("--config"); //9 character in the word
    if (configPos == std::string::npos) {
        throw std::runtime_error("Invalid service binary path \n");
    }

    //"C:\\Program Files\\MongoDB\\Server\\4.4\\bin\\mongod.exe --config C:\\Program Files\\MongoDB\\Server\\4.4\\bin\\mongod.cfg --service";
    m_settings.m_executablePath = _binPath.substr(1, configPos - 3);
    if (!std::filesystem::exists(m_settings.m_executablePath))
    {
        throw std::runtime_error("Invalid service executable path: " + m_settings.m_executablePath + "\n");
    }

    const std::string temp = _binPath.substr(configPos + 10);
    size_t servicePos = temp.find("--service");
    if (servicePos == std::string::npos) {
        throw std::runtime_error("Invalid service binary path\n");
    }
    m_settings.m_configFilePath = temp.substr(0, servicePos - 2);
    if (!std::filesystem::exists(m_settings.m_configFilePath))
    {
        throw std::runtime_error("Invalid service config path: " + m_settings.m_configFilePath + "\n");
    }
    
}

const bool MongoDBSettingsParser::extractionSucceeded(std::string& _errorMessage)
{
    _errorMessage = "";
    const std::string base = "Failed to find ";
    bool success = true;
    if (m_settings.m_bindIP == "")
    {
        _errorMessage += base + m_cfgKeyWordBindIP +"\n";
        success = false;
    }
    if (m_settings.m_port == "")
    {
        _errorMessage += base + m_cfgKeyWordPort + "\n";
        success = false;
    }
    if (m_settings.m_certKeyFile == "")
    {
        _errorMessage += m_cfgKeyWordCertKeyFile + "\n";
        success = false;
    }
    if (m_settings.m_dataPath== "")
    {
        _errorMessage += base + m_cfgKeyWordDBPath+"\n";
        success = false;
    }
    return success;
}

std::string MongoDBSettingsParser::removeTrailingComments(const std::string& _str)
{
    size_t pos = _str.find('#');
    if (pos != std::string::npos)
    {
        if (pos == 0)
        {
            return "";
        }
        else
        {
            return _str.substr(0, _str.find('#') - 1);
        }
    }
    else
    {
        return _str;
    }
}

void MongoDBSettingsParser::addJournalEntriesToConfig(const std::list<std::string>& _journalEntries)
{
    int countOfNonCommentEntries = 0;
    for (const std::string& entry : _journalEntries)
    {
        std::string cleanedLine = removeTrailingComments(entry);

        for (char ch : cleanedLine) {
            if (std::isalnum(static_cast<unsigned char>(ch)) && !std::isspace(static_cast<unsigned char>(ch))) {
                countOfNonCommentEntries++;
                break;
            }
        }
    }
    if (countOfNonCommentEntries > 1)
    {
        for (const std::string& entry : _journalEntries)
        {
            m_settings.m_configFileContent += entry + "\n";
        }
    }
}

void MongoDBSettingsParser::removeJournalEnabledEntry(std::list<std::string>& _journalEntries)
{
    auto journalSectionBufferItt = _journalEntries.begin();
    while (journalSectionBufferItt != _journalEntries.end())
    {
        if (journalSectionBufferItt->find("enabled") != std::string::npos)
        {
            journalSectionBufferItt = _journalEntries.erase(journalSectionBufferItt);
        }
        else
        {
            journalSectionBufferItt++;
        }
    }
}
