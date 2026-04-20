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
#include "OTCore/MetadataHandle/MetadataCampaign.h"
#include "OTCore/MetadataHandle/MetadataParameter.h"
#include "OTResultDataAccess/ResultCollection/ResultCollectionMetadataAccess.h"
#include "OTDataStorage/AdvancedQueryBuilder.h"
#include "OTCore/Variable/StringToVariableConverter.h"
#include "OTBlockEntities/Pipeline/EntityBlockDatabaseAccess.h"
#include "OTCore/QueryDescription/QueryDescription.h"
#include "OTResultDataAccess/DataLakeAccessor.h"

class BlockHandlerDatabaseAccess : public BlockHandler
{
public:
	BlockHandlerDatabaseAccess(EntityBlockDatabaseAccess* blockEntity, const HandlerMap& handlerMap);
	
	bool executeSpecialized() override;	
	
	std::string getBlockType() const override;
private:
	ot::DataLakeAccessCfg m_accessConfig;
	MetadataCampaign m_campaign;

	BsonViewOrValue m_projection;
	BsonViewOrValue m_sort;
	bool m_sortByID = false;
	uint32_t m_documentLimit = 0;

	PipelineData m_queriedData;

	void collectMetadataForPipeline(EntityBlockDatabaseAccess* _blockEntity);

};
