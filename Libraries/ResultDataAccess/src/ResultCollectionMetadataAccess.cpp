// @otlicense
// File: ResultCollectionMetadataAccess.cpp
// 
// License:
// Copyright 2025 by OpenTwin
//  
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
//  
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// @otlicense-end

#include <vector>

#include "ResultCollectionMetadataAccess.h"
#include "CrossCollectionAccess.h"
#include "MetadataEntityInterface.h"
#include "OTCore/FolderNames.h"
#include "OTModelAPI/ModelServiceAPI.h"
#include "DataBase.h"
#include "EntityAPI.h"
#include "EntityMetadataCampaign.h"
#include "EntityMetadataSeries.h"

ResultCollectionMetadataAccess::ResultCollectionMetadataAccess(const std::string& _collectionName, ot::components::ModelComponent* _modelComponent)
	:m_modelComponent(_modelComponent), m_collectionName(_collectionName)
{
	loadExistingCampaignData();
}

ResultCollectionMetadataAccess::ResultCollectionMetadataAccess(const std::string& _crossCollectionName, ot::components::ModelComponent* _modelComponent, const std::string& _sessionServiceURL)
	:m_modelComponent(_modelComponent), m_collectionName(_crossCollectionName)
{
	CrossCollectionAccess crossCollectionAccess(_crossCollectionName, _sessionServiceURL, _modelComponent->getServiceURL());
	std::shared_ptr<EntityMetadataCampaign> campaignMetadataEntity =	crossCollectionAccess.getMeasurementCampaignMetadata(*_modelComponent);
	if (campaignMetadataEntity != nullptr)
	{
		m_metadataExistInProject = true;
		std::list<std::shared_ptr<EntityMetadataSeries>> seriesMetadataEntities = crossCollectionAccess.getMeasurementMetadata(*_modelComponent);
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

const std::list<std::string> ResultCollectionMetadataAccess::listAllParameterLabelsFromSeries(const std::string& _label) const
{
	const MetadataSeries* series = findMetadataSeries(_label);
	assert(series != nullptr);
	auto allParameter = series->getParameter();
	std::list<std::string> allParameterLabel;
	for (const auto& parameter : allParameter)
	{
		allParameterLabel.push_back(parameter.parameterLabel);
	}
	return allParameterLabel;
}

const std::list<std::string> ResultCollectionMetadataAccess::listAllQuantityLabelsFromSeries(const std::string& _label) const
{
	const MetadataSeries* series = findMetadataSeries(_label);
	assert(series != nullptr);
	auto allQuantities = series->getQuantities();
	std::list<std::string> allQuantityLabel;
	for (const auto& quantity : allQuantities)
	{
		allQuantityLabel.push_back(quantity.quantityLabel);
	}
	return allQuantityLabel;
}

const MetadataSeries* ResultCollectionMetadataAccess::findMetadataSeries(const std::string& _name) const
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

const MetadataSeries* ResultCollectionMetadataAccess::findMetadataSeries(ot::UID _index) const
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

const MetadataParameter* ResultCollectionMetadataAccess::findMetadataParameter(const std::string& _name) const
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

const MetadataParameter* ResultCollectionMetadataAccess::findMetadataParameter(ot::UID _index) const
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

const MetadataQuantity* ResultCollectionMetadataAccess::findMetadataQuantity(const std::string& name) const
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

const MetadataQuantity* ResultCollectionMetadataAccess::findMetadataQuantity(ot::UID _index) const 
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

void ResultCollectionMetadataAccess::loadExistingCampaignData()
{
	std::vector<EntityBase*> allExistingMetadata = findAllExistingMetadata();

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
			if (series != nullptr)
			{
				metadataSeriesEntities.push_back(std::shared_ptr<EntityMetadataSeries>(series));
				series = nullptr;
			}
		}
	}
	
	MetadataEntityInterface campaignFactory;
	m_metadataCampaign =	campaignFactory.createCampaign(metadataCampaignEntity, metadataSeriesEntities);
}

std::vector<EntityBase*> ResultCollectionMetadataAccess::findAllExistingMetadata()
{
	std::list<std::string> allExistingMetadataNames = ot::ModelServiceAPI::getListOfFolderItems(ot::FolderNames::DatasetFolder);
	std::list<ot::EntityInformation> entityInfos;
	ot::ModelServiceAPI::getEntityInformation(allExistingMetadataNames, entityInfos);
	std::list<std::pair<ot::UID, ot::UID>> prefetchIdandVersion;
	for (auto& entityInfo : entityInfos)
	{
		prefetchIdandVersion.push_back(std::pair<ot::UID, ot::UID>(entityInfo.getEntityID(), entityInfo.getEntityVersion()));
	}
	DataBase::instance().prefetchDocumentsFromStorage(prefetchIdandVersion);

	std::vector<EntityBase*> allExistingMetadata;
	allExistingMetadata.reserve(entityInfos.size());
	for (auto& entityInfo : entityInfos)
	{
		allExistingMetadata.push_back(ot::EntityAPI::readEntityFromEntityIDandVersion(entityInfo.getEntityID(), entityInfo.getEntityVersion()));
	}
	
	return allExistingMetadata;
}
