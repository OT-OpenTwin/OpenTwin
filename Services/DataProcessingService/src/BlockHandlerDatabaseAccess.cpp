// @otlicense
// File: BlockHandlerDatabaseAccess.cpp
// 
// License:
// Copyright 2025 by OpenTwin
//  
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
//  
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// @otlicense-end

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
#include "IndexHandler.h"

#include "OTServiceFoundation/TimeFormatter.h"
#include "SolverReport.h"

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
	IndexHandler indexHandler(collectionName);
	indexHandler.createDefaultIndexes();

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
	const std::string debugQuery = bsoncxx::to_json(m_query.view());
	SolverReport::instance().addToContent("Executing query: " + debugQuery + "\n");
	
	const std::string debugProjection = bsoncxx::to_json(m_projection.view());
	SolverReport::instance().addToContent("Executing projection: " + debugProjection + "\n");
	mongocxx::options::find options;
	options.projection(m_projection);
	options.limit(m_documentLimit);
	SolverReport::instance().addToContent("Query limit: " + std::to_string( m_documentLimit )+ "\n");
	SolverReport::instance().addToContent("Sorting by _id: " + std::to_string(m_sortByID) + "\n");

	if (m_sortByID)
	{
		options.sort(m_sort);
		bsoncxx::builder::basic::document hintDoc;
		const std::string firstDefaultIndex = IndexHandler::getDefaultIndexes().front();
		hintDoc.append(bsoncxx::builder::basic::kvp(firstDefaultIndex, 1));
		SolverReport::instance().addToContent("Hint: " + bsoncxx::to_json(hintDoc) + "\n");

		mongocxx::v_noabi::hint hint(hintDoc.extract());
		options.hint(hint);
	}

	auto startTime = std::chrono::high_resolution_clock::now();

	DataStorageAPI::DataStorageResponse dbResponse = m_resultCollectionAccess->searchInResultCollection(m_query, options);

	auto endTime = std::chrono::high_resolution_clock::now();
	const std::string queryDuration =	TimeFormatter::formatDuration(startTime, endTime);
	
	SolverReport::instance().addToContent("Query executed in " + queryDuration + "\n");
	
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
		
		SolverReport::instance().addToContentAndDisplay("Query returned " + std::to_string(numberOfDocuments) + " results.\n", _uiComponent);
			
		ot::JsonDocument dataDoc;
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
	m_queriedData.setMetadataCampaign(campaign);
	m_queriedData.setMetadataSeries(series);

	m_dataPerPort[outputConnectorName] = &m_queriedData;
}

void BlockHandlerDatabaseAccess::createLabelFieldNameMap()
{
	
	const std::string selectedQuantity = m_selectedQuantityLabel;

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

	m_sortByID = _blockEntity->getReproducableOrder();
	if (m_sortByID)
	{
		m_sort = (bsoncxx::builder::stream::document{}
			<< "_id" << 1  // 1 for ascending, -1 for descending
			<< bsoncxx::builder::stream::finalize);
	}
	
	createLabelFieldNameMap();
	AdvancedQueryBuilder builder;
	m_query = builder.connectWithAND(std::move(m_comparisons));

	std::vector<std::string> projectionNamesForExclusion{ "SchemaVersion", "SchemaType"};

	m_projection = builder.GenerateSelectQuery(projectionNamesForExclusion, false, false);
}

void BlockHandlerDatabaseAccess::addParameterQueries(EntityBlockDatabaseAccess* _blockEntity)
{
	std::list<ValueComparisionDefinition> queries =	_blockEntity->getAdditionalQueries();
	const auto& parametersByLabel =	m_resultCollectionMetadataAccess->getMetadataCampaign().getMetadataParameterByLabel();
	for (ValueComparisionDefinition& query : queries)
	{
		const std::string parameterLabel = query.getName();
		auto parameterByLabel	= parametersByLabel.find(parameterLabel);
		if (parameterByLabel == parametersByLabel.end())
		{
			OT_LOG_E("Trying to query for: " + parameterLabel + " which was not found in the campaign.");
			assert(0);
		}
		else
		{
			const std::string parameterID = std::to_string(parameterByLabel->second->parameterUID);
			query.setName(parameterID);
		}
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
			m_selectedQuantityLabel= selectedQuantity->quantityName;
			
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
		if (_definition.getType() == ot::TypeNames::getStringTypeName() && _definition.getComparator()!= "=")
		{
			throw std::exception(("Query for " + _definition.getName() + " targets a string value with a not supported comparator. Only equality queries are currently supported.").c_str());
		}
		AdvancedQueryBuilder builder;
		BsonViewOrValue query =	builder.createComparison(_definition);
		m_comparisons.push_back(query);
	}

}

std::string BlockHandlerDatabaseAccess::getBlockType() const
{
	return "Database Access Block";
}
