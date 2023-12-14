#pragma once
#include "MetadataCampaign.h"
#include "MetadataQuantity.h"
#include "MetadataParameter.h"
#include "MetadataSeries.h"

class DigitalTwinExtender
{
public:
	DigitalTwinExtender(MetadataCampaign& metadataCampaign);
	void AddQuantity(MetadataQuantity&& quantity, const std::string metadataSeriesName);
	void AddParameter(MetadataParameter&& parameter, const std::string metadataSeriesName);
	void AddParameter(MetadataSeries&& parameter);

private:
	MetadataCampaign& _metadataCampaign;

	const std::string _parameterAbbreviationBase = "P_";
	const std::string _quantityAbbreviationBase = "Q_";
	/*const std::string _nameField;
	const std::string _valueField;
	const std::string _typeField;*/
	const std::string _msmdNameBase = "Series Metadata_";

	uint32_t FindNextFreeQuantityIndex();

};


