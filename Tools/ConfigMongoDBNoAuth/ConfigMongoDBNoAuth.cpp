// std header
#include <iostream>
#include <string>
#include <fstream>
#include <filesystem>
#include <list>
#include <cassert>

#include "OTSystem/Application.h"

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

void modifyConfigFile(const std::string& configFile, const std::string& dbPath, const std::string& logPath, const std::string& bindIp)
{
	ensureFolderExist(dbPath);
	ensureFolderExist(logPath);

	// Read the entire config file line by line
	std::ifstream inFile(configFile);
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

	std::filesystem::path logFilePath = std::filesystem::path(logPath) / std::filesystem::path("mongodb.log");

	for (auto line : inputFileContent)
	{
		if (lineStartsWith(line, "dbPath:"))
		{
			outputFileContent.push_back("  dbPath: " + dbPath);
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
			outputFileContent.push_back("  bindIp: " + bindIp);
		}
		else if (lineStartsWith(line, "#security:"))
		{
			line.replace(line.find("#security"), sizeof("#security")-1, "security");
			outputFileContent.push_back(line);
			outputFileContent.push_back("  authorization: disabled");
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

	// And finally we write the content back to the file
	std::ofstream outFile(configFile);

	for (auto line : outputFileContent)
	{
		outFile << line << std::endl;
	}

	outFile.close();
}

int main(int argc, char **argv)
{
	if (argc < 5) {
		std::cout << "Usage: ConfigMongoDBNoAuth.exe <config file path> <new db path> <new log path> <new bind IP>" << std::endl;
		return 1;
	}

	std::string configFile = argv[1];
	std::string dbPath     = argv[2];
	std::string logPath    = argv[3];
	std::string bindIP     = argv[4];

	modifyConfigFile(configFile, dbPath, logPath, bindIP);

	return 0;
}
