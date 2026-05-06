// @otlicense
// File: MetadataEntityInterface.cpp
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

// OpenTwin header
#include "OTCore/Variable/VariableListToStringListConverter.h"

#include "OTModelEntities/MetadataEntityInterface.h"
#include "OTCore/Tuple/TupleFactory.h"
#include "OTModelEntities/DataBase.h"
#include "OTModelEntities/EntityBase.h"
// std header
#include <vector>

MetadataCampaign MetadataEntityInterface::createCampaign(EntityMetadataCampaign* _rmd, std::list<EntityMetadataSeries*>& _allSeries)
{

	MetadataCampaign measurementCampaign;
	auto start = std::chrono::high_resolution_clock::now();
	
	auto camp = std::chrono::high_resolution_clock::now();
	for (EntityMetadataSeries* oneSeries : _allSeries)
	{
		MetadataSeries seriesMD = oneSeries->getSeries();
		measurementCampaign.addSeriesMetadata(std::move(seriesMD));
	}

	auto series = std::chrono::high_resolution_clock::now();
	measurementCampaign.updateMetadataOverview();
	auto update = std::chrono::high_resolution_clock::now();
	
	uint64_t totalTime = std::chrono::duration_cast<std::chrono::milliseconds>(update - start).count();
	uint64_t seriesTime = std::chrono::duration_cast<std::chrono::milliseconds>(camp - start).count();
	uint64_t campTime = std::chrono::duration_cast<std::chrono::milliseconds>(series- camp).count();
	uint64_t updateTime = std::chrono::duration_cast<std::chrono::milliseconds>(update - series).count();
	return measurementCampaign;
}

ot::NewModelStateInfo MetadataEntityInterface::storeCampaign(MetadataCampaign& _metaDataCampaign)
{
	if (EntityBase::getUidGenerator() == nullptr)
	{
		assert(false);
		return ot::NewModelStateInfo();
	}

	ot::UID entityID = EntityBase::getUidGenerator()->getUID();

	EntityMetadataCampaign entityCampaign(entityID, nullptr, nullptr, nullptr);
	entityCampaign.setCallbackData(this->getCallbackData());
	entityCampaign.setCampaign(_metaDataCampaign);
	entityCampaign.storeToDataBase();
	ot::NewModelStateInfo newEntity;
	newEntity.addTopologyEntity(entityCampaign);
	return newEntity;
}

ot::NewModelStateInfo MetadataEntityInterface::storeCampaign(MetadataCampaign& _metaDataCampaign, std::list<const MetadataSeries*>& _seriesMetadata, bool _saveModel)
{
	ot::NewModelStateInfo entInfosCamp = storeCampaign(_metaDataCampaign);
	ot::NewModelStateInfo entInfosSer = storeCampaign(_seriesMetadata, _saveModel);
	entInfosCamp.splice(entInfosSer);
	return entInfosCamp;
}

ot::NewModelStateInfo MetadataEntityInterface::storeCampaign(std::list<const MetadataSeries*>& _seriesMetadata, bool _saveModel)
{
	ot::NewModelStateInfo newEntitiesInfos;
	std::list< EntityMetadataSeries> entitiesMetadataSeries;
	for (auto& newSeriesMetadata : _seriesMetadata)
	{
		const std::string name = newSeriesMetadata->getName();
		EntityMetadataSeries entitySeries(newSeriesMetadata->getSeriesIndex(), nullptr, nullptr, nullptr);
		entitySeries.setName(name);
		entitySeries.setTreeItemEditable(true);
		entitySeries.setCallbackData(this->getCallbackData());
		entitySeries.setSeries(*newSeriesMetadata);
		
		entitySeries.storeToDataBase();
		newEntitiesInfos.addTopologyEntity(entitySeries);
	}
	return newEntitiesInfos;
}

std::list<ot::Variable> MetadataEntityInterface::convertFromStringVector(const std::vector<std::string> _values) const
{
	std::list<ot::Variable> output;
	for (const std::string& value : _values)
	{
		output.push_back(ot::Variable(value));
	}

	return output;
}

std::list<ot::Variable> MetadataEntityInterface::convertFromUInt32Vector(const std::vector<uint32_t> _values) const
{
	std::list<ot::Variable> output;
	for (const uint32_t& value : _values)
	{
		output.push_back(ot::Variable(static_cast<int32_t>(value)));
	}

	return output;
}

std::list<ot::Variable> MetadataEntityInterface::convertFromUInt64Vector(const std::vector<uint64_t> _values) const
{
	std::list<ot::Variable> output;
	for (const uint64_t& value : _values)
	{
		output.push_back(ot::Variable(static_cast<int64_t>(value)));
	}

	return output;
}
