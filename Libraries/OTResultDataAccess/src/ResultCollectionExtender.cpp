// @otlicense
// File: ResultCollectionExtender.cpp
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

// OpenTwin header
#include "OTCore/DefensiveProgramming.h"
#include "OTCore/Variable/VariableToStringConverter.h"
#include "OTResultDataAccess/ResultCollection/ResultCollectionExtender.h"

#include "OTModelEntities/MetadataEntityInterface.h"
#include "OTResultDataAccess/SerialisationInterfaces/QuantityDescriptionCurve.h"
#include "OTResultDataAccess/QuantityContainerSerialiser.h"
#include "OTResultDataAccess/SerialisationInterfaces/QuantityDescriptionSParameter.h"
#include "OTModelAPI/ModelServiceAPI.h"
#include "OTDataStorage/DataLakeHelper.h"
// std header
#include <set>
#include <cassert>
#include <algorithm>

ResultCollectionExtender::ResultCollectionExtender(const std::string& _collectionName, ot::components::ModelComponent& _modelComponent)
	: ResultCollectionMetadataAccess(_collectionName,&_modelComponent), m_requiresUpdateMetadataCampaign(false)
{}

ResultCollectionExtender::ResultCollectionExtender(ot::ApplicationBase* _applicationBase)
	: ResultCollectionExtender(_applicationBase->getCollectionName(), *_applicationBase->getModelComponent())
{}

ot::UID ResultCollectionExtender::buildSeriesMetadata(std::list<DatasetDescription>& _datasetDescriptions, const std::string& _seriesName, const ot::JsonDocument& _seriesMetadata)
{
	m_logger.log("Creating new series with " + std::to_string(_datasetDescriptions.size()) + " datasets.");
	uint32_t count(1);
	for (DatasetDescription& datasetDescription : _datasetDescriptions)
	{
		m_logger.log("Dataset " + std::to_string(count));
		count++;

		ot::UIDList parameterIDs = addCampaignContextDataToParameters(datasetDescription);
		addCampaignContextDataToQuantities(datasetDescription, parameterIDs);
	}
	const ot::UID newSeriesID = createNewSeries(_datasetDescriptions, _seriesName, _seriesMetadata);
	m_quantitiesUpForStorageByLabel.clear();
	m_parameterUpForStorageByLabel.clear();
	return newSeriesID;
}

void ResultCollectionExtender::processDataPoints(DatasetDescription* _dataDescription, uint64_t _seriesMetadataIndex)
{
	//First we create ways of accessing the parameters
	std::list<ot::Variable> sharedParameterValues;
	std::list<ot::UID> parameterIndicesConstant, parameterIndicesNonConstant;
	std::list<std::list<ot::Variable>::const_iterator> allParameterValueIt;

	auto parameterDescriptions = _dataDescription->getParameters();

	std::map < uint64_t, std::list<std::string>> parameterNamesByNumberOfParameterValues;

	ot::VariableToStringConverter converter;
	for (const auto& parameterDescription : parameterDescriptions)
	{
		auto& parameterMetadata = parameterDescription->getMetadataParameter();
		const ot::UID parameterID = parameterMetadata.parameterUID;

		bool parameterIsConstantForDataset = parameterDescription->getParameterConstantForDataset();
		if (parameterIsConstantForDataset)
		{
			//For parameter that are constant through the entire data set, we simply take the single value they have and add it to every quantity container later on.
			parameterIndicesConstant.push_back(parameterID);
			auto parameterValues = parameterDescription->getMetadataParameter().values;
			assert(parameterValues.size() == 1);
			sharedParameterValues.push_back(*parameterValues.begin());
			m_logger.log("Parameter " + parameterMetadata.parameterName + " has the value " + converter(*parameterValues.begin()) + " for the entire data set");
		}
		else
		{
			//For parameter that need to be iterated together with the quantities, we create here a list of iterators, each pointing at one parameter
			parameterIndicesNonConstant.push_back(parameterID);
			auto parameterValueIt = parameterMetadata.values.begin();
			parameterNamesByNumberOfParameterValues[parameterMetadata.values.size()].push_back(parameterMetadata.parameterName);
			allParameterValueIt.push_back(parameterValueIt);
		}
	}

	//The non-constant parameter are all iterated together with the quantity values. Thus, all non-const parameter are required to have the same number of values.
	if (parameterNamesByNumberOfParameterValues.size() > 1)
	{
		std::string exceptionMessage("An unequal number of parameter values was detected:\n");
		for (auto& parameterNamesByNumberOfParameterValue : parameterNamesByNumberOfParameterValues)
		{
			uint64_t numberOfParameterEntries = parameterNamesByNumberOfParameterValue.first;
			exceptionMessage += std::to_string(numberOfParameterEntries) + " entries: ";
			const std::list<std::string>& parameterNames = parameterNamesByNumberOfParameterValue.second;
			for (const std::string& parameterName : parameterNames)
			{
				exceptionMessage += parameterName;
			}
			exceptionMessage += "\n";
		}
		throw std::exception(exceptionMessage.c_str());
	}
	uint64_t numberOfParameter = parameterNamesByNumberOfParameterValues.begin()->first;

	//Add all the other parameters
	QuantityDescription* currentQuantityDescription = _dataDescription->getQuantityDescription();
	QuantityContainerSerialiser quantityContainerSerialiser(m_collectionName, m_logger);

	std::list<ot::UID> parameterIndices = parameterIndicesConstant;

	parameterIndices.insert(parameterIndices.end(), parameterIndicesNonConstant.begin(), parameterIndicesNonConstant.end());
	quantityContainerSerialiser.storeDataPoints(_seriesMetadataIndex, parameterIndices, sharedParameterValues, allParameterValueIt, numberOfParameter, currentQuantityDescription);

	//DataLakeHelper::createDefaultIndexes(m_collectionName);
}

