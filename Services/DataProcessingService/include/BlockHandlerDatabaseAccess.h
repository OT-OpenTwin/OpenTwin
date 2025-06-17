#pragma once
#include "BlockHandler.h"
#include "EntityBlockDatabaseAccess.h"
#include "MetadataCampaign.h"
#include "MetadataParameter.h"
#include "AdvancedQueryBuilder.h"
#include "ResultCollectionMetadataAccess.h"
#include "ResultDataStorageAPI.h"
#include "OTCore/StringToVariableConverter.h"

class BlockHandlerDatabaseAccess : public BlockHandler
{
public:
	BlockHandlerDatabaseAccess(EntityBlockDatabaseAccess* blockEntity, const HandlerMap& handlerMap);
	~BlockHandlerDatabaseAccess();

	bool executeSpecialized() override;	

private:
	struct LabelFieldNamePair
	{
		std::string m_label; //Comes from the campaign
		std::string m_fieldName;
	};
	
	DataStorageAPI::ResultDataStorageAPI* m_resultCollectionAccess = nullptr;
	ResultCollectionMetadataAccess* m_resultCollectionMetadataAccess = nullptr;

	std::list< BsonViewOrValue> m_comparisons;
	BsonViewOrValue m_query;
	BsonViewOrValue m_projection;
	const int m_documentLimit = 0;

	std::list<LabelFieldNamePair> m_labelFieldNamePairs;

	PipelineData m_queriedData;

	void collectMetadataForPipeline(EntityBlockDatabaseAccess* _blockEntity);
	void createLabelFieldNameMap();
	void buildQuery(EntityBlockDatabaseAccess* _blockEntity);
	const MetadataSeries* addSeriesQuery(EntityBlockDatabaseAccess* _blockEntity);
	void addQuantityQuery(EntityBlockDatabaseAccess* _blockEntity);
	void addParameterQueries(EntityBlockDatabaseAccess* _blockEntity);
		
	void addComparision(const ValueComparisionDefinition& _definition);
		
};
