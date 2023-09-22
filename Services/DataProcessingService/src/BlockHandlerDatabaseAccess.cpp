#pragma once
#include "BlockHandlerDatabaseAccess.h"
#include "ResultCollectionHandler.h"
#include "OpenTwinCore/JSONToVariableConverter.h"
#include "PropertyHandlerDatabaseAccessBlock.h"

BlockHandlerDatabaseAccess::BlockHandlerDatabaseAccess(EntityBlockDatabaseAccess* blockEntity)
{
	const std::string projectName =	blockEntity->getSelectedProjectName();

	ResultCollectionHandler resultCollectionHandler;
	std::string resultCollectionName = resultCollectionHandler.getProjectCollection(projectName) + ".results";
	_resultCollectionExists = resultCollectionHandler.CollectionExists(resultCollectionName);
	if (_resultCollectionExists)
	{
		const std::string dbURL = "Projects";
	
		_dataStorageAccess = new DataStorageAPI::DocumentAccess(dbURL, resultCollectionName);
	}

	
	OT_rJSON_createDOC(query);
	OT_rJSON_createDOC(projection);
	ot::rJSON::add(projection, "Value", 1);
	ot::rJSON::add(projection, "P1", 1);
	ot::rJSON::add(projection, "_id", 0);
	
	auto buffer =	PropertyHandlerDatabaseAccessBlock::instance().getBuffer(blockEntity->getEntityID());
	auto selectedQuantity =	buffer.quantities[buffer.SelectedQuantity];
	ot::rJSON::add(query, "Quantity", selectedQuantity.quantityIndex);

	_queryString = ot::rJSON::toJSON(query);
	_projectionString = ot::rJSON::toJSON(projection);

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
	auto dbResponse = _dataStorageAccess->GetAllDocuments(_queryString, _projectionString, 0);
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
