#include <vector>

#include "DigitalTwinAccess.h"
#include "MeasurementCampaignFactory.h"
#include "OTCore/FolderNames.h"
#include "ClassFactory.h"
#include "DataBase.h"
#include "EntityMetadataCampaign.h"
#include "EntityMetadataSeries.h"

DigitalTwinAccess::DigitalTwinAccess(const std::string& projectName, ot::components::ModelComponent* modelComponent)
	:_dataStorageAccess("Projects",projectName), _modelComponent(modelComponent)
{
	assert(_modelComponent != nullptr);
	LoadExistingCampaignData();

}

void DigitalTwinAccess::LoadExistingCampaignData()
{
	std::vector<EntityBase*> allExistingMetadata = FindAllExistingMetadata();


	EntityMetadataCampaign temp(-1, nullptr, nullptr, nullptr, nullptr, "");

	std::list<std::shared_ptr<EntityMetadataSeries>> existingMetadataEntities;


	MeasurementCampaignFactory campaignFactory;
	//campaignFactory.Create()

}

std::vector<EntityBase*> DigitalTwinAccess::FindAllExistingMetadata()
{
	std::list<std::string> allExistingMetadataNames = _modelComponent->getListOfFolderItems(ot::FolderNames::DatasetFolder);
	std::list<ot::EntityInformation> entityInfos;
	_modelComponent->getEntityInformation(allExistingMetadataNames, entityInfos);
	std::list<std::pair<ot::UID, ot::UID>> prefetchIdandVersion;
	for (auto& entityInfo : entityInfos)
	{
		prefetchIdandVersion.push_back(std::pair<ot::UID, ot::UID>(entityInfo.getID(), entityInfo.getVersion()));
	}
	DataBase::GetDataBase()->PrefetchDocumentsFromStorage(prefetchIdandVersion);


	ClassFactory classFactory;
	std::vector<EntityBase*> allExistingMetadata;
	allExistingMetadata.reserve(entityInfos.size());
	for (auto& entityInfo : entityInfos)
	{
		allExistingMetadata.push_back(_modelComponent->readEntityFromEntityIDandVersion(entityInfo.getID(), entityInfo.getVersion(), classFactory));
	}
	
	return allExistingMetadata;
}
