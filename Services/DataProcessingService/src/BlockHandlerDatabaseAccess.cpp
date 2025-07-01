#include "BlockHandlerDatabaseAccess.h"

#include "Application.h"

#include "AdvancedQueryBuilder.h"
#include "OTCore/String.h"
#include "OTCore/JSONToVariableConverter.h"
#include "OTCore/GenericDataStructMatrix.h"
#include "OTCore/GenericDataStructSingle.h"

#include "OTCore/ValueComparisionDefinition.h"

#include "ResultDataStorageAPI.h"
#include "QuantityContainer.h"
#include "PropertyHandlerDatabaseAccessBlock.h"

#include "OTCore/ExplicitStringValueConverter.h"
#include "OTCore/ComparisionSymbols.h"

BlockHandlerDatabaseAccess::BlockHandlerDatabaseAccess(EntityBlockDatabaseAccess* blockEntity, const HandlerMap& handlerMap)
	: BlockHandler(blockEntity, handlerMap)
{
	//First get handler of the selected project result data.
	std::string collectionName;
	m_resultCollectionMetadataAccess = (PropertyHandlerDatabaseAccessBlock::getResultCollectionMetadataAccess(blockEntity,collectionName));

	if (!m_resultCollectionMetadataAccess->collectionHasMetadata())
	{
		const std::string errorMessage = "Database Access not possible. The selected collection has no meta data.";
		throw std::exception(errorMessage.c_str());
	}
	
	m_resultCollectionAccess = new DataStorageAPI::ResultDataStorageAPI(collectionName);
	
	buildQuery(blockEntity);
}

BlockHandlerDatabaseAccess::~BlockHandlerDatabaseAccess()
{
	if (m_resultCollectionAccess != nullptr)
	{
		delete m_resultCollectionAccess;
		m_resultCollectionAccess = nullptr;
	}
	if (m_resultCollectionMetadataAccess != nullptr)
	{
		delete m_resultCollectionMetadataAccess;
		m_resultCollectionMetadataAccess = nullptr;
	}
}


bool BlockHandlerDatabaseAccess::executeSpecialized()
{
	_uiComponent->displayMessage("Executing Database Acccess Block: " + m_blockName + "\n");
	const std::string debugQuery = bsoncxx::to_json(m_query.view());
	_uiComponent->displayMessage("Executing query: " + debugQuery + "\n");
	
	const std::string debugProjection = bsoncxx::to_json(m_projection.view());
	_uiComponent->displayMessage("Executing projection: " + debugProjection + "\n");

	auto dbResponse = m_resultCollectionAccess->SearchInResultCollection(m_query, m_projection, m_documentLimit);

	if (dbResponse.getSuccess())
	{
		const std::string queryResponse = dbResponse.getResult();
		ot::JsonDocument doc;
		doc.fromJson(queryResponse);
		auto allMongoDocuments = ot::json::getArray(doc, "Documents");
		
		//We look through the returned documents
		const uint32_t numberOfDocuments = allMongoDocuments.Size();
		if (numberOfDocuments == 0)
		{
			throw std::exception("Query returned nothing.\n");
		}
		
		if (Application::instance()->uiComponent()) {
			Application::instance()->uiComponent()->displayMessage("Query returned " + std::to_string(numberOfDocuments) + " results.\n");
		}
		auto& dataDoc = m_queriedData.m_data;
		ot::JsonArray entries;
		for (uint32_t i = 0; i < numberOfDocuments; i++)
		{
			auto singleMongoDocument = ot::json::getObject(allMongoDocuments, i);
			ot::JsonObject translatedResponseDoc;
			for (const LabelFieldNamePair& labelFieldNamePair : m_labelFieldNamePairs)
			{
				if (singleMongoDocument.HasMember(labelFieldNamePair.m_fieldName.c_str()))
				{
					ot::JsonValue value(singleMongoDocument[labelFieldNamePair.m_fieldName.c_str()], dataDoc.GetAllocator());
					ot::JsonString newKey(labelFieldNamePair.m_label, dataDoc.GetAllocator());
					translatedResponseDoc.AddMember(std::move(newKey), std::move(value), dataDoc.GetAllocator());
				}
			}
			entries.PushBack(translatedResponseDoc, dataDoc.GetAllocator());
		}
	
		m_queriedData.setData(std::move(entries));
	}
	else
	{
		const std::string message = "Data base query failed with this response: " + dbResponse.getMessage();
		throw std::exception(message.c_str());
	}
	return true;

}

void BlockHandlerDatabaseAccess::collectMetadataForPipeline(EntityBlockDatabaseAccess* _blockEntity)
{
	const MetadataCampaign* campaign = &m_resultCollectionMetadataAccess->getMetadataCampaign();

	//If a series is selected, we need to add a corresponding query. Maybe n
	const MetadataSeries* series = addSeriesQuery(_blockEntity);

	//Now we setup the datastream
	ot::Connector outputConnector = _blockEntity->getConnectorOutput();
	const std::string outputConnectorName = outputConnector.getConnectorName();
	m_queriedData.m_campaign = campaign;
	m_queriedData.m_series = series;

	m_dataPerPort[outputConnectorName] = &m_queriedData;
}

