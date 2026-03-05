// @otlicense
// File: BlockHandlerDatabaseAccess.h
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

#pragma once
#include "BlockHandler.h"
#include "OTResultDataAccess/MetadataHandle/MetadataCampaign.h"
#include "OTResultDataAccess/MetadataHandle/MetadataParameter.h"
#include "OTResultDataAccess/ResultCollection/ResultCollectionMetadataAccess.h"
#include "OTDataStorage/AdvancedQueryBuilder.h"
#include "OTCore/StringToVariableConverter.h"
#include "OTBlockEntities/Pipeline/EntityBlockDatabaseAccess.h"
#include "OTDataStorage/DataLakeAPI.h"

class BlockHandlerDatabaseAccess : public BlockHandler
{
public:
	BlockHandlerDatabaseAccess(EntityBlockDatabaseAccess* blockEntity, const HandlerMap& handlerMap);
	~BlockHandlerDatabaseAccess();

	bool executeSpecialized() override;	
	
	std::string getBlockType() const override;
private:
	struct LabelFieldNamePair
	{
		std::string m_label; //Comes from the campaign
		std::string m_fieldName;
	};
	
	DataStorageAPI::DataLakeAPI* m_resultCollectionAccess = nullptr;
	ResultCollectionMetadataAccess* m_resultCollectionMetadataAccess = nullptr;

	std::list< BsonViewOrValue> m_comparisons;
	BsonViewOrValue m_query;
	BsonViewOrValue m_projection;
	BsonViewOrValue m_sort;
	bool m_sortByID = false;
	const int m_documentLimit = 0;

	std::list<LabelFieldNamePair> m_labelFieldNamePairsParameter;
	std::list<LabelFieldNamePair> m_labelFieldNamePairsQuantities;
	std::list<LabelFieldNamePair> m_labelFieldNamePairsSeries;
	PipelineData m_queriedData;

	void collectMetadataForPipeline(EntityBlockDatabaseAccess* _blockEntity);
	void createLabelFieldNameMap();
	void buildQuery(EntityBlockDatabaseAccess* _blockEntity);
	const MetadataSeries* addSeriesQuery(EntityBlockDatabaseAccess* _blockEntity);
	void addQuantityQuery(EntityBlockDatabaseAccess* _blockEntity);
	void addParameterQueries(EntityBlockDatabaseAccess* _blockEntity);
		
	void addComparision(const ot::ValueComparisonDescription& _definition);

	void applyRegexFilter(std::list<std::string>& _options, const std::string& _filter);
	bool compare(const ot::ValueComparisonDescription& _comparisionDef, const ot::JsonValue& _value);
};
