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

	//! @param _data Must be created with the allocator of the m_data document
	void setData(ot::JsonValue&& _data)
	{
		ot::JsonValue copy(_data.Move(), m_data.GetAllocator());
		m_data.AddMember("Data", copy, m_data.GetAllocator());
	}

	ot::JsonValue& getData()
	{
		if (!ot::json::exists(m_data, "Data"))
		{
			ot::JsonValue empty;
			m_data.AddMember("data", empty, m_data.GetAllocator());
		}
		return m_data["Data"];
	}

	ot::JsonValue& getMetadata()
	{
		#define memberCampaign "Campaign"
		#define memberSeries "Series"
		#define memberQuantity "Quantity"
		#define memberQuantityDescription "QuantityDescription"

		if (!ot::json::exists(m_data, "Meta"))
		{
			ot::JsonObject empty;
			m_data.AddMember("Meta", empty, m_data.GetAllocator());
			m_data["Meta"].AddMember(memberCampaign, empty, m_data.GetAllocator());
			m_data["Meta"].AddMember(memberSeries, empty, m_data.GetAllocator());
			m_data["Meta"].AddMember(memberQuantity, empty, m_data.GetAllocator());
			m_data["Meta"].AddMember(memberQuantityDescription, empty, m_data.GetAllocator());
		}

		if (m_campaign != nullptr)
		{
			ot::JsonObject entry;
			m_campaign->addToJsonObject(entry, m_data.GetAllocator());
			m_data["Meta"].RemoveMember(memberCampaign);
			m_data["Meta"].AddMember(memberCampaign, entry, m_data.GetAllocator());
		}

		if (m_series != nullptr)
		{
			ot::JsonObject entry;
			m_series->addToJsonObject(entry, m_data.GetAllocator());
			m_data["Meta"].RemoveMember(memberSeries);
			m_data["Meta"].AddMember(memberSeries, entry, m_data.GetAllocator());
		}

		if (m_quantity != nullptr)
		{
			ot::JsonObject entry;
			m_quantity->addToJsonObject(entry, m_data.GetAllocator());
			m_data["Meta"].RemoveMember(memberQuantity);
			m_data["Meta"].AddMember(memberQuantity, entry, m_data.GetAllocator());
		}

		if (m_quantityDescription != nullptr)
		{
			ot::JsonObject entry;
			m_quantityDescription->addToJsonObject(entry, m_data.GetAllocator());
			m_data["Meta"].RemoveMember(memberQuantityDescription);
			m_data["Meta"].AddMember(memberQuantityDescription, entry, m_data.GetAllocator());
		}

		return m_data["Meta"];
	}

	void setMetadata(ot::JsonValue&& _metaData)
	{
		if (ot::json::exists(_metaData, "Campaign"))
		{
			std::unique_ptr<MetadataCampaign> newCampaign(new MetadataCampaign());
			newCampaign->setFromJsonObject(ot::json::getObject(_metaData, "Campaign"));
			m_campaign = newCampaign.release();
		}

		if (ot::json::exists(_metaData, "Series"))
		{
			std::unique_ptr<MetadataSeries> newSeries(new MetadataSeries());
			newSeries->setFromJsonObject(ot::json::getObject(_metaData, "Series"));
			m_series = newSeries.release();
		}

		if (ot::json::exists(_metaData, "Quantity"))
		{
			std::unique_ptr<MetadataQuantity> newQuantity(new MetadataQuantity());
			newQuantity->setFromJsonObject(ot::json::getObject(_metaData, "Quantity"));
			m_quantity = newQuantity.release();
		}

		if (ot::json::exists(_metaData, "QuantityDescription"))
		{
			std::unique_ptr<MetadataQuantityValueDescription> newValueDescript(new MetadataQuantityValueDescription());
			newValueDescript->setFromJsonObject(ot::json::getObject(_metaData, "QuantityDescription"));
			m_quantityDescription = newValueDescript.release();
		}
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
