#pragma once
#include "MeasurementCampaign.h"
#include "EntityMeasurementMetadata.h"
#include "EntityResearchMetadata.h"

class MeasurementCampaignFactory
{
public:
	MeasurementCampaign Create(std::shared_ptr<EntityResearchMetadata> rmd, std::list<std::shared_ptr<EntityMeasurementMetadata>> msmds);
};

