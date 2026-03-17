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

// Service header
#include "Application.h"
#include "SolverReport.h"
#include "MetadataVectorizer.h"
#include "QueryDescriptionBuilder.h"
#include "BlockHandlerDatabaseAccess.h"
#include "PropertyHandlerDatabaseAccessBlock.h"

// OpenTwin header
#include "OTCore/String.h"
#include "OTCore/TimeFormatter.h"
#include "OTCore/ResultCollectionDefaultIndexes.h"
#include "OTCore/DataStruct/GenericDataStructMatrix.h"
#include "OTCore/DataStruct/GenericDataStructSingle.h"
#include "OTCore/QueryDescription/ValueComparisonDescription.h"
#include "OTCore/Tuple/TupleDescriptionComplex.h"
#include "OTCore/Variable/ExplicitStringValueConverter.h"
#include "OTCore/Variable/JSONToVariableConverter.h"

#include "OTDataStorage/AdvancedQueryBuilder.h"
#include "OTResultDataAccess/QuantityContainer.h"

// std header
#include <regex>
#include <algorithm>

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
	m_dataLakeAccessor.accessPartition(collectionName);

	buildQuery(blockEntity);
}

BlockHandlerDatabaseAccess::~BlockHandlerDatabaseAccess()
{
	if (m_resultCollectionMetadataAccess != nullptr)
	{
		delete m_resultCollectionMetadataAccess;
		m_resultCollectionMetadataAccess = nullptr;
	}
}

bool BlockHandlerDatabaseAccess::executeSpecialized()
{		
	const std::string debugProjection = bsoncxx::to_json(m_projection.view());
	SolverReport::instance().addToContent("Executing projection: " + debugProjection + "\n");
	mongocxx::options::find options;
	options.projection(m_projection);
	options.limit(m_documentLimit);
	SolverReport::instance().addToContent("Query limit: " + std::to_string( m_documentLimit )+ "\n");
	SolverReport::instance().addToContent("Sorting by _id: " + std::to_string(m_sortByID) + "\n");

	//For complex values {value : { $elemMatch:{"1" : 0}}}
	//{value : { $elemMatch:{"0":{$gt : 0}}}

	if (m_sortByID)
	{
		options.sort(m_sort);
		bsoncxx::builder::basic::document hintDoc;
		//Here we switched to a compound index. Bad style, but for now it works.
		auto index = ResultCollectionDefaultIndexes::getDefaultIndexes().begin();
		hintDoc.append(bsoncxx::builder::basic::kvp(*index, 1));
		index++;
		hintDoc.append(bsoncxx::builder::basic::kvp(*index, 1));
		SolverReport::instance().addToContent("Hint: " + bsoncxx::to_json(hintDoc) + "\n");

		mongocxx::v_noabi::hint hint(hintDoc.extract());
		options.hint(hint);
	}

	auto startTime = std::chrono::high_resolution_clock::now();
	ot::JsonDocument result = m_dataLakeAccessor.executeQuery(options);
	auto endTime = std::chrono::high_resolution_clock::now();
	m_queriedData.setData(std::move(result));
	//const std::string queryDuration =	TimeFormatter::formatDuration(startTime, endTime);
	//
	//SolverReport::instance().addToContent("Query executed in " + queryDuration + "\n");
	//
	//if (dbResponse.getSuccess())
	//{
	//	const std::string queryResponse = dbResponse.getResult();
	//	ot::JsonDocument doc;
	//	doc.fromJson(queryResponse);
	//	auto allMongoDocuments = ot::json::getArray(doc, "Documents");
	//	
	//	//We look through the returned documents
	//	const uint32_t numberOfDocuments = allMongoDocuments.Size();
	//	if (numberOfDocuments == 0)
	//	{
	//		throw std::exception("Query returned nothing.\n");
	//	}
	//	
	//	SolverReport::instance().addToContentAndDisplay("Query returned " + std::to_string(numberOfDocuments) + " results.\n", _uiComponent);
	//		
	//	ot::JsonDocument dataDoc;
	//	ot::JsonArray entries;
	//	for (uint32_t i = 0; i < numberOfDocuments; i++)
	//	{
	//		auto singleMongoDocument = ot::json::getObject(allMongoDocuments, i);
	//		ot::JsonObject translatedResponseDoc;
	//		for (const LabelFieldNamePair& labelFieldNamePair : m_labelFieldNamePairsParameter)
	//		{
	//			if (singleMongoDocument.HasMember(labelFieldNamePair.m_fieldName.c_str()))
	//			{
	//				ot::JsonValue value(singleMongoDocument[labelFieldNamePair.m_fieldName.c_str()], dataDoc.GetAllocator());
	//				ot::JsonString newKey(labelFieldNamePair.m_label, dataDoc.GetAllocator());
	//				translatedResponseDoc.AddMember(std::move(newKey), std::move(value), dataDoc.GetAllocator());
	//			}
	//		}
	//		ot::JsonValue quantityValue(singleMongoDocument[QuantityContainer::getFieldName().c_str()], dataDoc.GetAllocator());
	//		std::string quantityID = std::to_string(singleMongoDocument[MetadataQuantity::getFieldName().c_str()].GetInt64());
	//		for (const auto& quantityPairs : m_labelFieldNamePairsQuantities)
	//		{
	//			if (quantityID == quantityPairs.m_fieldName)
	//			{
	//				ot::JsonString newKey(quantityPairs.m_label, dataDoc.GetAllocator());
	//				translatedResponseDoc.AddMember(std::move(newKey), std::move(quantityValue), dataDoc.GetAllocator());
	//				break;
	//			}
	//		}
	//		
	//		std::string seriesID = std::to_string(singleMongoDocument[MetadataSeries::getFieldName().c_str()].GetInt64());
	//		for (const auto& seriesPair : m_labelFieldNamePairsSeries)
	//		{
	//			if (seriesID == seriesPair.m_fieldName)
	//			{
	//				ot::JsonString newKey(MetadataSeries::getFieldName(), dataDoc.GetAllocator());
	//				translatedResponseDoc.AddMember(std::move(newKey), ot::JsonString(seriesPair.m_label, dataDoc.GetAllocator()), dataDoc.GetAllocator());
	//				break;
	//			}
	//		}

	//		entries.PushBack(translatedResponseDoc, dataDoc.GetAllocator());
	//	}
	//
	
	//}
	//else
	//{
	//	const std::string message = "Data base query failed with this response: " + dbResponse.getMessage();
	//	throw std::exception(message.c_str());
	//}
	return true;

}

