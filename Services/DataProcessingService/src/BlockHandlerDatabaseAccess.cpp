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

std::string BlockHandlerDatabaseAccess::getBlockType() const
{
	return "Database Access Block";
}
