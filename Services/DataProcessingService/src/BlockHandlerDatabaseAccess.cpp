#pragma once
#include "BlockHandlerDatabaseAccess.h"
#include "ResultCollectionHandler.h"

BlockHandlerDatabaseAccess::BlockHandlerDatabaseAccess(EntityBlockDatabaseAccess* blockEntity)
{
	const std::string projectName =	blockEntity->getSelectedProjectName();
	ResultCollectionHandler resultCollectionHandler;
	std::string resultCollectionName = resultCollectionHandler.getProjectCollection(projectName) + ".results";
	_queryString = blockEntity->getQuery();
	_projectionString = blockEntity->getProjection();
	const std::string dbURL = "Projects";
	
	_dataStorageAccess = new DataStorageAPI::DocumentAccess(dbURL, resultCollectionName);
}

BlockHandlerDatabaseAccess::~BlockHandlerDatabaseAccess()
{
	if (_dataStorageAccess != nullptr)
	{
		delete _dataStorageAccess;
		_dataStorageAccess = nullptr;
	}
}

BlockHandler::genericDataBlock BlockHandlerDatabaseAccess::Execute(BlockHandler::genericDataBlock& inputData)
{
	OT_rJSON_createDOC(query);
	//ot::rJSON::add(query, "P_1", 1);
	OT_rJSON_createDOC(projection);
	ot::rJSON::add(projection, "Value", 1);
	ot::rJSON::add(projection, "_id", 0);
	auto dbResponse = _dataStorageAccess->GetAllDocuments(/*ot::rJSON::toJSON(query)*/ "{}", ot::rJSON::toJSON(projection), 0);
	bool success = dbResponse.getSuccess();

	auto resultDoc = ot::rJSON::fromJSON(dbResponse.getResult());
	auto allEntries = resultDoc["Documents"].GetArray();
	
	BlockHandler::genericDataBlock result;
	ot::JSONToVariableConverter converter;
	for (uint32_t i = 0; i < allEntries.Size(); i++)
	{
		if (!allEntries[i].IsNull())
		{
			auto tt = allEntries[i].GetObject();
			result.push_back(converter(tt["Value"]));
		}
	}
	
	return result;
}
