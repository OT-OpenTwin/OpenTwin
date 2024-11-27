#pragma once
#include "BlockHandler.h"
#include "EntityBlockDatabaseAccess.h"
#include "Document/DocumentAccess.h"
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
		PipelineData m_outputData;
	};
	
	DataStorageAPI::ResultDataStorageAPI* m_resultCollectionAccess = nullptr;
	ResultCollectionMetadataAccess* m_resultCollectionMetadataAccess = nullptr;
	std::list< BsonViewOrValue> m_comparisons;
	
	std::list< QueryDescription> m_queryDescriptions;

	BsonViewOrValue m_query;
	BsonViewOrValue m_projection;

	void buildRangeQuery(const ValueComparisionDefinition& _definition, AdvancedQueryBuilder& _builder, ot::StringToVariableConverter& _converter);
	void buildContainsQuery(const ValueComparisionDefinition& _definition, AdvancedQueryBuilder& _builder, ot::StringToVariableConverter& _converter, bool _contains);
	void setValueFromString(std::unique_ptr<ot::Variable>& _value, const std::string& _valueString, const std::string& _valueType) const;
	void addComparision(const ValueComparisionDefinition& _definition);
	void addParameter(ValueComparisionDefinition& definition, const MetadataParameter& parameter, const std::string& connectorName);
};
