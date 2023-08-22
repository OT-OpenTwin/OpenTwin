#pragma once
#include "BlockHandler.h"
#include "EntityBlockDatabaseAccess.h"
#include "Document/DocumentAccess.h"

class BlockHandlerDatabaseAccess : public BlockHandler
{
public:
	BlockHandlerDatabaseAccess(EntityBlockDatabaseAccess* blockEntity);
	~BlockHandlerDatabaseAccess();

	virtual genericDataBlock Execute(genericDataBlock& inputData) override;

protected:
	DataStorageAPI::DocumentAccess* _dataStorageAccess = nullptr;
	std::string _queryString;
	std::string _projectionString;
};
