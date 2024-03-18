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

	// Now write information about all files in the project and their timestamps
	writeFileInfo(baseProjectName + ".cst", projectRoot, file);

	for (const auto& dirEntry : std::filesystem::recursive_directory_iterator(baseProjectName + "/Result"))
	{
		if (!dirEntry.is_directory())
		{
			std::string path = dirEntry.path().string();
			writeFileInfo(path, projectRoot, file);
		}
	}
}

void VersionFile::read()
{
	projectName.clear();
	version.clear();
	projectFiles.clear();

	std::ifstream file(versionFilePath);

	// Check whether the file version is supported
	std::string line;
	std::getline(file, line);
	int fileVersion = std::atoi(line.c_str());
	if (fileVersion != 1) throw (std::string("Unsupported file version of version information file."));

	// Read the project name and version
	std::getline(file, projectName);
	std::getline(file, version);

	// Read the information about the files and time stamps
	while (!file.eof())
	{
		std::string path, time;
		std::getline(file, path);
		std::getline(file, time);

		size_t timeStamp = atoll(time.c_str());

		projectFiles[path] = timeStamp;
	}
}

void VersionFile::writeFileInfo(std::string path, std::string &projectRoot, std::ofstream &file)
{
	// Make sure to use a forward slash convention in the file paths
	std::replace(path.begin(), path.end(), '\\', '/');

	// Write the file path (excluding the root folder)
	file << path.substr(projectRoot.size()+1) << std::endl;

	// Get the time stamp from the path and write it to the file
	auto timeStamp = std::filesystem::last_write_time(path).time_since_epoch();
	file << timeStamp.count() << std::endl;
}

