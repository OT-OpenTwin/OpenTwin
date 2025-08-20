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
	//First we are adding the parameter of the series to the parameter overview of the entire campaign
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
			//If the parameter was already added, we extend the list of possible values
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
			//assert(0); //Each quantity should only be added once.
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
	m_metaData.clear();
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