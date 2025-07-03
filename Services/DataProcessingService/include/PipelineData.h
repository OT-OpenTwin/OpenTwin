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
		:m_campaign(_other.m_campaign),m_series(_other.m_series), m_quantity(_other.m_quantity), m_quantityDescription(_other.m_quantityDescription)
	{
		const ot::JsonDocument& source = _other.m_data;
		m_data.CopyFrom(source, m_data.GetAllocator());
	}

	PipelineData(PipelineData&& _other) noexcept
		:m_campaign(_other.m_campaign), m_series(_other.m_series), m_quantity(_other.m_quantity), m_quantityDescription(_other.m_quantityDescription)
	{
		m_campaign = (_other.m_campaign);
		m_series = (_other.m_series);
		m_quantity = (_other.m_quantity);
		m_quantityDescription = (_other.m_quantityDescription);

		ot::JsonDocument& source = _other.m_data;
		m_data.Swap(source);
	}

	PipelineData& operator=(const PipelineData& _other)
	{
		m_campaign = (_other.m_campaign);
		m_series= (_other.m_series); 
		m_quantity = (_other.m_quantity);
		m_quantityDescription = (_other.m_quantityDescription);

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
