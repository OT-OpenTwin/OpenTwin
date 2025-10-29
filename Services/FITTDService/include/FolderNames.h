// @otlicense

#pragma once
#include <string>

class FolderNames
{
public:
	static const std::string GetFolderNameSignalType() { return folderNameSignalType; }
	static const std::string GetFolderNameSolver() { return folderNameSolver; }
	static const std::string GetFolderNamePorts() { return folderNamePorts; }
	static const std::string GetFolderNameMonitors() { return folderNameMonitors; }
	static const std::string GetFolderNameRawResultBase() { return folderNameRawResultBase; }

private:
	static const std::string folderNameSignalType;
	static const std::string folderNameSolver;
	static const std::string folderNamePorts;
	static const std::string folderNameMonitors;
	static const std::string folderNameRawResultBase;
};