void ResultCollectionExtender::storeCampaignChanges()
{
	MetadataEntityInterface entityCreator;
	entityCreator.setCallbackData(this->getCallbackData());
	ot::NewModelStateInfo newEntities;
	if (m_requiresUpdateMetadataCampaign && m_seriesMetadataForStorage.size() != 0)
	{
		newEntities = entityCreator.storeCampaign(m_metadataCampaign, m_seriesMetadataForStorage, m_saveModel);
	}
	else if (m_requiresUpdateMetadataCampaign)
	{
		newEntities = entityCreator.storeCampaign(m_metadataCampaign);
	}
	else if (m_seriesMetadataForStorage.size() != 0)
	{
		newEntities = entityCreator.storeCampaign(m_seriesMetadataForStorage, m_saveModel);
	}
	
	if (newEntities.getTopologyEntityIDs().size() > 0)
	{
		ot::ModelServiceAPI::addEntitiesToModel(newEntities, "Updated result data collection", true, m_saveModel);
	}
}

bool ResultCollectionExtender::removeSeries(ot::UID _uid)
{
	bool removed = false;
	for (auto series : m_seriesMetadataForStorage)
	{
		if (series->getSeriesIndex() == _uid)
		{
			m_seriesMetadataForStorage.remove(series);
			removed = true;
			m_metadataCampaign.updateMetadataOverview();
			break;
		}
	}
	return removed;
}

ot::UIDList ResultCollectionExtender::addCampaignContextDataToParameters(DatasetDescription& _dataDescription)
{
	ot::UIDList dependingParameterIDs;
	auto& allParameterDescriptions = _dataDescription.getParameters();
	assert(allParameterDescriptions.size() > 0);
	const auto& parametersInCampaignByLabel =	getMetadataCampaign().getMetadataParameterByLabel();
	for (auto& parameterDescription : allParameterDescriptions)
	{		
		MetadataParameter& newParameter = parameterDescription->getMetadataParameter();
		//Parameter was not dealt with. They are held within a shared_ptr and may occur in multiple dataDescriptions, so the second time they appear, they have an uid.
		if (newParameter.parameterUID == 0)
		{
			std::map<std::string, MetadataParameter*> parameterToConsiderByLabel = parametersInCampaignByLabel;
			parameterToConsiderByLabel.merge(m_parameterUpForStorageByLabel);

			// First we try the parameter name as label and see if such a parameter already exists
			std::string parameterLabel = newParameter.parameterName;
			auto existingParameterByLabel = parameterToConsiderByLabel.find(parameterLabel);
			if(existingParameterByLabel != parameterToConsiderByLabel.end())
			{
				// Now we check if the found parameter is identical with the new parameter
				if (*existingParameterByLabel->second != newParameter)
				{
					// Here we have a new parameter with the same label, so we search for a new, free label
					int counter = 1;
					while (existingParameterByLabel != parameterToConsiderByLabel.end())
					{
						parameterLabel = newParameter.parameterName + "_" + std::to_string(counter);
						counter++;
						existingParameterByLabel = parameterToConsiderByLabel.find(parameterLabel);
					}
					m_logger.log("Parameter " + newParameter.parameterName + " stored as new parameter with label: " + parameterLabel);
					newParameter.parameterUID = findNextFreeParameterIndex(); // New parameter get a new UID
					m_parameterUpForStorageByLabel[newParameter.parameterName] = (&newParameter);
				}
				else
				{
					newParameter.parameterUID = existingParameterByLabel->second->parameterUID; // Here the parameter are identical. Thus we take the already existing UID
					// Label ramains the name
				}
			}
			else
			{
				// Here we introduce a new parameter, whichs label is not in use yet
				newParameter.parameterUID = findNextFreeParameterIndex(); // New parameter get a new UID
				m_parameterUpForStorageByLabel[newParameter.parameterName] = (&newParameter);
				// Label ramains the name
			}
			newParameter.parameterLabel = parameterLabel; // This is either the newly found unique label or the name
		}
		
		dependingParameterIDs.push_back(newParameter.parameterUID);
	}
	return dependingParameterIDs;
}

