#pragma once
#include <map>
#include <string>
#include <memory>

#include "MetadataQuantity.h"
#include "MetadataParameter.h"

class MeasurementCampaign
{
public:
	void AddQuantity(MetadataQuantity& quantity);
	void AddParameter (MetadataParameter& parameter);
	void AddMetaInformation(const std::string& key, std::list<ot::Variable>& metadata);

private:
	std::map < std::string, MetadataQuantity > _quantities;
	std::map < std::string, MetadataParameter > _parameter;
	
	const std::string measurementCampaignName;
	
	std::map < std::string, std::list<ot::Variable>> metaData;
};

