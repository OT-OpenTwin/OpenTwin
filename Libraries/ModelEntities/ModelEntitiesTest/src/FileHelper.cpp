#include "FileHelper.h"
#include "OTSystem/OperatingSystem.h"

#include <fstream>


std::vector<char> FileHelper::extractFileContentAsBinary(const std::string& fileName)
{
	std::ifstream file(fileName, std::ios::binary | std::ios::ate);

	if (file.is_open())
	{
		std::streampos size = file.tellg();

		std::shared_ptr<char> memBlock(new char[size]);
		file.seekg(0, std::ios::beg);
		file.read(memBlock.get(), size);
		file.close();
		std::vector<char> memBlockVector(memBlock.get(), memBlock.get() + size);
		return memBlockVector;
	}
	else
	{
		throw std::exception("File cannot be opened.");
	}
}

std::string FileHelper::getFilePath()
{
	std::string executablePath = ot::os::getExecutablePath();
	executablePath = executablePath.substr(0, executablePath.find_last_of("\\")); //One level up
	executablePath = executablePath.substr(0, executablePath.find_last_of("\\")); // Other level up
	const std::string filePath = executablePath + "\\ModelEntitiesTest\\Files\\";
	return filePath;
}
