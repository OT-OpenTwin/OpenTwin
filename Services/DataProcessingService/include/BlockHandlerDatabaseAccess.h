#pragma once
#include "BlockHandler.h"
#include "EntityBlockDatabaseAccess.h"
#include "Document/DocumentAccess.h"
#include "BufferBlockDatabaseAccess.h"
//
class BlockHandlerDatabaseAccess : public BlockHandler
{
public:
	BlockHandlerDatabaseAccess(EntityBlockDatabaseAccess* blockEntity, const HandlerMap& handlerMap);
	~BlockHandlerDatabaseAccess();
	
	bool executeSpecialized() override;	

protected:
	DataStorageAPI::DocumentAccess* _dataStorageAccess = nullptr;
	BufferBlockDatabaseAccess* _collectionInfos = nullptr;
	genericDataBlock _output;
	bool _isValid = true;
	std::string _quantityConnectorName;
	std::string _parameterConnectorName;

	std::string _queryString;
	std::string _projectionString;
};