void BlockHandlerDatabaseAccess::createLabelFieldNameMap()
{
	
	const std::string selectedQuantity = m_queriedData.m_quantity->quantityLabel;
	const MetadataCampaign& campaign = m_resultCollectionMetadataAccess->getMetadataCampaign();
	const MetadataQuantity* quantity = campaign.getMetadataQuantitiesByLabel().find(selectedQuantity)->second;
	auto& allDependingParameter = campaign.getMetadataParameterByUID();
	const std::vector<ot::UID>& dependingParameterIDs = quantity->dependingParameterIds;
	for (ot::UID dependingParameterID : dependingParameterIDs)
	{
		LabelFieldNamePair queryDescription;
		queryDescription.m_label = allDependingParameter.find(dependingParameterID)->second.parameterLabel;
		queryDescription.m_fieldName = std::to_string(dependingParameterID);
		m_labelFieldNamePairs.push_back(queryDescription);
	}
}

void BlockHandlerDatabaseAccess::buildQuery(EntityBlockDatabaseAccess* _blockEntity)
{
	collectMetadataForPipeline(_blockEntity);

	//Next we add a query corresponding to the selected quantity.
	addQuantityQuery(_blockEntity);


	//Adding all parameter that may occur in the returned documents.
	addParameterQueries(_blockEntity);

	const bool reproducableOrder = _blockEntity->getReproducableOrder();

	createLabelFieldNameMap();
	AdvancedQueryBuilder builder;
	m_query = builder.connectWithAND(std::move(m_comparisons));

	std::vector<std::string> projectionNamesForExclusion{ "SchemaVersion", "SchemaType"};

	m_projection = builder.GenerateSelectQuery(projectionNamesForExclusion, false, false);
}

void BlockHandlerDatabaseAccess::addParameterQueries(EntityBlockDatabaseAccess* _blockEntity)
{
	std::list<ValueComparisionDefinition> queries =	_blockEntity->getAdditionalQueries();
	for (const ValueComparisionDefinition& query : queries)
	{
		addComparision(query);
	}
}

const MetadataSeries* BlockHandlerDatabaseAccess::addSeriesQuery(EntityBlockDatabaseAccess* _blockEntity)
{
	const MetadataSeries* series = nullptr;
	const std::string seriesLabel = _blockEntity->getSeriesSelection()->getValue();
	if (seriesLabel != "")
	{
		series = m_resultCollectionMetadataAccess->findMetadataSeries(seriesLabel);
		assert(series != nullptr);
		ot::UID valueUID = series->getSeriesIndex();
		ValueComparisionDefinition seriesComparision(MetadataSeries::getFieldName(), "=", std::to_string(valueUID), ot::TypeNames::getInt64TypeName(), "");
		addComparision(seriesComparision);
	}
	else
	{
		const std::list<MetadataSeries>& allSeries = m_resultCollectionMetadataAccess->getMetadataCampaign().getSeriesMetadata();
		AdvancedQueryBuilder builder;
		std::list< BsonViewOrValue> queries;
		for (const MetadataSeries& series : allSeries)
		{
			ValueComparisionDefinition seriesComparision(MetadataSeries::getFieldName(), "=", std::to_string(series.getSeriesIndex()), ot::TypeNames::getInt64TypeName(), "");
			BsonViewOrValue query = builder.createComparison(seriesComparision);
			queries.push_back(std::move(query));
		}
		BsonViewOrValue query = builder.connectWithOR(std::move(queries));
		m_comparisons.push_back(query);
	}

	return  series;
}

void BlockHandlerDatabaseAccess::addQuantityQuery(EntityBlockDatabaseAccess* _blockEntity)
{
	ValueComparisionDefinition quantityDef = _blockEntity->getSelectedQuantityDefinition();
	if (quantityDef.getName() == "")
	{
		throw std::exception("DatabaseAccessBlock has no quantity set.");
	}

	const std::string& valueDescriptionLabel = _blockEntity->getQuantityValueDescriptionSelection()->getValue();
	//The entity selection contains the names of the quantity/parameter. In the mongodb documents only the abbreviations are used.
	const auto selectedQuantity = m_resultCollectionMetadataAccess->findMetadataQuantity(quantityDef.getName());
	m_queriedData.m_quantity = selectedQuantity;

	if (selectedQuantity == nullptr)
	{
		throw std::exception("DatabaseAccessBlock has quantity set which is not part of the selected result collection.");
	}

	auto& valueDescriptions = selectedQuantity->valueDescriptions;
	ot::UID valueUID = 0;
	for (auto& valueDescription : valueDescriptions)
	{
		if (valueDescription.quantityValueLabel == valueDescriptionLabel)
		{
			valueUID = valueDescription.quantityIndex;
			m_queriedData.m_quantityDescription = &valueDescription;
		}
	}
	assert(valueUID != 0);

	//Now we add the query for the quantity ID
	ValueComparisionDefinition selectedQuantityDef(MetadataQuantity::getFieldName(), "=", std::to_string(valueUID), ot::TypeNames::getInt64TypeName(), "");
	addComparision(selectedQuantityDef);

	//Now we add a comparision for the searched quantity value.
	LabelFieldNamePair labelFieldNamePair;
	labelFieldNamePair.m_label = quantityDef.getName();
	quantityDef.setName(QuantityContainer::getFieldName());
	labelFieldNamePair.m_fieldName = QuantityContainer::getFieldName();
	addComparision(quantityDef);

	m_labelFieldNamePairs.push_back(labelFieldNamePair);
}

void BlockHandlerDatabaseAccess::addComparision(const ValueComparisionDefinition& _definition)
{
	if (!_definition.getComparator().empty() && !_definition.getValue().empty())
	{
		AdvancedQueryBuilder builder;
		BsonViewOrValue query =	builder.createComparison(_definition);
		m_comparisons.push_back(query);
	}

}