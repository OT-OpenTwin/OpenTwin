#include <fstream>
#include <exception>
#include <vector>

#include <OpenTwinCommunication/ActionTypes.h>

#include "DataSourceHandler.h"

#include "EntityParameterizedDataSourceCSV.h"
#include "EntityBinaryData.h"
#include "ClassFactory.h"


DataSourceHandler::DataSourceHandler(std::string dataSourceFolder) : _dataSourceFolder(dataSourceFolder) 
{}

void DataSourceHandler::AddSourceFileToModel()
{
	auto uid = _reserveSourceUIDs.begin();
	std::list<ot::UID> topologyEntityIDList = { *uid };
	std::list<ot::UID> topologyEntityVersionList = { *uid +1};
	std::list<bool> topologyEntityForceVisible = { false };

	uid++;
	std::list<ot::UID> dataEntityIDList{ *uid };
	std::list<ot::UID> dataEntityVersionList{ *uid +1 };
	std::list<ot::UID> dataEntityParentList{ *_reserveSourceUIDs.begin() };

	_modelComponent->addEntitiesToModel(topologyEntityIDList, topologyEntityVersionList, topologyEntityForceVisible,
		dataEntityIDList, dataEntityVersionList, dataEntityParentList, "added new source file");
}

void DataSourceHandler::StorePythonScriptAsEntity(std::string fileName)
{
	CheckEssentials();

	auto memBlock = ExtractFileContentAsBinary(fileName);

	std::string documentName = fileName.substr(fileName.find_last_of("/") + 1);
	std::string path = fileName.substr(0, fileName.find_last_of("/"));
	std::string documentType = documentName.substr(documentName.find(".") + 1);
	documentName = documentName.substr(0, documentName.find("."));

	auto newSourceEntity = CreateNewSourceEntity(documentType);
	std::list<std::string> takenNames;
	std::string fullFileName = CreateNewUniqueTopologyNamePlainPossible("Scripts", documentName, takenNames);
	newSourceEntity->setName(fullFileName);
	newSourceEntity->setInitiallyHidden(false);
	newSourceEntity->setFileProperties(path, documentName, documentType);

	ot::UID newDataID = _modelComponent->createEntityUID();
	std::unique_ptr <EntityBinaryData> newData(new EntityBinaryData(newDataID, newSourceEntity.get(), nullptr, nullptr, nullptr, OT_INFO_SERVICE_TYPE_ImportParameterizedDataService));
	newData->setData(memBlock.data(), memBlock.size());
	newData->StoreToDataBase();

	newSourceEntity->setData(newData->getEntityID(), newData->getEntityStorageVersion());
	newSourceEntity->StoreToDataBase();

	std::list<ot::UID> topologyEntityIDList = { newSourceEntity->getEntityID() };
	std::list<ot::UID> topologyEntityVersionList = { newSourceEntity->getEntityStorageVersion() };
	std::list<bool> topologyEntityForceVisible = { false };

	std::list<ot::UID> dataEntityIDList{ newDataID };
	std::list<ot::UID> dataEntityVersionList{ newData->getEntityStorageVersion() };
	std::list<ot::UID> dataEntityParentList{ newSourceEntity->getEntityID() };

	_modelComponent->addEntitiesToModel(topologyEntityIDList, topologyEntityVersionList, topologyEntityForceVisible,
		dataEntityIDList, dataEntityVersionList, dataEntityParentList, "added new source file: " + fullFileName);
}

std::vector<char> DataSourceHandler::ExtractFileContentAsBinary(std::string fileName)
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

std::shared_ptr<EntityParameterizedDataSource> DataSourceHandler::CreateNewSourceEntity(std::string dataType)
{
	EntityParameterizedDataSource* newSource = nullptr;
	ot::UID newSourceID = _modelComponent->createEntityUID();
	if (dataType == "txt" || dataType == "csv" || dataType == "CSV")
	{
		newSource = new EntityParameterizedDataSourceCSV(newSourceID, nullptr, nullptr, nullptr, nullptr, OT_INFO_SERVICE_TYPE_ImportParameterizedDataService);
	}
	else
	{
		newSource = new EntityParameterizedDataSource(newSourceID, nullptr, nullptr, nullptr, nullptr, OT_INFO_SERVICE_TYPE_ImportParameterizedDataService);
	}
	return std::shared_ptr<EntityParameterizedDataSource>(newSource);
}


