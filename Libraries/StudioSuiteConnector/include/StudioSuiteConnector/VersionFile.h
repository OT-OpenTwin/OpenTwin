#pragma once

#include <string>
#include <map>
#include <fstream>

class VersionFile
{
public:
	VersionFile(const std::string& filePath) : versionFilePath(filePath) {};
	VersionFile() = delete;
	~VersionFile() {};

	void write(const std::string& baseProjectName, const std::string& projectName, const std::string& version);
	void read();

	std::string getProjectName() { return projectName; }
	std::string getVersion() { return version; }

private:
	void writeFileInfo(std::string path, std::string& projectRoot, std::ofstream& file);

	std::string versionFilePath;
	std::string projectName;
	std::string version;
	std::map<std::string, size_t> projectFiles;
};

