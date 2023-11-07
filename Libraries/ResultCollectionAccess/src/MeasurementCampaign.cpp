#include "MeasurementCampaign.h"

void MeasurementCampaign::AddMetaInformation(const std::string& key, std::shared_ptr<MetadataEntry> metadata)
{
}

void MeasurementCampaign::reset()
{
	_seriesMetadata.clear();
	_quantitiesByName.clear();
	_parameterByName.clear();
	metaData.clear();
}
