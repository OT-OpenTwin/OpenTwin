#include "MetadataCampaign.h"

void MetadataCampaign::AddMetaInformation(const std::string& key, std::shared_ptr<MetadataEntry> metadata)
{
}

void MetadataCampaign::reset()
{
	_seriesMetadata.clear();
	_quantitiesByName.clear();
	_parameterByName.clear();
	metaData.clear();
}
