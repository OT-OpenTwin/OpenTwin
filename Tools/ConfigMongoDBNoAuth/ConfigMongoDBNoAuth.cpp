
// OpenTwin header
#include "OTSystem/SystemProcess.h"
#include "OTServiceFoundation/UserCredentials.h"
#include "OTCore/EncodingGuesser.h"
#include "OTCore/EncodingConverter_ISO88591ToUTF8.h"

// std header
#include <iostream>
#include <string>
#include <fstream>
#include <filesystem>
#include <list>
#include <cassert>

void ensureFolderExist(const std::string& path)
{
	try {
		std::filesystem::create_directories(path);
	}
	catch (std::exception &e)
	{
		std::cout << "ERROR: Directory could not be created: " << path << " (" << e.what() << ")" << std::endl;
	}
}

bool lineStartsWith(const std::string& line, const std::string &pattern)
{
	for (size_t index = 0; index < line.size(); index++)
	{
		if (line[index] != ' ')
		{
			// We found the beginning of the string
			std::string token = line.substr(index, pattern.size());
			return (token == pattern);
		}
	}

	return false;
}

void modifyConfigFile(const std::string& _configFile, const std::string& _dbPath, const std::string& _logPath, const std::string& _bindIp, const std::string& _bindPort)
{
	ensureFolderExist(_dbPath);
	ensureFolderExist(_logPath);

	// Read the entire config file line by line
	std::ifstream inFile(_configFile);
	std::list<std::string> inputFileContent;

	while (!inFile.eof())
	{
		std::string line;
		std::getline(inFile, line);

		inputFileContent.push_back(line);
	}

	inFile.close();

	// Now we process and modify the content
	std::list<std::string> outputFileContent;

	std::filesystem::path logFilePath = std::filesystem::path(_logPath) / std::filesystem::path("mongodb.log");
	bool tlsUseSystemCAIsSet = false;
	for (auto line : inputFileContent)
	{
		if (lineStartsWith(line, "dbPath:"))
		{
			outputFileContent.push_back("  dbPath: " + _dbPath);
		}
		else if (lineStartsWith(line, "systemLog:"))
		{
			outputFileContent.push_back(line);
		}
		else if (lineStartsWith(line, "path:"))
		{
			outputFileContent.push_back("  path: " + logFilePath.string());
		}
		else if (lineStartsWith(line, "bindIp:"))
		{
			outputFileContent.push_back("  bindIp: " + _bindIp);
		}
		else if (lineStartsWith(line, "port:"))
		{
			outputFileContent.push_back("  port: " + _bindPort);
		}
		else if (lineStartsWith(line, "#security:"))
		{
			line.replace(line.find("#security"), sizeof("#security")-1, "security");
			outputFileContent.push_back(line);
			outputFileContent.push_back("  authorization: disabled");
		}
		else if (lineStartsWith(line, "tlsUseSystemCA:"))
		{
			outputFileContent.push_back("  tlsUseSystemCA: true");
			tlsUseSystemCAIsSet = true;
		}
		else if (lineStartsWith(line, "security:"))
		{
			assert(0);
		}
		else
		{
			outputFileContent.push_back(line);
		}
	}

	if (!tlsUseSystemCAIsSet)
	{
		outputFileContent.push_back("setParameter:");
		outputFileContent.push_back("  tlsUseSystemCA: true");
	}

	// And finally we write the content back to the file
	std::ofstream outFile(_configFile);

	for (auto line : outputFileContent)
	{
		outFile << line << std::endl;
	}

	outFile.close();
}

bool setPermanentEnvironmentVariable(const char *value, const char *data)
{
	HKEY hKey;
	LPCTSTR keyPath = TEXT("System\\CurrentControlSet\\Control\\Session Manager\\Environment");
	LSTATUS lOpenStatus = RegOpenKeyEx(HKEY_LOCAL_MACHINE, keyPath, 0, KEY_ALL_ACCESS, &hKey);
	if (lOpenStatus == ERROR_SUCCESS)
	{
		LSTATUS lSetStatus = RegSetValueExA(hKey, value, 0, REG_SZ, (LPBYTE)data, (DWORD)(strlen(data) + 1));
		RegCloseKey(hKey);

		if (lSetStatus == ERROR_SUCCESS)
		{
			SendMessageTimeout(HWND_BROADCAST, WM_SETTINGCHANGE, 0, (LPARAM)"Environment", SMTO_BLOCK, 100, NULL);
			return true;
		}
	}

	return false;
}

void setAdminPassword(std::string& password, const std::string& jsFile)
{
	ot::EncodingGuesser guesser;
	ot::TextEncoding::EncodingStandard textEncoding =	guesser(password.c_str(), password.size());
	if (textEncoding == ot::TextEncoding::EncodingStandard::ANSI)
	{
		ot::EncodingConverter_ISO88591ToUTF8 converter;
		password = converter(password);
	}

	std::ifstream inFile(jsFile);
	std::list<std::string> inputFileContent;

	while (!inFile.eof())
	{
		std::string line;
		std::getline(inFile, line);

		inputFileContent.push_back(line);
	}

	inFile.close();

	std::list<std::string> outputFileContent;

	for (auto line : inputFileContent)
	{
		if (lineStartsWith(line, "pwd:"))
		{
			outputFileContent.push_back("    pwd: \"" + password + "\",");
		}
		else
		{
			outputFileContent.push_back(line);
		}
	}

	std::ofstream outFile(jsFile + "_tmp");

	for (auto line : outputFileContent)
	{
		outFile << line << std::endl;
	}

	outFile.close();

	ot::UserCredentials userCredential;
	userCredential.setPassword(password);

	setPermanentEnvironmentVariable("OPEN_TWIN_MONGODB_PWD", userCredential.getEncryptedPassword().c_str());
}

int main(int argc, char **argv)
{
	if (argc < 7) {
		std::cout << "Usage: ConfigMongoDBNoAuth.exe <config file path> <new db path> <new log path> <new bind IP> <admin password> <server config script>" << std::endl;
		return 1;
	}

	std::string configFile = argv[1];
	std::string dbPath     = argv[2];
	std::string logPath    = argv[3];
	std::string bindIP     = argv[4];
	std::string bindPort   = argv[5];
	std::string adminPWD   = argv[6];
	std::string serverJS   = argv[7];

	modifyConfigFile(configFile, dbPath, logPath, bindIP, bindPort);

	setAdminPassword(adminPWD, serverJS);

	return 0;
}
