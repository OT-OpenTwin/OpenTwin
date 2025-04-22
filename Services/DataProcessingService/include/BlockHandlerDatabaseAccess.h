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
	struct QueryDescription
	{
		std::string m_connectorName;
		std::string m_projectionName;
		PipelineData m_pipelineData;
	};
	
	DataStorageAPI::ResultDataStorageAPI* m_resultCollectionAccess = nullptr;
	ResultCollectionMetadataAccess* m_resultCollectionMetadataAccess = nullptr;
	std::list< BsonViewOrValue> m_comparisons;
	BsonViewOrValue m_query;
	BsonViewOrValue m_projection;
	const int m_documentLimit = 0;

	std::list< QueryDescription> m_queryDescriptions;
	std::map<std::string, ot::Variable> m_fixedParameter;



	void buildQuery(EntityBlockDatabaseAccess* _blockEntity);
	const MetadataSeries* addSeriesQuery(EntityBlockDatabaseAccess* _blockEntity);
	void addQuantityQuery(EntityBlockDatabaseAccess* _blockEntity);
	void addParameterQueries(EntityBlockDatabaseAccess* _blockEntity);
	
	void buildContainsQuery(const ValueComparisionDefinition& _definition, AdvancedQueryBuilder& _builder, ot::StringToVariableConverter& _converter, bool _contains);
	
	void addComparision(const ValueComparisionDefinition& _definition);
	void addParameterQueryDescription(ValueComparisionDefinition& definition, const MetadataParameter& parameter, const std::string& connectorName);

	void extractQuantity(QueryDescription& _queryDescription, ot::ConstJsonObject& _databaseDocument);
	void addNotFixedParameter(PipelineDataDocument& _document, ot::ConstJsonObject& _databaseDocument, const MetadataSeries* _series, const MetadataCampaign* _campaign);
	void extractParameter(QueryDescription& _queryDescription, ot::ConstJsonObject& _databaseDocument);
	
};
