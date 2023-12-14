#include "MetadataCampaign.h"

MetadataCampaign::MetadataCampaign(MetadataCampaign&& other)
	:_seriesMetadata(std::move(other._seriesMetadata)),
	_quantitiesByName(std::move(other._quantitiesByName)),
	_parameterByName(std::move(other._parameterByName)),
	_metaData(std::move(other._metaData)),
	_measurementCampaignName(other._measurementCampaignName)
{}

MetadataCampaign MetadataCampaign::operator=(MetadataCampaign&& other)
{
	return MetadataCampaign(std::move(other));
}

void MetadataCampaign::AddMetaInformation(const std::string& key, std::shared_ptr<MetadataEntry> metadata)
{
}

void MetadataCampaign::reset()
{
	_seriesMetadata.clear();
	_quantitiesByName.clear();
	_parameterByName.clear();
	_metaData.clear();
}
