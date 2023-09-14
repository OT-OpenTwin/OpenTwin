#pragma once
#include <map>
#include <string>
#include <memory>

#include "SeriesMetadata.h"
#include "MetadataQuantity.h"
#include "MetadataParameter.h"
#include "MetadataEntry.h"
#include "SeriesMetadata.h"


class MeasurementCampaign
{
public:
	void AddSeriesMetadata(SeriesMetadata&& seriesMetadata) { _seriesMetadata.push_back(seriesMetadata); }
	void AddMetaInformation(const std::string& key, std::shared_ptr<MetadataEntry> metadatametadata);
	const std::list<SeriesMetadata>& getSeriesMetadata() { return _seriesMetadata; };
	void setParameterOverview(std::map <std::string, MetadataParameter>&& parameter) { _parameter = parameter; };
	void setQuantityOverview(std::map <std::string, MetadataQuantity>&& quantities) { _quantities = quantities; };

private:
	std::list<SeriesMetadata> _seriesMetadata;
	
	std::map < std::string, MetadataQuantity > _quantities;
	std::map < std::string, MetadataParameter > _parameter;
	
	const std::string measurementCampaignName;
	
	std::map <std::string, std::shared_ptr<MetadataEntry>> metaData;
};

