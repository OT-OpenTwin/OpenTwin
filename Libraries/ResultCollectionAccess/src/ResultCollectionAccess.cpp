#include <vector>

#include "ResultCollectionAccess.h"
#include "CrossCollectionAccess.h"
#include "MetadataEntityInterface.h"
#include "OTCore/FolderNames.h"
#include "ClassFactory.h"
#include "DataBase.h"
#include "EntityMetadataCampaign.h"
#include "EntityMetadataSeries.h"

ResultCollectionAccess::ResultCollectionAccess(const std::string& collectionName, ot::components::ModelComponent* modelComponent, ClassFactory *classFactory)
	:_dataStorageAccess("Projects", collectionName), _modelComponent(modelComponent), _collectionName(collectionName)
{
	LoadExistingCampaignData(classFactory);
}

ResultCollectionAccess::ResultCollectionAccess(const std::string& crossCollectionName, ot::components::ModelComponent* modelComponent, ClassFactory* classFactory, const std::string& sessionServiceURL)
	:_dataStorageAccess("Projects", crossCollectionName),_modelComponent(modelComponent), _collectionName(crossCollectionName)
{
	CrossCollectionAccess crossCollectionAccess(crossCollectionName, sessionServiceURL, modelComponent->serviceURL());
	std::shared_ptr<EntityMetadataCampaign> campaignMetadataEntity =	crossCollectionAccess.getMeasurementCampaignMetadata(*_modelComponent, classFactory);
	std::list<std::shared_ptr<EntityMetadataSeries>> seriesMetadataEntities = crossCollectionAccess.getMeasurementMetadata(*_modelComponent, classFactory);

	MetadataEntityInterface campaignFactory;
	_metadataCampaign = campaignFactory.CreateCampaign(campaignMetadataEntity, seriesMetadataEntities);
	
}

ResultCollectionAccess::ResultCollectionAccess(ResultCollectionAccess&& other)
	:_dataStorageAccess(std::move(other._dataStorageAccess)),_modelComponent(other._modelComponent), _metadataCampaign(std::move(other._metadataCampaign)), _collectionName(other._collectionName)
{}

ResultCollectionAccess& ResultCollectionAccess::operator=(ResultCollectionAccess&& other) noexcept
{
	_collectionName = std::move(other._collectionName);
	_dataStorageAccess = std::move(other._dataStorageAccess);
	_modelComponent = std::move(other._modelComponent);
	_metadataCampaign = (std::move(other._metadataCampaign));
	return *this;
}

const std::list<std::string> ResultCollectionAccess::ListAllSeriesNames() const
{
	auto& seriesMetadataEntries= _metadataCampaign.getSeriesMetadata();
	std::list<std::string> seriesMetadataNames;
	for (auto& seriesMetadata: seriesMetadataEntries)
	{
		seriesMetadataNames.push_back(seriesMetadata.getName());
	}
	return seriesMetadataNames;
}

const std::list<std::string> ResultCollectionAccess::ListAllParameterNames() const
{
	auto& parametersByName = _metadataCampaign.getMetadataParameterByName();
	std::list<std::string> parameterNames;
	for (auto& parameterByName : parametersByName)
	{
		parameterNames.push_back(parameterByName.first);
	}
	return parameterNames;
}

const std::list<std::string> ResultCollectionAccess::ListAllQuantityNames() const
{
	auto& quantitiesByName = _metadataCampaign.getMetadataQuantitiesByName();
	std::list<std::string> quantityNames;
	for (auto& quantityByName : quantitiesByName)
	{
		quantityNames.push_back(quantityByName.first);
	}
	return quantityNames;
}

const MetadataSeries* ResultCollectionAccess::FindMetadataSeries(const std::string& name)
{
	const std::list<MetadataSeries>& allSeriesMetadata = _metadataCampaign.getSeriesMetadata();

	const MetadataSeries* selectedMetadataSeries = nullptr;
	for (auto& metadataSeries : allSeriesMetadata)
	{
		if (metadataSeries.getName() == name)
		{
			selectedMetadataSeries = &metadataSeries;
			break;
		}
	}
	return selectedMetadataSeries;
}

const MetadataParameter* ResultCollectionAccess::FindMetadataParameter(const std::string& name)
{
	auto& parameterByName = _metadataCampaign.getMetadataParameterByName();
	auto parameter = parameterByName.find(name);
	if (parameter == parameterByName.end())
	{
		return nullptr;
	}
	else
	{
		return &(parameter->second);
	}
}

const MetadataQuantity* ResultCollectionAccess::FindMetadataQuantity(const std::string& name)
{
	auto& quantitiesByName = _metadataCampaign.getMetadataQuantitiesByName();
	auto quantity = quantitiesByName.find(name);
	if (quantity == quantitiesByName.end())
	{
		return nullptr;
	}
	else
	{
		return &(quantity->second);

	}
}

void ResultCollectionAccess::LoadExistingCampaignData(ClassFactory* classFactory)
{
	std::vector<EntityBase*> allExistingMetadata = FindAllExistingMetadata(classFactory);


	EntityMetadataCampaign temp(-1, nullptr, nullptr, nullptr, nullptr, "");
	std::shared_ptr<EntityMetadataCampaign> metadataCampaignEntity = nullptr;
	std::list<std::shared_ptr<EntityMetadataSeries>> metadataSeriesEntities;

	for (EntityBase* entity : allExistingMetadata)
	{
		if (entity->getClassName() == temp.getClassName())
		{
			metadataCampaignEntity.reset(dynamic_cast<EntityMetadataCampaign*>(entity));
		}
		else
		{
			metadataSeriesEntities.push_back(std::shared_ptr<EntityMetadataSeries>(dynamic_cast<EntityMetadataSeries*>(entity)));
		}
	}
	
	MetadataEntityInterface campaignFactory;
	_metadataCampaign =	campaignFactory.CreateCampaign(metadataCampaignEntity, metadataSeriesEntities);
}

std::vector<EntityBase*> ResultCollectionAccess::FindAllExistingMetadata(ClassFactory *classFactory)
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

	std::vector<EntityBase*> allExistingMetadata;
	allExistingMetadata.reserve(entityInfos.size());
	for (auto& entityInfo : entityInfos)
	{
		allExistingMetadata.push_back(_modelComponent->readEntityFromEntityIDandVersion(entityInfo.getID(), entityInfo.getVersion(), *classFactory));
	}
	
	return allExistingMetadata;
}
