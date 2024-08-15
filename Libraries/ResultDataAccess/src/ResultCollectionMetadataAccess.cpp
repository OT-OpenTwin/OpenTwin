#include <vector>

#include "ResultCollectionMetadataAccess.h"
#include "CrossCollectionAccess.h"
#include "MetadataEntityInterface.h"
#include "OTCore/FolderNames.h"
#include "ClassFactory.h"
#include "DataBase.h"
#include "EntityMetadataCampaign.h"
#include "EntityMetadataSeries.h"

ResultCollectionMetadataAccess::ResultCollectionMetadataAccess(const std::string& _collectionName, ot::components::ModelComponent* _modelComponent, ClassFactory *_classFactory)
	:m_modelComponent(_modelComponent), m_collectionName(_collectionName)
{
	loadExistingCampaignData(_classFactory);
}

ResultCollectionMetadataAccess::ResultCollectionMetadataAccess(const std::string& _crossCollectionName, ot::components::ModelComponent* _modelComponent, ClassFactory* _classFactory, const std::string& _sessionServiceURL)
	:m_modelComponent(_modelComponent), m_collectionName(_crossCollectionName)
{
	CrossCollectionAccess crossCollectionAccess(_crossCollectionName, _sessionServiceURL, _modelComponent->serviceURL());
	std::shared_ptr<EntityMetadataCampaign> campaignMetadataEntity =	crossCollectionAccess.getMeasurementCampaignMetadata(*_modelComponent, _classFactory);
	if (campaignMetadataEntity != nullptr)
	{
		m_metadataExistInProject = true;
		std::list<std::shared_ptr<EntityMetadataSeries>> seriesMetadataEntities = crossCollectionAccess.getMeasurementMetadata(*_modelComponent, _classFactory);
		MetadataEntityInterface campaignFactory;
		m_metadataCampaign = campaignFactory.createCampaign(campaignMetadataEntity, seriesMetadataEntities);
	}
}

ResultCollectionMetadataAccess::ResultCollectionMetadataAccess(ResultCollectionMetadataAccess&& other) noexcept
	:m_modelComponent(other.m_modelComponent), m_metadataCampaign(std::move(other.m_metadataCampaign)), m_collectionName(other.m_collectionName)
{}

ResultCollectionMetadataAccess& ResultCollectionMetadataAccess::operator=(ResultCollectionMetadataAccess&& other) noexcept
{
	m_collectionName = std::move(other.m_collectionName);
	m_modelComponent = std::move(other.m_modelComponent);
	m_metadataCampaign = (std::move(other.m_metadataCampaign));
	return *this;
}

const std::list<std::string> ResultCollectionMetadataAccess::listAllSeriesNames() const
{
	auto& seriesMetadataEntries= m_metadataCampaign.getSeriesMetadata();
	std::list<std::string> seriesMetadataNames;
	for (auto& seriesMetadata: seriesMetadataEntries)
	{
		seriesMetadataNames.push_back(seriesMetadata.getName());
	}
	return seriesMetadataNames;
}

const std::list<std::string> ResultCollectionMetadataAccess::listAllParameterLabels() const
{
	auto& parametersByName = m_metadataCampaign.getMetadataParameterByLabel();
	std::list<std::string> parameterNames;
	for (auto& parameterByName : parametersByName)
	{
		parameterNames.push_back(parameterByName.first);
	}
	return parameterNames;
}

const std::list<std::string> ResultCollectionMetadataAccess::listAllQuantityLabels() const
{
	auto& quantitiesByName = m_metadataCampaign.getMetadataQuantitiesByLabel();
	std::list<std::string> quantityNames;
	for (auto& quantityByName : quantitiesByName)
	{
		quantityNames.push_back(quantityByName.first);
	}
	return quantityNames;
}

const MetadataSeries* ResultCollectionMetadataAccess::findMetadataSeries(const std::string& _name)
{
	const std::list<MetadataSeries>& allSeriesMetadata = m_metadataCampaign.getSeriesMetadata();

	const MetadataSeries* selectedMetadataSeries = nullptr;
	for (auto& metadataSeries : allSeriesMetadata)
	{
		if (metadataSeries.getName() == _name)
		{
			selectedMetadataSeries = &metadataSeries;
			break;
		}
	}
	return selectedMetadataSeries;
}

const MetadataSeries* ResultCollectionMetadataAccess::findMetadataSeries(ot::UID _index)
{
	const std::list<MetadataSeries>& allSeriesMetadata = m_metadataCampaign.getSeriesMetadata();

	const MetadataSeries* selectedMetadataSeries = nullptr;
	for (auto& metadataSeries : allSeriesMetadata)
	{
		if (metadataSeries.getSeriesIndex() == _index)
		{
			selectedMetadataSeries = &metadataSeries;
			break;
		}
	}
	return selectedMetadataSeries;
}

