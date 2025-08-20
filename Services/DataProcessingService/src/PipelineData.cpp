#include "PipelineData.h"

#define memberCampaign "Selected Campaign"
#define memberSeries "Selected Series label"
#define memberData "Data"
#define memberMetaData "Meta"

void PipelineData::setData(ot::JsonValue&& _data)
{
	ot::JsonValue copy(_data.Move(), m_data.GetAllocator());
	m_data.AddMember(memberData, copy, m_data.GetAllocator());
}

ot::JsonValue& PipelineData::getData()
{
	if (!ot::json::exists(m_data, memberData))
	{
		ot::JsonValue empty;
		m_data.AddMember(memberData, empty, m_data.GetAllocator());
	}
	return m_data[memberData];
}

ot::JsonValue& PipelineData::getMetadata()
{
	if (!ot::json::exists(m_data, memberMetaData))
	{
		ot::JsonObject empty;
		m_data.AddMember(memberMetaData, empty, m_data.GetAllocator());
		m_data[memberMetaData].AddMember(memberCampaign, empty, m_data.GetAllocator());
		m_data[memberMetaData].AddMember(memberSeries, empty, m_data.GetAllocator());
	}

	if (m_campaign != nullptr)
	{
		ot::JsonObject entry;
		m_campaign->addToJsonObject(entry, m_data.GetAllocator());
		m_data[memberMetaData].RemoveMember(memberCampaign);
		m_data[memberMetaData].AddMember(memberCampaign, entry, m_data.GetAllocator());
	}

	if (m_series != nullptr)
	{
		
		m_data[memberMetaData].RemoveMember(memberSeries);
		m_data[memberMetaData].AddMember(memberSeries, ot::JsonString(m_series->getLabel(),m_data.GetAllocator()), m_data.GetAllocator());
	}
	return m_data[memberMetaData];
}

void PipelineData::setMetadata(ot::JsonValue&& _metaData)
{
	if (ot::json::exists(_metaData, memberCampaign))
	{
		const std::string temp = ot::json::toJson(_metaData);
		std::unique_ptr<MetadataCampaign> newCampaign(new MetadataCampaign());
		newCampaign->setFromJsonObject(ot::json::getObject(_metaData, memberCampaign));
		m_campaign = newCampaign.release();
	}

	if (ot::json::exists(_metaData, memberSeries))
	{
		if (m_campaign != nullptr)
		{
			if (ot::json::isString(_metaData, memberSeries))
			{
				const std::string seriesLabel = ot::json::getString(_metaData, memberSeries);
				const std::list<MetadataSeries>& allSeries =  m_campaign->getSeriesMetadata();
				for (auto& series : allSeries)
				{
					if (series.getLabel() == seriesLabel)
					{
						m_series = &series;
					}
				}
			}
		}
		else
		{
			assert(false); // The campaign should always be set.
		}
	}

}