void ResultCollectionExtender::addCampaignContextDataToQuantities(DatasetDescription& _dataDescription, ot::UIDList& _dependingParameterIDs)
{
	QuantityDescription* quantityDescription = _dataDescription.getQuantityDescription();
	assert(quantityDescription != nullptr);
	MetadataQuantity& newQuantity = quantityDescription->getMetadataQuantity();
	
	std::map<std::string,MetadataQuantity*> quantitiesToConsiderByLabel  =  getMetadataCampaign().getMetadataQuantitiesByLabel();
	m_quantitiesUpForStorageByLabel.merge(m_quantitiesUpForStorageByLabel);

	std::string quantityLabel = newQuantity.quantityName;
	auto existingQuantityByLabel = quantitiesToConsiderByLabel.find(quantityLabel);
	if (existingQuantityByLabel != quantitiesToConsiderByLabel.end())
	{
		// Now we check if the found quantity is identical with the new quantity
		if (*existingQuantityByLabel->second != newQuantity)
		{
			// Here we have a new quantity with the same label, so we search for a new, free label
			int counter = 1;
			while (existingQuantityByLabel != quantitiesToConsiderByLabel.end())
			{
				quantityLabel = newQuantity.quantityName+ "_" + std::to_string(counter);
				counter++;
				existingQuantityByLabel = quantitiesToConsiderByLabel.find(quantityLabel);
			}
			m_logger.log("Quantity " + newQuantity.quantityName+ " stored as new quantity with label: " + quantityLabel);
			newQuantity.quantityIndex = findNextFreeParameterIndex(); // New quantity get a new UID
			m_quantitiesUpForStorageByLabel[newQuantity.quantityName] = (&newQuantity);
		}
		else
		{
			newQuantity.quantityIndex= existingQuantityByLabel->second->quantityIndex; // Here the quantities are identical. Thus we take the already existing UID
			newQuantity.quantityLabel = existingQuantityByLabel->second->quantityLabel;
			newQuantity.m_tupleDescription = existingQuantityByLabel->second->m_tupleDescription;
			newQuantity.dataDimensions = existingQuantityByLabel->second->dataDimensions;
			// Label ramains the name
		}
	}
	else
	{
		newQuantity.quantityIndex = findNextFreeParameterIndex(); // New quantity get a new UID
		m_quantitiesUpForStorageByLabel[newQuantity.quantityName] = (&newQuantity);
	}
	newQuantity.quantityLabel= quantityLabel; // This is either the newly found unique label or the name
	newQuantity.dependingParameterIds = std::vector<ot::UID>(_dependingParameterIDs.begin(), _dependingParameterIDs.end());
	m_logger.log("Quantity depends on " + std::to_string(_dependingParameterIDs.size()) + " parameter.");
}

