#include "StudioSuiteConnector/VersionFile.h"

#include <cassert>
#include <fstream>
#include <iostream>
#include <string>
#include <algorithm>
#include <filesystem>

void VersionFile::write(const std::string & baseProjectName, const std::string& projectName, const std::string& version)
{
	assert(!versionFilePath.empty());

	// Write the file version
	std::ofstream file(versionFilePath);
	int fileVersion = 1;
	file << fileVersion << std::endl;

	// Write information about OpenTwin project name and project version
	file << projectName << std::endl;
	file << version << std::endl;

	// Determine the project root folder
	std::filesystem::path projectPath(baseProjectName);
	std::string projectRoot = projectPath.parent_path().string();
}

void VersionFile::read()
{
	projectName.clear();
	version.clear();

	std::ifstream file(versionFilePath);

	// Check whether the file version is supported
	std::string line;
	std::getline(file, line);
	int fileVersion = std::atoi(line.c_str());
	if (fileVersion != 1) throw (std::string("Unsupported file version of version information file."));

	// Read the project name and version
	std::getline(file, projectName);
	std::getline(file, version);
}


