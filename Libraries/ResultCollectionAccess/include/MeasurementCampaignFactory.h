#pragma once
#include "MetadataCampaign.h"
#include "EntityMetadataSeries.h"
#include "EntityMetadataCampaign.h"

#include "MetadataEntry.h"

class __declspec(dllexport) MeasurementCampaignFactory
{
public:
	MetadataCampaign Create(std::shared_ptr<EntityMetadataCampaign> rmd, std::list<std::shared_ptr<EntityMetadataSeries>> msmds);

private:
	void ExtractCampaignMetadata(MetadataCampaign& measurementCampaign, std::shared_ptr<EntityMetadataCampaign> rmd);
	void ExtractSeriesMetadata(MetadataCampaign& measurementCampaign, std::list<std::shared_ptr<EntityMetadataSeries>> msmds);

	std::list<std::shared_ptr<MetadataEntry>> ExtractMetadataObjects(const GenericDocument& document);
	std::list<std::shared_ptr<MetadataEntry>> ExtractMetadataFields(const GenericDocument& document);

	void BuildUpOverviewLists(MetadataCampaign& measurementCampaign);
};

