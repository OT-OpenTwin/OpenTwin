#pragma once
#include "MetadataCampaign.h"
#include "EntityMeasurementMetadata.h"
#include "EntityResearchMetadata.h"

#include "MetadataEntry.h"

class __declspec(dllexport) MeasurementCampaignFactory
{
public:
	MetadataCampaign Create(std::shared_ptr<EntityResearchMetadata> rmd, std::list<std::shared_ptr<EntityMeasurementMetadata>> msmds);

private:
	void ExtractCampaignMetadata(MetadataCampaign& measurementCampaign, std::shared_ptr<EntityResearchMetadata> rmd);
	void ExtractSeriesMetadata(MetadataCampaign& measurementCampaign, std::list<std::shared_ptr<EntityMeasurementMetadata>> msmds);

	std::list<std::shared_ptr<MetadataEntry>> ExtractMetadataObjects(const GenericDocument& document);
	std::list<std::shared_ptr<MetadataEntry>> ExtractMetadataFields(const GenericDocument& document);

	void BuildUpOverviewLists(MetadataCampaign& measurementCampaign);
};