void BlockHandlerDatabaseAccess::createQueryDescriptionsQuantities(EntityBlockDatabaseAccess* _blockEntity)
{
	ot::ValueComparisonDescription quantityDef = _blockEntity->getSelectedQuantityDefinition();
	if (quantityDef.getName() == "")
	{
		throw std::exception("DatabaseAccessBlock has no quantity set.");
	}

	//First we try to find the corresponding metadata
	std::list<const MetadataQuantity*> viableQuantities;
	const auto selectedQuantity = m_resultCollectionMetadataAccess->findMetadataQuantity(quantityDef.getName());
	if (selectedQuantity != nullptr)
	{
		viableQuantities.push_back(selectedQuantity);
	}
	else
	{
		// The selected quantity name may be a regex expression
		auto allQuantityLabel = m_resultCollectionMetadataAccess->listAllQuantityLabels();
		applyRegexFilter(allQuantityLabel, quantityDef.getName());
		for (const std::string& viableQuantityLabel : allQuantityLabel)
		{
			auto viableQuantity = m_resultCollectionMetadataAccess->findMetadataQuantity(viableQuantityLabel);
			viableQuantities.push_back(viableQuantity);
		}
	}

	if (viableQuantities.size() == 0)
	{
		throw std::exception("Given quantity label expression does not match any of the possible options.");
	}
	else
	{
		std::list<ot::QueryDescription> allQuantityQueries;
		ot::UIDList relatedParameterUIDs;
		for (const MetadataQuantity* viableQuantity : viableQuantities)
		{
			relatedParameterUIDs.insert(relatedParameterUIDs.begin(), viableQuantity->dependingParameterIds.begin(), viableQuantity->dependingParameterIds.end());
			ot::ValueComparisonDescription comparisonDescription = quantityDef;
			comparisonDescription.setName(viableQuantity->quantityName);
			TupleInstance tupleInstance = 	quantityDef.getTupleInstance();
			tupleInstance.setTupleTypeName(viableQuantity->m_tupleDescription.getTupleTypeName());
			comparisonDescription.setTupleInstance(tupleInstance);

			ot::QueryDescription queryDescription =  QueryDescriptionBuilder::create(comparisonDescription, viableQuantity);
			allQuantityQueries.push_back(queryDescription);			
		}
		m_dataLakeAccessor.setValueDescriptionsQuantities(allQuantityQueries);

		std::list<ot::QueryDescription> allRelatedParameterQueries;
		relatedParameterUIDs.sort();
		relatedParameterUIDs.unique();
		for (ot::UID parameterUID : relatedParameterUIDs)
		{
			const MetadataParameter* relatedParameter = m_resultCollectionMetadataAccess->findMetadataParameter(parameterUID);
			ot::ValueComparisonDescription comparisonDescription;
			ot::QueryDescription queryDescription = QueryDescriptionBuilder::create(comparisonDescription, relatedParameter);
			allRelatedParameterQueries.push_back(queryDescription);
		}
		m_dataLakeAccessor.addValueDescriptionsParameters(allRelatedParameterQueries);
	}
}

