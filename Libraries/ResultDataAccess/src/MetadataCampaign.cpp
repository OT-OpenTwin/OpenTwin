#include "MetadataCampaign.h"
#include <cassert>

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
		assert(quantity.quantityIndex != 0);
		auto existingQuantityEntry = m_quantityOverviewByUID.find(quantity.quantityIndex);
		if (existingQuantityEntry == m_quantityOverviewByUID.end())
		{
			m_quantityOverviewByUID[quantity.quantityIndex] = quantity;
			m_quantityOverviewByLabel[quantity.quantityLabel] = &m_quantityOverviewByUID[quantity.quantityIndex];
		}
		else
		{
			assert(0); //Each quantity should have its unique ID.
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

