// @otlicense
// File: PipelineData.h
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

#include "DataDocument.h"

#include "MetadataCampaign.h"
#include "MetadataSeries.h"
#include "MetadataParameter.h"
#include "MetadataQuantity.h"


#include <memory>
#include <map>

class PipelineData
{
public:
	PipelineData() = default;
	~PipelineData() = default;

	PipelineData(const PipelineData& _other)
		:m_campaign(_other.m_campaign),m_series(_other.m_series)
	{
		const ot::JsonDocument& source = _other.m_data;
		m_data.CopyFrom(source, m_data.GetAllocator());
	}

	PipelineData(PipelineData&& _other) noexcept
		:m_campaign(_other.m_campaign), m_series(_other.m_series)
	{
		m_campaign = (_other.m_campaign);
		m_series = (_other.m_series);

		ot::JsonDocument& source = _other.m_data;
		m_data.Swap(source);
	}

	PipelineData& operator=(const PipelineData& _other)
	{
		m_campaign = (_other.m_campaign);
		m_series= (_other.m_series); 

		const ot::JsonDocument& source = _other.m_data;
		m_data.CopyFrom(source, m_data.GetAllocator());
	}

	PipelineData& operator=(PipelineData&& _other) noexcept
	{
		ot::JsonDocument& source = _other.m_data;
		m_data.Swap(source);
		return *this;
	}

	//! @param _data Must be created with the allocator of the m_data document
	void setData(ot::JsonValue&& _data);

	ot::JsonValue& getData();

	ot::JsonValue& getMetadata();

	void setMetadata(ot::JsonValue&& _metaData);

	void setMetadataCampaign(const MetadataCampaign* _campaign) { m_campaign = _campaign;}
	void setMetadataSeries(const MetadataSeries* _series) { m_series = _series;}
	
	const MetadataCampaign* getMetadataCampaign() { return m_campaign; }
	const MetadataSeries* getMetadataSeries() { return m_series; }

private:
	const MetadataCampaign* m_campaign = nullptr;
	const MetadataSeries* m_series = nullptr;
	ot::JsonDocument m_data;
};
