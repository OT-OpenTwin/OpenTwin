#pragma once

#include <string>

class TmpFileManager
{
public:
	TmpFileManager() {};
	~TmpFileManager() {};

	static std::string createTmpDir(void);
	static bool checkFileOrDirExists(const std::string &path);
	static bool deleteDirectory(const std::string &pathName);

private:
	static std::string readEnvironmentVariable(const std::string &variableName);

};
