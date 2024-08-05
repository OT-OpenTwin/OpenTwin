#include "MetadataCampaign.h"
#include <cassert>
MetadataCampaign::MetadataCampaign(MetadataCampaign&& other) noexcept
	:m_seriesMetadata(std::move(other.m_seriesMetadata)),
	m_quantityOverviewByLabel(std::move(other.m_quantityOverviewByLabel)),
	m_parameterOverviewByLabel(std::move(other.m_parameterOverviewByLabel)),
	m_metaData(std::move(other.m_metaData)),
	m_campaignName(other.m_campaignName)
{}

MetadataCampaign::MetadataCampaign(const MetadataCampaign& other)
	:m_seriesMetadata(other.m_seriesMetadata),
	m_quantityOverviewByLabel(other.m_quantityOverviewByLabel),
	m_parameterOverviewByLabel(other.m_parameterOverviewByLabel),
	m_metaData(other.m_metaData),
	m_campaignName(other.m_campaignName)
{}

MetadataCampaign& MetadataCampaign::operator=(MetadataCampaign&& other) noexcept
{
	m_seriesMetadata = std::move(other.m_seriesMetadata);
	m_quantityOverviewByLabel = std::move(other.m_quantityOverviewByLabel);
	m_parameterOverviewByLabel = std::move(other.m_parameterOverviewByLabel);
	m_metaData = std::move(other.m_metaData);
	m_campaignName = std::move(other.m_campaignName);
	return *this;
}

//MetadataCampaign& MetadataCampaign::operator=(const MetadataCampaign& other)
//{
//	_seriesMetadata = other._seriesMetadata;
//	_quantityOverviewByName = other._quantityOverviewByName;
//	_parameterOverviewByName = other._parameterOverviewByName;
//	_metaData = other._metaData;
//	_campaignName = other._campaignName;
//	return *this;
//}


void MetadataCampaign::updateMetadataOverview()
{
	for (auto& seriesMetadata : m_seriesMetadata)
	{
		updateMetadataOverview(seriesMetadata);
	}
}

void MetadataCampaign::updateMetadataOverviewFromLastAddedSeries()
{
	updateMetadataOverview(m_seriesMetadata.back());
}

void MetadataCampaign::updateMetadataOverview(MetadataSeries& seriesMetadata)
{
	auto allParameter = seriesMetadata.getParameter();
	for (auto& parameter : allParameter)
	{
		auto existingParameterEntry = m_parameterOverviewByUID.find(parameter.parameterUID);
		if (existingParameterEntry == m_parameterOverviewByUID.end())
		{
			m_parameterOverviewByUID[parameter.parameterUID] = parameter;
			m_parameterOverviewByLabel[parameter.parameterLabel] = &m_parameterOverviewByUID[parameter.parameterUID];
		}
		else
		{
			std::list<ot::Variable>& values = existingParameterEntry->second.values;
			values.splice(values.begin(), parameter.values);
			values.sort();
		}
	}

	auto allQuanties = seriesMetadata.getQuantities();
	for (auto& quantity : allQuanties)
	{
		auto& valueDescriptions = quantity.valueDescriptions;
		assert(valueDescriptions.size()>= 1);
		ot::UID index = valueDescriptions.begin()->quantityIndex;
		auto existingQuantityEntry = m_quantityOverviewByUID.find(index);
		if (existingQuantityEntry == m_quantityOverviewByUID.end())
		{
			m_quantityOverviewByUID[index] = quantity;
			m_quantityOverviewByLabel[quantity.quantityLabel] = &m_quantityOverviewByUID[index];
		}
	}
}


void MetadataCampaign::reset()
{
	m_seriesMetadata.clear();
	m_quantityOverviewByLabel.clear();
	m_parameterOverviewByLabel.clear();
	m_metaData.clear();
}

