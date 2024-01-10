#include "MetadataCampaign.h"

MetadataCampaign::MetadataCampaign(MetadataCampaign&& other)
	:_seriesMetadata(std::move(other._seriesMetadata)),
	_quantityOverviewByName(std::move(other._quantityOverviewByName)),
	_parameterOverviewByName(std::move(other._parameterOverviewByName)),
	_metaData(std::move(other._metaData)),
	_campaignName(other._campaignName)
{}

MetadataCampaign::MetadataCampaign(const MetadataCampaign& other)
	:_seriesMetadata(other._seriesMetadata),
	_quantityOverviewByName(other._quantityOverviewByName),
	_parameterOverviewByName(other._parameterOverviewByName),
	_metaData(other._metaData),
	_campaignName(other._campaignName)
{}

MetadataCampaign& MetadataCampaign::operator=(MetadataCampaign&& other)
{
	_seriesMetadata = std::move(other._seriesMetadata);
	_quantityOverviewByName = std::move(other._quantityOverviewByName);
	_parameterOverviewByName = std::move(other._parameterOverviewByName);
	_metaData = std::move(other._metaData);
	_campaignName = std::move(other._campaignName);
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


void MetadataCampaign::UpdateMetadataOverview()
{
	for (auto& seriesMetadata : _seriesMetadata)
	{
		UpdateMetadataOverview(seriesMetadata);
	}
}

void MetadataCampaign::UpdateMetadataOverviewFromLastAddedSeries()
{
	UpdateMetadataOverview(_seriesMetadata.back());
}

void MetadataCampaign::UpdateMetadataOverview(MetadataSeries& seriesMetadata)
{
	auto allParameter = seriesMetadata.getParameter();
	for (auto& parameter : allParameter)
	{
		if (_parameterOverviewByName.find(parameter.parameterName) == _parameterOverviewByName.end())
		{
			_parameterOverviewByName[parameter.parameterName] = parameter;
		}
		else
		{
			std::list<ot::Variable>& values = _parameterOverviewByName[parameter.parameterName].values;
			values.splice(values.begin(), parameter.values);
		}
	}

	auto allQuanties = seriesMetadata.getQuantities();
	for (auto& quantity : allQuanties)
	{
		if (_quantityOverviewByName.find(quantity.quantityName) == _quantityOverviewByName.end())
		{
			_quantityOverviewByName[quantity.quantityName] = quantity;
		}
	}
}


void MetadataCampaign::reset()
{
	_seriesMetadata.clear();
	_quantityOverviewByName.clear();
	_parameterOverviewByName.clear();
	_metaData.clear();
}

