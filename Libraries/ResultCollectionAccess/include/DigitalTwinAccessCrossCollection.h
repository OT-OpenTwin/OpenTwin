#pragma once
#include "OTServiceFoundation/ModelComponent.h"
#include "MetadataCampaign.h"

class DigitalTwinAccessCrossCollection
{
public:
	DigitalTwinAccessCrossCollection(const std::string& projectName, ot::components::ModelComponent* modelComponent, const std::string& sessionServiceURL, const std::string& modelServiceURL);
	MetadataCampaign& getMetadataCampaign() { return _metadataCampaign; }

private:
	MetadataCampaign _metadataCampaign;
};