const MetadataParameter* ResultCollectionMetadataAccess::findMetadataParameter(const std::string& _name)
{
	auto& parameterByName = m_metadataCampaign.getMetadataParameterByLabel();
	auto parameter = parameterByName.find(_name);
	if (parameter == parameterByName.end())
	{
		return nullptr;
	}
	else
	{
		return parameter->second;
	}
}

const MetadataParameter* ResultCollectionMetadataAccess::findMetadataParameter(ot::UID _index)
{
	auto& parameterByUID = m_metadataCampaign.getMetadataParameterByUID();
	auto parameter = parameterByUID.find(_index);
	if (parameter == parameterByUID.end())
	{
		return nullptr;
	}
	else
	{
		return &(parameter->second);
	}
}

const MetadataQuantity* ResultCollectionMetadataAccess::findMetadataQuantity(const std::string& name)
{
	auto& quantitiesByLabel = m_metadataCampaign.getMetadataQuantitiesByLabel();
	auto quantity = quantitiesByLabel.find(name);
	if (quantity == quantitiesByLabel.end())
	{
		return nullptr;
	}
	else
	{
		return quantity->second;

	}
}

const MetadataQuantity* ResultCollectionMetadataAccess::findMetadataQuantity(ot::UID _index)
{
	auto& quantityByUID = m_metadataCampaign.getMetadataQuantitiesByUID();
	auto quantity= quantityByUID.find(_index);
	if (quantity == quantityByUID.end())
	{
		return nullptr;
	}
	else
	{
		return &(quantity->second);
	}
}

std::list<MetadataQuantity*> ResultCollectionMetadataAccess::findQuantityWithSameName(const std::string& _name)
{
	std::list<MetadataQuantity*> matchingQuantities;
	auto& quantitiesByLabel = m_metadataCampaign.getMetadataQuantitiesByLabel();
	
	for (auto quantityByLabel : quantitiesByLabel)
	{
		MetadataQuantity* quantity = 	quantityByLabel.second;
		if (quantity->quantityName == _name)
		{
			matchingQuantities.push_back(quantity);
		}
	}

	return matchingQuantities;
}

std::list<MetadataParameter*> ResultCollectionMetadataAccess::findParameterWithSameName(const std::string& _name)
{
	std::list<MetadataParameter*> matchingParameter;
	auto& parametersByLabel = m_metadataCampaign.getMetadataParameterByLabel();

	for (auto parameterByLabel : parametersByLabel)
	{
		MetadataParameter* parameter = parameterByLabel.second;
		if (parameter->parameterName == _name)
		{
			matchingParameter.push_back(parameter);
		}
	}
	return matchingParameter;
}

void ResultCollectionMetadataAccess::loadExistingCampaignData(ClassFactory* _classFactory)
{
	std::vector<EntityBase*> allExistingMetadata = findAllExistingMetadata(_classFactory);

	std::shared_ptr<EntityMetadataCampaign> metadataCampaignEntity = nullptr;
	std::list<std::shared_ptr<EntityMetadataSeries>> metadataSeriesEntities;

	for (EntityBase* entity : allExistingMetadata)
	{
		EntityMetadataCampaign* campaign = dynamic_cast<EntityMetadataCampaign*>(entity);
		if (campaign != nullptr)
		{
			assert(metadataCampaignEntity == nullptr); //Should only exist once.
			metadataCampaignEntity.reset(campaign);
			campaign = nullptr;
		}
		else
		{
			EntityMetadataSeries* series =	dynamic_cast<EntityMetadataSeries*>(entity);
			assert(series != nullptr);
			metadataSeriesEntities.push_back(std::shared_ptr<EntityMetadataSeries>(series));
			series = nullptr;
		}
	}
	
	MetadataEntityInterface campaignFactory;
	m_metadataCampaign =	campaignFactory.createCampaign(metadataCampaignEntity, metadataSeriesEntities);
}

std::vector<EntityBase*> ResultCollectionMetadataAccess::findAllExistingMetadata(ClassFactory *_classFactory)
{
	std::list<std::string> allExistingMetadataNames = m_modelComponent->getListOfFolderItems(ot::FolderNames::DatasetFolder);
	std::list<ot::EntityInformation> entityInfos;
	m_modelComponent->getEntityInformation(allExistingMetadataNames, entityInfos);
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
		allExistingMetadata.push_back(m_modelComponent->readEntityFromEntityIDandVersion(entityInfo.getID(), entityInfo.getVersion(), *_classFactory));
	}
	
	return allExistingMetadata;
}
