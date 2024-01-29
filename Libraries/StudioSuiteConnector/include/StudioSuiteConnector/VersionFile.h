#pragma once

#include <string>

class VersionFile
{
public:
	VersionFile(const std::string& filePath) : versionFilePath(filePath) {};
	VersionFile() = delete;
	~VersionFile() {};

	void write(const std::string& projectName, const std::string& version);
	void read();

	std::string getProjectName() { return projectName; }
	std::string getVersion() { return version; }

private:
	std::string versionFilePath;
	std::string projectName;
	std::string version;

};

