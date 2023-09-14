#pragma once
#include "MeasurementCampaign.h"
#include "EntityMeasurementMetadata.h"
#include "EntityResearchMetadata.h"

#include "MetadataEntry.h"

class MeasurementCampaignFactory
{
public:
	MeasurementCampaign Create(std::shared_ptr<EntityResearchMetadata> rmd, std::list<std::shared_ptr<EntityMeasurementMetadata>> msmds);

private:
	void ExtractCampaignMetadata(MeasurementCampaign& measurementCampaign, std::shared_ptr<EntityResearchMetadata> rmd);
	void ExtractSeriesMetadata(MeasurementCampaign& measurementCampaign, std::list<std::shared_ptr<EntityMeasurementMetadata>> msmds);

	std::list<std::shared_ptr<MetadataEntry>> ExtractMetadataObjects(const GenericDocument& document);
	std::list<std::shared_ptr<MetadataEntry>> ExtractMetadataFields(const GenericDocument& document);

	void BuildUpOverviewLists(MeasurementCampaign& measurementCampaign);
};

