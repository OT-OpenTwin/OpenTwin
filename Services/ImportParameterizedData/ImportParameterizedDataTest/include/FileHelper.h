#pragma once
#include <vector>
#include <string>
class FileHelper
{
public:
	static std::vector<char> extractFileContentAsBinary(const std::string& fileName);

	static std::string getFilePath();
};

