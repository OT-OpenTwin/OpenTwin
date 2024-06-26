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
	MetadataCampaign(const MetadataCampaign& other);
	MetadataCampaign& operator=(const MetadataCampaign& other) = delete;
	MetadataCampaign(MetadataCampaign&& other);
	MetadataCampaign& operator=(MetadataCampaign&& other);
	~MetadataCampaign() {};
	void AddSeriesMetadata(MetadataSeries&& seriesMetadata) { _seriesMetadata.push_back(seriesMetadata); }
	void AddMetaInformation(const std::string& key, std::shared_ptr<MetadataEntry> metadata) { _metaData[key] = metadata; }
	const std::list<MetadataSeries>& getSeriesMetadata() const { return _seriesMetadata; };

	const std::map <std::string, MetadataQuantity>& getMetadataQuantitiesByName() const { return _quantityOverviewByName; }
	const std::map <std::string, MetadataParameter>& getMetadataParameterByName() const { return _parameterOverviewByName; }
	const std::map <std::string, std::shared_ptr<MetadataEntry>>&	getMetaData() const { return _metaData; }
	
	void UpdateMetadataOverview();
	void UpdateMetadataOverviewFromLastAddedSeries();
	void setCampaignName(const std::string name) { _campaignName = name; }
	const std::string& getCampaignName()const { return _campaignName; }
	void reset();
private:
	std::list<MetadataSeries> _seriesMetadata;
	
	std::map < std::string, MetadataQuantity > _quantityOverviewByName;
	std::map < std::string, MetadataParameter > _parameterOverviewByName;
	
	std::string _campaignName;
	
	std::map <std::string, std::shared_ptr<MetadataEntry>> _metaData;

	void UpdateMetadataOverview(MetadataSeries& series);

};

