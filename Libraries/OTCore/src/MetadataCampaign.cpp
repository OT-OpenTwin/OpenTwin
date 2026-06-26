// @otlicense
// File: MetadataCampaign.cpp
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
#include "OTCore/MetadataHandle/MetadataCampaign.h"
#include "OTCore/MetadataHandle/Helper.h"
#include "OTCore/Logging/Logger.h"
// std header
#include <cassert>

MetadataCampaign::MetadataCampaign(const MetadataCampaign& _other)
	:m_seriesMetadata(_other.m_seriesMetadata),
	m_quantityOverviewByUID(_other.m_quantityOverviewByUID),
	m_parameterOverviewByUID(_other.m_parameterOverviewByUID)
{ 

	for (auto& entry : m_quantityOverviewByUID)
	{
		m_quantityOverviewByLabel[entry.second.quantityLabel] = &(entry.second);
	}
	
	for (auto& entry : m_parameterOverviewByUID)
	{
		m_parameterOverviewByLabel[entry.second.parameterLabel] = &(entry.second);
	}

	m_metaData.CopyFrom(_other.m_metaData, m_metaData.GetAllocator());
}	 

MetadataCampaign& MetadataCampaign::operator=(const MetadataCampaign& _other)
{
	MetadataCampaign temp(_other);
	swap(*this, temp);
	return *this;
}

MetadataCampaign::MetadataCampaign(MetadataCampaign&& _other) noexcept
{
	swap(*this, _other);
}

MetadataCampaign& MetadataCampaign::operator=(MetadataCampaign&& _other) noexcept
{
	MetadataCampaign temp;
	swap(*this, _other);
	return *this;
}

void MetadataCampaign::swap(MetadataCampaign& _a, MetadataCampaign& _b)
{
	std::swap(_a.m_seriesMetadata,_b.m_seriesMetadata);
	std::swap(_a.m_quantityOverviewByUID,_b.m_quantityOverviewByUID);
	std::swap(_a.m_parameterOverviewByUID,_b.m_parameterOverviewByUID);
	std::swap(_a.m_quantityOverviewByLabel,_b.m_quantityOverviewByLabel);
	std::swap(_a.m_parameterOverviewByLabel,_b.m_parameterOverviewByLabel);
	std::swap(_a.m_campaignName,_b.m_campaignName);
	m_metaData.Swap(_b.m_metaData);
}

void MetadataCampaign::updateMetadataOverview()
{
	for (auto& seriesMetadata : m_seriesMetadata)
	{
		updateMetadataOverview(seriesMetadata);
	}

	for (auto& entry : m_parameterOverviewByUID)
	{
		entry.second.values.sort();
		entry.second.values.unique();
	}
}

void MetadataCampaign::updateMetadataOverviewFromLastAddedSeries()
{
	updateMetadataOverview(m_seriesMetadata.back());
}

