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
#include "BlockHandlerDatabaseAccess.h"
#include <mongocxx/options/find.hpp>
// OpenTwin header
#include "OTCore/String.h"
#include "OTCore/TimeFormatter.h"

#include "OTCore/DataStruct/GenericDataStructMatrix.h"
#include "OTCore/DataStruct/GenericDataStructSingle.h"
#include "OTCore/QueryDescription/ValueComparisonDescription.h"
#include "OTCore/Tuple/TupleDescriptionComplex.h"
#include "OTCore/Variable/ExplicitStringValueConverter.h"
#include "OTCore/Variable/JSONToVariableConverter.h"
#include "OTDataStorage/DataLakeHelper.h"
#include "OTDataStorage/AdvancedQueryBuilder.h"
#include "OTResultDataAccess/QuantityContainer.h"

// std header
#include <regex>
#include <algorithm>

BlockHandlerDatabaseAccess::BlockHandlerDatabaseAccess(EntityBlockDatabaseAccess* blockEntity, const HandlerMap& handlerMap)
	: BlockHandler(blockEntity, handlerMap)
{
	//First get handler of the selected project result data.
	m_accessConfig =blockEntity->getDataLakeAccessCfg();

	if (m_accessConfig.getQueriesByCollection().size() == 0 || m_accessConfig.getCollectionName().empty())
	{
		throw std::exception("No valid queries to execute");
	}
	std::pair<bool, uint32_t> resultLimit = blockEntity->getResultLimit();
	if (resultLimit.first)
	{
		m_documentLimit = resultLimit.second;
	}
	std::string collectionName;
	
	collectMetadataForPipeline(blockEntity);

	m_sortByID = blockEntity->getReproducibleOrder();
	if (m_sortByID)
	{
		m_sort = (bsoncxx::builder::stream::document{}
			<< "_id" << 1  // 1 for ascending, -1 for descending
			<< bsoncxx::builder::stream::finalize);
	}
}

bool BlockHandlerDatabaseAccess::executeSpecialized()
{	
	//SolverReport::instance().addToContent("Executing projection: " + debugProjection + "\n");
	mongocxx::options::find options;
	options.projection(m_projection);
	options.limit(m_documentLimit);
	SolverReport::instance().addToContent("Query limit: " + std::to_string( m_documentLimit )+ "\n");
	SolverReport::instance().addToContent("Sorting by _id: " + std::to_string(m_sortByID) + "\n");


	if (m_sortByID)
	{
		options.sort(m_sort);
		bsoncxx::builder::basic::document hintDoc;
		//Here we switched to a compound index. Bad style, but for now it works.
		mongocxx::v_noabi::hint hint = DataLakeHelper::getDefaultIndexHint();
		SolverReport::instance().addToContent("Hint: " + bsoncxx::to_json(hintDoc) + "\n");
		options.hint(hint);
	}
	DataLakeHelper dataLakeHelper;
	std::string log;
	auto startTime = std::chrono::high_resolution_clock::now();
	ot::JsonDocument result = dataLakeHelper.executeQuery(m_accessConfig, options, log);
	auto endTime = std::chrono::high_resolution_clock::now();
	m_queriedData.setData(std::move(result));
	return true;

}


void BlockHandlerDatabaseAccess::collectMetadataForPipeline(EntityBlockDatabaseAccess* _blockEntity)
{
	const std::string targetCollectionName = m_accessConfig.getCollectionName();
	const std::string thisCollectionNme = Application::instance()->getCollectionName();
	if (!targetCollectionName.empty())
	{
		ResultCollectionMetadataAccess metadataAccess(targetCollectionName, Application::instance()->getModelComponent(), targetCollectionName!= thisCollectionNme);
		m_campaign = metadataAccess.getMetadataCampaign();
	}
	else
	{
		assert(false); 
	}
	

	////Now we setup the datastream
	ot::Connector outputConnector = _blockEntity->getConnectorOutput();
	const std::string outputConnectorName = outputConnector.getConnectorName();
	m_queriedData.setMetadataCampaign(&m_campaign);
	m_dataPerPort[outputConnectorName] = &m_queriedData;
}

std::string BlockHandlerDatabaseAccess::getBlockType() const
{
	return "Database Access Block";
}
