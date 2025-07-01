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
	//! @param _data Must be created with the allocator of the m_data document
	void setData(ot::JsonValue&& _data)
	{
		ot::JsonValue copy(_data.Move(), m_data.GetAllocator());
		m_data.AddMember("Data", copy,m_data.GetAllocator());
	}

	ot::JsonValue& getData()
	{
		if (ot::json::exists(m_data, "Data"))
		{
			return m_data["Data"];
		}
		else
		{
			return m_data;
		}
	}

	PipelineData() = default;
	~PipelineData() = default;

	PipelineData(const PipelineData& _other)
	{
		const ot::JsonDocument& source = _other.m_data;
		m_data.CopyFrom(source, m_data.GetAllocator());
	}

	PipelineData(PipelineData&& _other) noexcept
	{
		ot::JsonDocument& source = _other.m_data;
		m_data.Swap(source);
	}

	PipelineData& operator=(const PipelineData& _other)
	{
		const ot::JsonDocument& source = _other.m_data;
		m_data.CopyFrom(source, m_data.GetAllocator());
	}

	PipelineData& operator=(PipelineData&& _other) noexcept
	{
		ot::JsonDocument& source = _other.m_data;
		m_data.Swap(source);
		return *this;
	}

	void setMetadataCampaign(const MetadataCampaign* _campaign) { m_campaign = _campaign;}
	void setMetadataSeries(const MetadataSeries* _series) { m_series = _series;}
	void setMetadataQuantity(const MetadataQuantity* _quantity, const MetadataQuantityValueDescription* _quantityDescription) 
	{ 
		m_quantity = _quantity; 
		m_quantityDescription = _quantityDescription;
	}
	const MetadataCampaign* getMetadataCampaign() { return m_campaign; }
	const MetadataSeries* getMetadataSeries() { return m_series; }
	const MetadataQuantity* getMetadataQuantity() { return m_quantity; }
	const MetadataQuantityValueDescription* getMetadataValueDescription() { return m_quantityDescription; }


private:
	const MetadataCampaign* m_campaign = nullptr;
	const MetadataSeries* m_series = nullptr;
	const MetadataQuantity* m_quantity = nullptr;
	const MetadataQuantityValueDescription* m_quantityDescription = nullptr;

	ot::JsonDocument m_data;
};
