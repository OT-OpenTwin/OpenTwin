#include "BusinessLogicHandler.h"

std::string BusinessLogicHandler::CreateNewUniqueTopologyName(std::string folderName, std::string fileName)
{
	std::list<std::string> folderContent = _modelComponent->getListOfFolderItems(folderName);
	int count = 1;
	std::string fullFileName = folderName + "/" + fileName + "_" + std::to_string(count);
	while (std::find(folderContent.begin(), folderContent.end(), fullFileName) != folderContent.end())
	{
		fullFileName = folderName + "/" + fileName + "_" + std::to_string(count);
		count++;
	}
	return fullFileName;
}

std::string BusinessLogicHandler::CreateNewUniqueTopologyNamePlainPossible(std::string folderName, std::string fileName)
{
	std::list<std::string> folderContent = _modelComponent->getListOfFolderItems(folderName);
	int count = 1;
	std::string fullFileName = folderName + "/" + fileName;
	while (std::find(folderContent.begin(), folderContent.end(), fullFileName) != folderContent.end())
	{
		fullFileName = folderName + "/" + fileName + "_" + std::to_string(count);
		count++;
	}
	return fullFileName;
}

std::vector<std::string> BusinessLogicHandler::CreateNewUniqueTopologyName(std::string folderName, std::string fileName, int numberOfFiles)
{
	std::list<std::string> uniqueFileNames = _modelComponent->getListOfFolderItems(folderName);
	std::vector<std::string> newNames;
	int count = 1;
	std::string fullFileName = folderName + "/" + fileName;
	for (int i = 0; i < numberOfFiles; i++)
	{
		while (std::find(uniqueFileNames.begin(), uniqueFileNames.end(), fullFileName) != uniqueFileNames.end())
		{
			fullFileName = folderName + "/" + fileName + "_" + std::to_string(count);
			count++;
		}

		uniqueFileNames.push_back(fullFileName);
		newNames.push_back(fullFileName);
		fullFileName = folderName + "/" + fileName + "_" + std::to_string(count);
	}
		
	return newNames;
}



void BusinessLogicHandler::CheckEssentials()
{
	if (_modelComponent == nullptr || _uiComponent == nullptr)
	{
		throw std::runtime_error("UIComponent or ModelComponent is not connected with DataSourceHandler");
	}
}