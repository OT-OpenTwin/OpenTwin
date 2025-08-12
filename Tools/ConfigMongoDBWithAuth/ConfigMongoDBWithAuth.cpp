
// OpenTwin header
#include "OTSystem/SystemProcess.h"

// std header
#include <iostream>
#include <string>
#include <fstream>
#include <filesystem>
#include <list>
#include <cassert>

bool lineStartsWith(const std::string& line, const std::string& pattern)
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

void modifyConfigFile(const std::string& configFile, const std::string& tlsKeyPath)
{
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

	for (auto line : inputFileContent)
	{
		if (lineStartsWith(line, "authorization:"))
		{
			line.replace(line.find("disabled"), sizeof("disabled") - 1, "enabled");
			outputFileContent.push_back(line);
		}
		else if (lineStartsWith(line, "bindIp:"))
		{
			outputFileContent.push_back(line);

			outputFileContent.push_back("  tls: ");
			outputFileContent.push_back("    mode: requireTLS");
			outputFileContent.push_back("    certificateKeyFile: " + tlsKeyPath);
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

int main(int argc, char** argv)
{
	if (argc < 3) {
		std::cout << "Usage: ConfigMongoDBWithAuth.exe <config file path> <tls key path> <new port>" << std::endl;
		return 1;
	}

	std::string configFile = argv[1];
	std::string tlsKeyPath = argv[2];

	modifyConfigFile(configFile, tlsKeyPath);

	return 0;
}
