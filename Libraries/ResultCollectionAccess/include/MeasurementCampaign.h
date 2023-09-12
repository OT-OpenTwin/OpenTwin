#pragma once
#include <map>
#include <string>

#include "MetadataQuantity.h"
#include "MetadataParameter.h"

class MeasurementCampaign
{


private:
	std::map < std::string, MetadataQuantity > _quantities;
	std::map < std::string, MetadataParameter > _parameter;
	
	const std::string measurementCampaignName;
	
	std::map < std::string, std::list<ot::Variable>> metaData;
};