void MetadataCampaign::updateMetadataOverview(MetadataSeries& seriesMetadata)
{
	//First we are adding the parameter of the series to the parameter overview of the entire campaign
	const auto& allParameter = seriesMetadata.getParameter();
	for (const auto& parameter : allParameter)
	{
		auto existingParameterEntry = m_parameterOverviewByUID.find(parameter.parameterUID);
		if (existingParameterEntry == m_parameterOverviewByUID.end())
		{
			m_parameterOverviewByUID[parameter.parameterUID] = parameter;
			m_parameterOverviewByLabel[parameter.parameterLabel] = &m_parameterOverviewByUID[parameter.parameterUID];
		}
		else
		{
			//If the parameter was already added, we extend the list of possible values
			std::list<ot::Variable>& values = existingParameterEntry->second.values;
			values.insert(values.begin(), parameter.values.begin(), parameter.values.end());
		}
	}

	const std::list<MetadataQuantity>& allQuantities = seriesMetadata.getQuantities();
	auto& temp = const_cast<std::list<MetadataQuantity>&>(allQuantities);
	for (MetadataQuantity& quantity : temp)
	{
		assert(quantity.quantityIndex != 0);
		auto existingQuantityEntry = m_quantityOverviewByUID.find(quantity.quantityIndex);
		if (existingQuantityEntry == m_quantityOverviewByUID.end())
		{
			m_quantityOverviewByUID[quantity.quantityIndex] = quantity;
			m_quantityOverviewByLabel[quantity.quantityLabel] = &m_quantityOverviewByUID[quantity.quantityIndex];
		}
		else
		{
			//assert(0); //Each quantity should only be added once.
		}
		
		MetadataQuantity& quantityInOverview = m_quantityOverviewByUID[quantity.quantityIndex];
		for (ot::UID parameterID : quantity.dependingParameterIds)
		{
			auto dependingParameter = m_parameterOverviewByUID.find(parameterID);
			if (dependingParameter != m_parameterOverviewByUID.end())
			{
				if (std::find(quantity.dependingParameterLabels.begin(), quantity.dependingParameterLabels.end(), dependingParameter->second.parameterLabel) == quantity.dependingParameterLabels.end())
				{
					quantity.dependingParameterLabels.push_back(dependingParameter->second.parameterLabel);
				}
				
				if (std::find(quantityInOverview.dependingParameterLabels.begin(), quantityInOverview.dependingParameterLabels.end(), dependingParameter->second.parameterLabel) == quantityInOverview.dependingParameterLabels.end())
				{
					quantityInOverview.dependingParameterLabels.push_back(dependingParameter->second.parameterLabel);
				}
			}
		}
	}
}

std::map<std::string, MetadataParameter*> MetadataCampaign::extractParameter()
{
	std::map<std::string, MetadataParameter*> pipelineCampaignParametersByLabel;
	for (const MetadataSeries& pipelineSeries : getSeriesMetadata())
	{
		for (const MetadataParameter& pipelineParameter : pipelineSeries.getParameter())
		{
			auto pipelineCampaignParameterByLabel = pipelineCampaignParametersByLabel.find(pipelineParameter.parameterLabel);
			if (pipelineCampaignParameterByLabel == pipelineCampaignParametersByLabel.end())
			{
				pipelineCampaignParametersByLabel[pipelineParameter.parameterLabel] = const_cast<MetadataParameter*>(&pipelineParameter);
			}
			else
			{
				//Label should be unique
				if (*pipelineCampaignParameterByLabel->second != pipelineParameter)
				{
					throw std::exception(("Incosistent parameter with the same label detected: " + pipelineParameter.parameterLabel + ". Metadata throughout the entire campaign must have the same characteristics, if they have the same label.").c_str());
				}
			}
		}
	}

	return pipelineCampaignParametersByLabel;
}

std::map<std::string, MetadataQuantity*> MetadataCampaign::extractQuantity()
{
	std::map<std::string, MetadataQuantity*> pipelineCampaignQuantitiesByLabel;

	for (const MetadataSeries& pipelineSeries : getSeriesMetadata())
	{

		for (const MetadataQuantity& pipelineQuantity : pipelineSeries.getQuantities())
		{
			auto pipelineCampaignQuantityByLabel = pipelineCampaignQuantitiesByLabel.find(pipelineQuantity.quantityLabel);
			if (pipelineCampaignQuantityByLabel == pipelineCampaignQuantitiesByLabel.end())
			{
				pipelineCampaignQuantitiesByLabel[pipelineQuantity.quantityLabel] = const_cast<MetadataQuantity*>(&pipelineQuantity);
			}
			else
			{
				//Label should be unique
				if (*pipelineCampaignQuantityByLabel->second != pipelineQuantity)
				{
					throw std::exception(("Incosistent quantity with the same label detected: " + pipelineQuantity.quantityLabel + ". Metadata throughout the entire campaign must have the same characteristics, if they have the same label.").c_str());
				}
			}
		}
	}
	return pipelineCampaignQuantitiesByLabel;
}


