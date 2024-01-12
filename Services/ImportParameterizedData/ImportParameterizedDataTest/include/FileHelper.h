#pragma once
#include <vector>
#include <string>
class FileHelper
{
public:
	static std::vector<char> ExtractFileContentAsBinary(const std::string& fileName);
};

