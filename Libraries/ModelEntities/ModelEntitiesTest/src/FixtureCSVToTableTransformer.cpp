#include "FixtureCSVToTableTransformer.h"
#include "FileHelper.h"
#include <fstream>
#include "OTSystem/OperatingSystem.h"


FixtureCSVToTableTransformer::FixtureCSVToTableTransformer()
	:m_filePath(FileHelper::getFilePath())
{
}

std::string FixtureCSVToTableTransformer::loadFileContent(std::string fullPath)
{
	auto fileContent = FileHelper::extractFileContentAsBinary(fullPath);
	std::string fileText(fileContent.begin(), fileContent.end());
	return fileText;
}

ot::GenericDataStructMatrix FixtureCSVToTableTransformer::getTable(const std::string& text, CSVProperties& properties)
{
	ot::GenericDataStructMatrix table = m_extractor(text, properties);
	return table;
}
