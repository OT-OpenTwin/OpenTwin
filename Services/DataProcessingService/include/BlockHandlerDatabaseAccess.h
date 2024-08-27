#pragma once
#include "BlockHandler.h"
#include "EntityBlockDatabaseAccess.h"
#include "Document/DocumentAccess.h"
#include "MetadataCampaign.h"
#include "MetadataParameter.h"
#include "AdvancedQueryBuilder.h"
#include "ResultCollectionMetadataAccess.h"
#include "ResultDataStorageAPI.h"

class BlockHandlerDatabaseAccess : public BlockHandler
{
public:
	BlockHandlerDatabaseAccess(EntityBlockDatabaseAccess* blockEntity, const HandlerMap& handlerMap);
	~BlockHandlerDatabaseAccess();

	bool executeSpecialized() override;	

private:
	struct QueryDescription
	{
		std::string m_connectorName;
		std::string m_projectionName;
		PipelineData m_outputData;
	};
	
	DataStorageAPI::ResultDataStorageAPI* m_resultCollectionAccess = nullptr;
	ResultCollectionMetadataAccess* m_resultCollectionMetadataAccess = nullptr;
	std::list< BsonViewOrValue> _comparisons;
	
	std::list< QueryDescription> m_queryDescriptions;

	BsonViewOrValue _query;
	BsonViewOrValue _projection;

	bool _isValid = true;

	void AddComparision(const ValueComparisionDefinition& definition);
	void AddParameter(ValueComparisionDefinition& definition, const MetadataParameter& parameter, const std::string& connectorName);
};
