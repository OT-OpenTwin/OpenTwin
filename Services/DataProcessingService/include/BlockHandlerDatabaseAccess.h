#pragma once
#include "BlockHandler.h"
#include "EntityBlockDatabaseAccess.h"
#include "Document/DocumentAccess.h"
#include "MetadataCampaign.h"
#include "MetadataParameter.h"
#include "AdvancedQueryBuilder.h"
#include "ResultDataStorageAPI.h"

class BlockHandlerDatabaseAccess : public BlockHandler
{
public:
	BlockHandlerDatabaseAccess(EntityBlockDatabaseAccess* blockEntity, const HandlerMap& handlerMap);
	~BlockHandlerDatabaseAccess();

	bool executeSpecialized() override;	

private:
	uint32_t _dataColumns;
	uint32_t _dataRows;
	
	DataStorageAPI::ResultDataStorageAPI* _dataStorageAccess = nullptr;
	std::list< BsonViewOrValue> _comparisons;
	std::vector<std::string> _projectionNames;

	BsonViewOrValue _query;
	BsonViewOrValue _projection;

	bool _isValid = true;
	std::vector<std::string> _connectorNames;

	void AddComparision(const ValueComparisionDefinition& definition);
	void AddParameter(ValueComparisionDefinition& definition, const MetadataParameter& parameter, const std::string& connectorName);
};
