#pragma once
#include <string>
#include "Document/DocumentAccess.h"
#include "ResultDataStorageAPI.h"
#include "MetadataCampaign.h"
#include "OTServiceFoundation/ModelComponent.h"

class DigitalTwinAccess
{
public:
	DigitalTwinAccess(const std::string& projectName, ot::components::ModelComponent* modelComponent);
	MetadataCampaign& getMetadataCampaign() { return _metadataCampaign; }
private:
	ot::components::ModelComponent* _modelComponent = nullptr;
	DataStorageAPI::ResultDataStorageAPI _dataStorageAccess;
	MetadataCampaign _metadataCampaign;

	void LoadExistingCampaignData();
	std::vector<EntityBase*> FindAllExistingMetadata();
};
