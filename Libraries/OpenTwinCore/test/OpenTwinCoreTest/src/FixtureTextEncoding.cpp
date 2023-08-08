#include "FixtureTextEncoding.h"
#include <fstream>

std::vector<unsigned char> FixtureTextEncoding::ReadFile(ot::TextEncoding encoding)
{
	std::string projectDir = PROJECT_DIR;
	projectDir += "TestEncodingTypeFiles\\";
	if (encoding == ot::TextEncoding::ANSI)
	{
		projectDir += "ANSI.txt";
	}
	else if (encoding == ot::TextEncoding::UTF8)
	{
		projectDir += "UTF8.txt";
	}
	else if (encoding == ot::TextEncoding::UTF8_BOM)
	{
		projectDir += "UTF8_BOM.txt";
	}
	else if (encoding == ot::TextEncoding::UTF16_BEBOM)
	{
		projectDir += "UTF16_BE_BOM.txt";
	}

	else if (encoding == ot::TextEncoding::UTF16_LEBOM)
	{
		projectDir += "UTF16_LE_BOM.txt";
	}
	else
	{
		return std::vector<unsigned char>();
	}
	
	std::ifstream file(projectDir, std::ios::binary | std::ios::ate);

	if (file.is_open())
	{
		std::streampos size = file.tellg();

		std::shared_ptr<char> memBlock(new char[size]);
		file.seekg(0, std::ios::beg);
		file.read(memBlock.get(), size);
		file.close();
		std::vector<unsigned char> memBlockVector(memBlock.get(), memBlock.get() + size);
		return memBlockVector;
	}
	else
	{
		throw std::exception("File could not be opened");
	}
}
