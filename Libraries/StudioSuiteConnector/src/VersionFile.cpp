#include "StudioSuiteConnector/VersionFile.h"

#include <cassert>
#include <fstream>
#include <iostream>

void VersionFile::write(const std::string& projectName, const std::string& version)
{
	assert(!versionFilePath.empty());

	std::ofstream file(versionFilePath);
	int fileVersion = 1;

	file << fileVersion << std::endl;
	file << projectName << std::endl;
	file << version << std::endl;
}

void VersionFile::read()
{
	projectName.clear();
	version.clear();

	std::ifstream file(versionFilePath);
	int fileVersion = 0;

	file >> fileVersion;
	if (fileVersion != 1) throw (std::string("Unsupported file version of version information file."));

	std::getline(file, projectName);
	std::getline(file, version);
}

