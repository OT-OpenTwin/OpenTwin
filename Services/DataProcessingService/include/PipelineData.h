#pragma once

#include "DataDocument.h"

#include "MetadataCampaign.h"
#include "MetadataSeries.h"
#include "MetadataParameter.h"
#include "MetadataQuantity.h"


#include <memory>
#include <map>

struct PipelineData
{
	const MetadataCampaign* m_campaign = nullptr;
	const MetadataSeries* m_series = nullptr;
	const MetadataQuantity* m_quantity = nullptr;
	const MetadataQuantityValueDescription* m_quantityDescription = nullptr;

	//! @param _data Must be created with the allocator of the m_data document
	void setData(ot::JsonArray&& _data)
	{
		m_data.AddMember("Data", std::move(_data), m_data.GetAllocator());
	}

	ot::ConstJsonArray getData()
	{
		return ot::json::getArray(m_data, "Data");
	}

	PipelineData& operator=(const PipelineData& _pipeline)
	{
		const ot::JsonDocument& source = _pipeline.m_data;
		m_data.CopyFrom(source, m_data.GetAllocator());
		return *this;
	}

	ot::JsonDocument m_data;
};
