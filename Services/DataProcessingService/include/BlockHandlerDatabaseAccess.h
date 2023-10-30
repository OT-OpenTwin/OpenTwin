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

	void executeSpecialized() override;
	void setData(genericDataBlock& data, const std::string& targetPort) override;
	

protected:
	DataStorageAPI::DocumentAccess* _dataStorageAccess = nullptr;
	BufferBlockDatabaseAccess* _collectionInfos = nullptr;
	genericDataBlock _output;
	bool _isValid = true;
	std::string _dataConnectorName;
	std::string _parameterConnectorName;

	std::string _queryString;
	std::string _projectionString;
};
