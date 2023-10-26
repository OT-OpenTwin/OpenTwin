//#pragma once
//#include "BlockHandler.h"
//#include "EntityBlockDatabaseAccess.h"
//#include "Document/DocumentAccess.h"
//#include "BufferBlockDatabaseAccess.h"
//
//class BlockHandlerDatabaseAccess : public BlockHandler
//{
//public:
//	BlockHandlerDatabaseAccess(EntityBlockDatabaseAccess* blockEntity);
//	~BlockHandlerDatabaseAccess();
//
//	virtual genericDataBlock Execute(genericDataBlock& inputData) override;
//	bool ResultCollectionExists() const { return _resultCollectionExists; }
//
//protected:
//	bool _resultCollectionExists;
//	DataStorageAPI::DocumentAccess* _dataStorageAccess = nullptr;
//
//	BufferBlockDatabaseAccess* _collectionInfos = nullptr;
//	genericDataBlock _output;
//
//	std::string _dataConnectorName;
//	std::string _parameterConnectorName;
//
//	std::string _queryString;
//	std::string _projectionString;
//};