void BlockHandlerDatabaseAccess::createQueryDescriptionsParameter(EntityBlockDatabaseAccess* _blockEntity)
{
	std::list<ot::ValueComparisonDescription> queries =	_blockEntity->getAdditionalQueries();
	const auto& parametersByLabel =	m_resultCollectionMetadataAccess->getMetadataCampaign().getMetadataParameterByLabel();
	std::list<ot::QueryDescription> allParameterQueries;
	for (ot::ValueComparisonDescription& query : queries)
	{
		if (!query.getName().empty() && !query.getComparator().empty() && !query.getValue().empty())
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
				ot::QueryDescription queryDescription =	QueryDescriptionBuilder::create(query, parameterByLabel->second);
				allParameterQueries.push_back(queryDescription);
			}
		}
	}
	m_dataLakeAccessor.addValueDescriptionsParameters(allParameterQueries);
}

void BlockHandlerDatabaseAccess::createQueryDescriptionsSeries(EntityBlockDatabaseAccess* _blockEntity)
{
	const MetadataSeries* series = nullptr;
	const std::string seriesLabel = _blockEntity->getSeriesSelection()->getValue();

	//First we search the series by name, or if it failes we assume a regex expression and conduct another search.
	std::list<const MetadataSeries*> matchingSeries;
	if (seriesLabel != "")
	{
		series = m_resultCollectionMetadataAccess->findMetadataSeries(seriesLabel);
		if (series != nullptr)
		{
			matchingSeries.push_back(series);
		}
		else
		{
			std::list<std::string> allSeriesLabels = m_resultCollectionMetadataAccess->listAllSeriesNames();
			applyRegexFilter(allSeriesLabels, seriesLabel);
			for (const std::string& seriesName : allSeriesLabels)
			{
				const MetadataSeries* series = m_resultCollectionMetadataAccess->findMetadataSeries(seriesName);
				assert(series != nullptr);
				matchingSeries.push_back(series);
			}
		}
	}
	else
	{
		//If no label is selected, we take all series in consideration to allow a query that aligns with the current model state.
		const std::list<MetadataSeries>& allSeries = m_resultCollectionMetadataAccess->getMetadataCampaign().getSeriesMetadata();
		for (const MetadataSeries& series : allSeries)
		{
			matchingSeries.push_back(&series);
		}
	}

	//Now we apply metadata filter and sort out the list of series
	std::list<ot::ValueComparisonDescription> metadataQueries = _blockEntity->getMetadataQueries();
	auto oneSeriesIt = matchingSeries.begin();
	while (oneSeriesIt != matchingSeries.end())
	{
		const ot::JsonDocument& metadata = (*oneSeriesIt)->getMetadata();
		bool match = true;
		for (auto& metadataQuery : metadataQueries)
		{
			if (metadataQuery.getComparator() != "")
			{
				const std::string fieldName = metadataQuery.getName();
				const ot::JsonValue& fieldValue = MetadataVectorizer::getValue(metadata, fieldName);
				const std::string temp = ot::json::toJson(fieldValue);
				if (fieldValue.IsString())
				{
					if (metadataQuery.getComparator() != "")
					{
						throw std::exception(("Comparison of strings are only possible with an equality check. Field: " + metadataQuery.getName()).c_str());
					}
					else
					{
						const std::string actualSeriesMetadataValue = fieldValue.GetString();
						match &= actualSeriesMetadataValue != metadataQuery.getValue();
					}
				}
				else if (!fieldValue.IsObject() && !fieldValue.IsArray())
				{
					match &= compare(metadataQuery, fieldValue);
				}
				else
				{
					throw std::exception(("Metadata query targets an object, which is not yet supported. Field: " + metadataQuery.getName()).c_str());
				}
			}
			else
			{
				OT_LOG_W("Skipping metadata query since no comparator was selected. Field: " + metadataQuery.getName());
			}
			if (!match)
			{
				break;
			}
		}

		if (match)
		{
			//The series matches the metadata query. Check the next metadata query.
			oneSeriesIt++;
		}
		else
		{
			oneSeriesIt = matchingSeries.erase(oneSeriesIt);
		}

	}

	std::list<ot::QueryDescription> seriesQueries;
	for (const MetadataSeries* series : matchingSeries)
	{
		ot::QueryDescription queryDescription = QueryDescriptionBuilder::create(series);
		seriesQueries.push_back(queryDescription);
	}

	if (matchingSeries.size() == 1)
	{
		m_queriedData.setMetadataSeries(series);
	}

	m_dataLakeAccessor.setValueDescriptionsSeries(seriesQueries);
}

