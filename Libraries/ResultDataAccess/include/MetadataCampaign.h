// @otlicense
// File: MetadataCampaign.h
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
#include <map>
#include <string>
#include <memory>

#include "MetadataSeries.h"
#include "MetadataQuantity.h"
#include "MetadataParameter.h"
#include "MetadataEntry.h"
#include "OTCore/CoreTypes.h"

#include "OTCore/Serializable.h"

class __declspec(dllexport) MetadataCampaign : public ot::Serializable
{
public:
	MetadataCampaign() = default;
	MetadataCampaign(const MetadataCampaign& _other) = default;
	MetadataCampaign& operator=(const MetadataCampaign& _other) = delete;
	MetadataCampaign(MetadataCampaign&& other) noexcept =default;
	MetadataCampaign& operator=(MetadataCampaign&& other) noexcept = default;
	~MetadataCampaign() {};

	void addMetaInformation(const std::string& key, std::shared_ptr<MetadataEntry> _metadata) { m_metaData[key] = _metadata; }
	const std::map <std::string, std::shared_ptr<MetadataEntry>>&	getMetaData() const { return m_metaData; }
	
	void addSeriesMetadata(MetadataSeries&& seriesMetadata) { m_seriesMetadata.push_back(seriesMetadata); }
	const std::list<MetadataSeries>& getSeriesMetadata() const { return m_seriesMetadata; };

	void setCampaignName(const std::string _name) { m_campaignName = _name; }
	const std::string& getCampaignName()const { return m_campaignName; }
	
	const std::map <std::string, MetadataQuantity*>& getMetadataQuantitiesByLabel() const { return m_quantityOverviewByLabel; }
	const std::map <std::string, MetadataParameter*>& getMetadataParameterByLabel() const { return m_parameterOverviewByLabel; }
	const std::map <ot::UID, MetadataParameter>& getMetadataParameterByUID() const { return m_parameterOverviewByUID; }
	const std::map <ot::UID, MetadataQuantity>& getMetadataQuantitiesByUID() const { return m_quantityOverviewByUID; }
	
	void updateMetadataOverview();
	void updateMetadataOverviewFromLastAddedSeries();
	
	void reset();

	virtual void addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const;
	virtual void setFromJsonObject(const ot::ConstJsonObject& _object);
	

private:
	std::list<MetadataSeries> m_seriesMetadata;
	
	std::map<ot::UID,MetadataQuantity> m_quantityOverviewByUID;
	std::map<ot::UID, MetadataParameter> m_parameterOverviewByUID;
	std::map<std::string, MetadataQuantity*> m_quantityOverviewByLabel;
	std::map<std::string, MetadataParameter*> m_parameterOverviewByLabel;

	std::string m_campaignName;
	
	std::map <std::string, std::shared_ptr<MetadataEntry>> m_metaData;

	void updateMetadataOverview(MetadataSeries& _series);

	std::map<std::string, MetadataParameter*> extractParameter();
	std::map<std::string, MetadataQuantity*> extractQuantity();
};

