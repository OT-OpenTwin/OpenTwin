#include "OTServiceFoundation/BusinessLogicHandler.h"
#include "OTCore/OTAssert.h"

std::string BusinessLogicHandler::CreateNewUniqueTopologyName(std::list<std::string>& folderContent, const std::string& folderName, const std::string& fileName, int startNumber, bool alwaysNumbered)
{
	std::string fullFileName = folderName + "/" + fileName;
	int count = startNumber;
	if (alwaysNumbered)
	{
		fullFileName += "_" + std::to_string(count);
	}
	while (std::find(folderContent.begin(), folderContent.end(), fullFileName) != folderContent.end())
	{
		fullFileName = folderName + "/" + fileName + "_" + std::to_string(count);
		count++;
	}
	return fullFileName;
}

std::string BusinessLogicHandler::CreateNewUniqueTopologyName(const std::string& folderName, const std::string& fileName, int startNumber, bool alwaysNumbered)
{
	assert(_modelComponent != nullptr);
	std::list<std::string> folderItems = _modelComponent->getListOfFolderItems(folderName);
	return CreateNewUniqueTopologyName(folderItems, folderName, fileName, startNumber, alwaysNumbered);
}


std::vector<std::string> BusinessLogicHandler::CreateNewUniqueTopologyNames(std::list<std::string>& folderContent, const std::string& folderName, const std::string& fileName, uint64_t numberOfFiles, int startNumber, bool alwaysNumbered)
{
	std::vector<std::string> newNames;
	newNames.reserve(numberOfFiles);
	for (uint64_t i = 0; i < numberOfFiles; i++)
	{
		newNames.push_back(CreateNewUniqueTopologyName(folderContent, folderName, fileName, startNumber, alwaysNumbered));
		folderContent.push_back(newNames.back());
	}
	return newNames;
}

std::vector<std::string> BusinessLogicHandler::CreateNewUniqueTopologyNames(const std::string& folderName, const std::string& fileName, uint64_t numberOfFiles, int startNumber, bool alwaysNumbered)
{
	assert(_modelComponent != nullptr);
	std::list<std::string> folderItems = _modelComponent->getListOfFolderItems(folderName);
	return CreateNewUniqueTopologyNames(folderItems, folderName, fileName, numberOfFiles, startNumber, alwaysNumbered);
}


inline void BusinessLogicHandler::CheckEssentials()
{
	OTAssert(_modelComponent != nullptr, "No model component set");
	OTAssert(_uiComponent != nullptr, "No UI component set");
}