void BlockHandlerDatabaseAccess::collectMetadataForPipeline(EntityBlockDatabaseAccess* _blockEntity)
{
	const MetadataCampaign* campaign = &m_resultCollectionMetadataAccess->getMetadataCampaign();

	//Now we setup the datastream
	ot::Connector outputConnector = _blockEntity->getConnectorOutput();
	const std::string outputConnectorName = outputConnector.getConnectorName();
	m_queriedData.setMetadataCampaign(campaign);

	m_dataPerPort[outputConnectorName] = &m_queriedData;
}

void BlockHandlerDatabaseAccess::buildQuery(EntityBlockDatabaseAccess* _blockEntity)
{
	m_sortByID = _blockEntity->getReproducibleOrder();
	if (m_sortByID)
	{
		m_sort = (bsoncxx::builder::stream::document{}
			<< "_id" << 1  // 1 for ascending, -1 for descending
			<< bsoncxx::builder::stream::finalize);
	}
	
	createQueryDescriptionsParameter(_blockEntity);
	createQueryDescriptionsQuantities(_blockEntity);
	createQueryDescriptionsSeries(_blockEntity);

	collectMetadataForPipeline(_blockEntity);

	AdvancedQueryBuilder builder;
	std::vector<std::string> projectionNamesForExclusion{ "SchemaVersion", "SchemaType"};
	m_projection = builder.GenerateSelectQuery(projectionNamesForExclusion, false, false);
}

void BlockHandlerDatabaseAccess::applyRegexFilter(std::list<std::string>& _options, const std::string& _filter)
{
	std::regex pattern(_filter);

	auto option = _options.begin();
	while (option != _options.end())
	{
		if (!std::regex_match(*option, pattern)) 
		{
			option = _options.erase(option);
		}
		else
		{
			option++;
		}
	}	
}

bool BlockHandlerDatabaseAccess::compare(const ot::ValueComparisonDescription& _comparisonDef, const ot::JsonValue& _value)
{
	std::string type;
	if (_value.IsInt())
	{
		type = ot::TypeNames::getInt32TypeName();
	}
	else if (_value.IsInt64())
	{
		type = ot::TypeNames::getInt64TypeName();
	}
	else if (_value.IsFloat())
	{
		type = ot::TypeNames::getFloatTypeName();
	}
	else if (_value.IsDouble())
	{
		type = ot::TypeNames::getDoubleTypeName();
	}
	else if(_value.IsBool())
	{
		type = ot::TypeNames::getBoolTypeName();
	}
	else
	{
		throw std::exception("Not supported data type for comparison");
	}
	
	ot::ExplicitStringValueConverter converter;
	ot::Variable givenValue = converter.setValueFromString(_comparisonDef.getValue(), type);
	
	ot::JSONToVariableConverter converterJ;
	ot::Variable isValue = converterJ(_value);

	if (_comparisonDef.getComparator() == "=")
	{
		return givenValue == isValue;
	}
	else if (_comparisonDef.getComparator() == "<")
	{
		return givenValue < isValue;
	}
	else if (_comparisonDef.getComparator() == "<=")
	{
		return (givenValue < isValue) || (givenValue == isValue);
	}
	else if (_comparisonDef.getComparator() == ">")
	{
		return (givenValue > isValue);
	}
	else if (_comparisonDef.getComparator() == ">=")
	{
		return (givenValue > isValue) || (givenValue == isValue);
	}
	else if (_comparisonDef.getComparator() == "!=")
	{
		return !(givenValue == isValue);
	}
	else
	{
		throw std::exception(("Not supported operator for comparison: " + _comparisonDef.getComparator()).c_str());
	}
	return false;
}

std::string BlockHandlerDatabaseAccess::getBlockType() const
{
	return "Database Access Block";
}