void MetadataCampaign::reset()
{
	m_seriesMetadata.clear();
	m_quantityOverviewByLabel.clear();
	m_parameterOverviewByLabel.clear();
}

void MetadataCampaign::setFromJsonObject(const ot::ConstJsonObject& _object)
{
	m_campaignName = ot::json::getString(_object, "Name");

	ot::ConstJsonArray allSeries = ot::json::getArray(_object, "series");
	for (rapidjson::SizeType i = 0; i < allSeries.Size(); i++)
	{
		MetadataSeries series;
		series.setFromJsonObject(ot::json::getObject(allSeries, i));
		m_seriesMetadata.push_back(series);
	}
	m_quantityOverviewByLabel =	extractQuantity();
	m_parameterOverviewByLabel = extractParameter();
}

void MetadataCampaign::addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const
{

	_object.AddMember("Name", ot::JsonString(m_campaignName, _allocator), _allocator);

	ot::JsonArray allSeries;
	for (const MetadataSeries& series : m_seriesMetadata)
	{
		ot::JsonObject object;
		series.addToJsonObject(object, _allocator);
		allSeries.PushBack(object, _allocator);
	}
	_object.AddMember("series", allSeries, _allocator);
}

size_t MetadataCampaign::getMemSize()
{
	size_t total = sizeof(MetadataCampaign);

	// --- std::string m_campaignName ---
	total += ot::stringHeapSize(m_campaignName);

	// --- std::list<MetadataSeries> m_seriesMetadata ---
	size_t seriesMemPart = 0;
	for (MetadataSeries& s : m_seriesMetadata)
	{
		seriesMemPart += s.getMemSize();        // full object footprint incl. sizeof(MetadataSeries)
		seriesMemPart += 2 * sizeof(void*);     // list node prev/next pointers
	}

	double seriesMemPartMB = seriesMemPart / (1024 * 1024);
	OT_USER_LOG_I("Series Mem in MB: " + std::to_string(seriesMemPartMB));
	total += seriesMemPart;

	// --- std::map<ot::UID, MetadataQuantity> m_quantityOverviewByUID ---
	// Each std::map node is a heap-allocated red-black tree node containing:
	//   - the key/value pair (std::pair<const ot::UID, MetadataQuantity>)
	//   - three pointers (left, right, parent) + one color bit (typically padded to pointer size)
	constexpr size_t mapNodeOverhead = 3 * sizeof(void*) + sizeof(void*); // 4 words typical
	for (auto& [uid, quantity] : m_quantityOverviewByUID)
	{
		total += quantity.getMemSize();  // full object footprint incl. sizeof(MetadataQuantity)
		total += mapNodeOverhead;
	}

	// --- std::map<ot::UID, MetadataParameter> m_parameterOverviewByUID ---
	for (auto& [uid, parameter] : m_parameterOverviewByUID)
	{
		total += parameter.getMemSize(); // full object footprint incl. sizeof(MetadataParameter)
		total += mapNodeOverhead;
	}

	// --- std::map<std::string, MetadataQuantity*> m_quantityOverviewByLabel ---
	// These maps hold NON-OWNING pointers into m_quantityOverviewByUID.
	// Count only the map node structure + the key string heap cost.
	// Do NOT count the pointed-to MetadataQuantity objects (already counted above).
	for (auto& [label, ptr] : m_quantityOverviewByLabel)
	{
		total += ot::stringHeapSize(label);  // std::string key heap allocation
		total += mapNodeOverhead;        // tree node overhead
		total += sizeof(MetadataQuantity*); // the pointer value itself (in the node payload)
	}

	// --- std::map<std::string, MetadataParameter*> m_parameterOverviewByLabel ---
	for (const auto& [label, ptr] : m_parameterOverviewByLabel)
	{
		total += ot::stringHeapSize(label);
		total += mapNodeOverhead;
		total += sizeof(MetadataParameter*);
	}

	// --- ot::JsonDocument m_metaData ---
	total += m_metaData.GetAllocator().Capacity();

	return total;
}
