#include "FileHandler.h"
#include <fstream>
#include <exception>
#include "EntityParameterizedDataSourceCSV.h"


std::string FileHandler::StoreFileInDataBase(const std::string& absoluteFilePath, const std::string entityName, const ot::UIDList& uids, const std::string& owner)
{
	std::string documentName = absoluteFilePath.substr(absoluteFilePath.find_last_of("/") + 1);
	std::string directoryPath = absoluteFilePath.substr(0, absoluteFilePath.find_last_of("/"));
	std::string documentType = documentName.substr(documentName.find(".") + 1);
	documentName = documentName.substr(0, documentName.find("."));
	
	auto uid = uids.begin();
	auto newSourceEntity = CreateNewSourceEntity(documentType, *uid, owner);

	newSourceEntity->setName(entityName);
	newSourceEntity->setInitiallyHidden(false);
	newSourceEntity->setFileProperties(directoryPath, documentName, documentType);

	auto memBlock = ExtractFileContentAsBinary(absoluteFilePath);
	uid++;
	std::unique_ptr <EntityBinaryData> newData(new EntityBinaryData(*uid, newSourceEntity.get(), nullptr, nullptr, nullptr, owner));
	newData->setData(memBlock.data(), memBlock.size());
	newData->StoreToDataBase(*uid +1);

	newSourceEntity->setData(newData->getEntityID(), newData->getEntityStorageVersion());
	
	newSourceEntity->StoreToDataBase(*uids.begin() +1);
	return documentName;
}

std::vector<char> FileHandler::ExtractFileContentAsBinary(const std::string& fileName)
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

std::string FileHandler::CreateNewUniqueTopologyName(std::list<std::string>& takenNames, const std::string& fileName, const std::string& entityDestinationPath)
{
	int count = 1;
	std::string fullFileName = entityDestinationPath + "/" + fileName;
	while (std::find(takenNames.begin(), takenNames.end(), fullFileName) != takenNames.end())
	{
		fullFileName = entityDestinationPath + "/" + fileName + "_" + std::to_string(count);
		count++;
	}
	return fullFileName;
}

std::string FileHandler::ExtractFileNameFromPath(const std::string& absoluteFilePath)
{
	std::string documentName = absoluteFilePath.substr(absoluteFilePath.find_last_of("/") + 1);
	documentName = documentName.substr(0, documentName.find("."));
	return documentName;

	return std::string();
}

std::shared_ptr<EntityParameterizedDataSource> FileHandler::CreateNewSourceEntity(const std::string& dataType, ot::UID entityID, const std::string& owner)
{
	EntityParameterizedDataSource* newSource = nullptr;
	if (dataType == "txt" || dataType == "csv" || dataType == "CSV")
	{
		newSource = new EntityParameterizedDataSourceCSV(entityID, nullptr, nullptr, nullptr, nullptr, owner);
	}
	else
	{
		newSource = new EntityParameterizedDataSource(entityID, nullptr, nullptr, nullptr, nullptr, owner);
	}
	return std::shared_ptr<EntityParameterizedDataSource>(newSource);
}