ot::UID ResultCollectionExtender::createNewSeries(std::list<DatasetDescription>& _dataDescription, const std::string& _seriesName, const ot::JsonDocument& _seriesMetadata)
{
	uint64_t seriesIndex;
	const MetadataSeries* existingSeries = findMetadataSeries(_seriesName);

	//Series with the same name exists already in the campaign.
	std::string seriesLabel(_seriesName);
	int i = 0;
	while (existingSeries != nullptr)
	{
		i++;
		seriesLabel = _seriesName + "_" + std::to_string(i);
		existingSeries = findMetadataSeries(seriesLabel);
	}
	seriesIndex = findNextFreeSeriesIndex();

	MetadataSeries newSeries(_seriesName);
	newSeries.setLabel(seriesLabel);
	m_logger.log("Series received unique label:" + seriesLabel);

	ot::UID seriesID = findNextFreeSeriesIndex();
	newSeries.setIndex(seriesID);

	addMetadataToSeries(_dataDescription, newSeries);
	
	if (!_seriesMetadata.ObjectEmpty())
	{
		newSeries.setMetadata(_seriesMetadata);
	}

	m_metadataCampaign.addSeriesMetadata(std::move(newSeries));
	m_seriesMetadataForStorage.push_back(&m_metadataCampaign.getSeriesMetadata().back());
	m_metadataCampaign.updateMetadataOverviewFromLastAddedSeries();

	return seriesID;
}

void ResultCollectionExtender::addMetadataToSeries(std::list<DatasetDescription>& _dataDescription, MetadataSeries& _newSeries)
{
	std::map<ot::UID,MetadataQuantity*> uniqueQuantities;
	std::map<ot::UID,MetadataParameter> uniqueParameters;
	for (DatasetDescription& dataDescription : _dataDescription)
	{
		QuantityDescription* quantityDescription = dataDescription.getQuantityDescription();
		MetadataQuantity& quantity = quantityDescription->getMetadataQuantity();

		auto uniqueQuantity = uniqueQuantities.find(quantity.quantityIndex);
		if (uniqueQuantity == uniqueQuantities.end())
		{
			uniqueQuantities[quantity.quantityIndex] = &quantity;
		}

		auto& allParameters = dataDescription.getParameters();
		for (auto parameterDescription : allParameters)
		{
			MetadataParameter& parameter = parameterDescription->getMetadataParameter();

			auto uniqueParameter = uniqueParameters.find(parameter.parameterUID);
			if (uniqueParameter == uniqueParameters.end())
			{
				MetadataParameter newParameter(parameter);
				newParameter.values.sort();
				newParameter.values.unique();
				uniqueParameters[parameter.parameterUID] = std::move(newParameter);
			}
			else
			{
				uniqueParameter->second.values.insert(uniqueParameter->second.values.end(), parameter.values.begin(), parameter.values.end());
				uniqueParameter->second.values.sort();
				uniqueParameter->second.values.unique();
			}
		}
	}
	for (auto& uniqueQuantity : uniqueQuantities)
	{
		PRE(quantityIsCorrectlySet(*uniqueQuantity.second));
		_newSeries.addQuantity(*uniqueQuantity.second);
	}
	for (auto& uniqueParameter : uniqueParameters)
	{
		PRE(parameterIsCorrectlySet(uniqueParameter.second));
		_newSeries.addParameter(std::move(uniqueParameter.second));
	}
}

const uint64_t ResultCollectionExtender::findNextFreeSeriesIndex()
{
	return m_modelComponent->createEntityUID();
}

const uint64_t ResultCollectionExtender::findNextFreeQuantityIndex()
{
	return m_modelComponent->createEntityUID();
}

const uint64_t ResultCollectionExtender::findNextFreeParameterIndex()
{
	return m_modelComponent->createEntityUID();
}

bool ResultCollectionExtender::quantityIsCorrectlySet(MetadataQuantity& _quantity)
{
	bool correctlySet =
		_quantity.quantityIndex != 0 &&
		_quantity.dataDimensions.size() > 0 &&
		_quantity.dependingParameterIds.size() > 0 &&
		_quantity.quantityLabel != "" &&
		_quantity.quantityName != "";
	//List of meta data may be empty
	return correctlySet;
}

bool ResultCollectionExtender::parameterIsCorrectlySet(MetadataParameter& _parameter)
{
	bool correctlySet =
		_parameter.parameterLabel != "" &&
		_parameter.parameterName != "" &&
		_parameter.parameterUID != 0 &&
		_parameter.typeName != "" &&
		_parameter.values.size() > 0;
	//Unit and list of meta data may be empty
	return correctlySet;
}

