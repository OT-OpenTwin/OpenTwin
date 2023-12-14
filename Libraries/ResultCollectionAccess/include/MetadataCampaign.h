#pragma once
#include <map>
#include <string>
#include <memory>

#include "MetadataSeries.h"
#include "MetadataQuantity.h"
#include "MetadataParameter.h"
#include "MetadataEntry.h"


class __declspec(dllexport) MetadataCampaign
{
public:
	MetadataCampaign(){}
	MetadataCampaign(const MetadataCampaign& other) = delete;
	MetadataCampaign operator=(const MetadataCampaign& other) = delete;
	MetadataCampaign(MetadataCampaign&& other);
	MetadataCampaign operator=(MetadataCampaign&& other);

	void AddSeriesMetadata(MetadataSeries&& seriesMetadata) { _seriesMetadata.push_back(seriesMetadata); }
	void AddMetaInformation(const std::string& key, std::shared_ptr<MetadataEntry> metadatametadata);
	const std::list<MetadataSeries>& getSeriesMetadata()const { return _seriesMetadata; };
	const std::map <std::string, MetadataQuantity>& getMetadataQuantitiesByName() const { return _quantitiesByName; }
	const std::map <std::string, MetadataParameter>& getMetadataParameterByName() const { return _parameterByName; }

	void setParameterOverview(std::map <std::string, MetadataParameter>&& parameterByName) { _parameterByName = parameterByName; };
	void setQuantityOverview(std::map <std::string, MetadataQuantity>&& quantitiesByName) { _quantitiesByName = quantitiesByName; };

	void reset();
private:
	std::list<MetadataSeries> _seriesMetadata;
	
	std::map < std::string, MetadataQuantity > _quantitiesByName;
	std::map < std::string, MetadataParameter > _parameterByName;
	
	//Kann komischerweise nicht gelöscht werden
	const std::string _measurementCampaignName;
	
	std::map <std::string, std::shared_ptr<MetadataEntry>> _metaData;
};

