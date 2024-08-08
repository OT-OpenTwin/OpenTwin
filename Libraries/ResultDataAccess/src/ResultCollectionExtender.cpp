#include "ResultCollectionExtender.h"
#include "MetadataEntryComperator.h"
#include <set>
#include <cassert>
#include <algorithm>
#include "MetadataEntityInterface.h"

#include "QuantityDescriptionCurve.h"
#include "QuantityDescriptionSParameter.h"
#include "OTCore/DefensiveProgramming.h"
#include "QuantityContainerSerialiser.h"

ResultCollectionExtender::ResultCollectionExtender(const std::string& _collectionName, ot::components::ModelComponent& _modelComponent, ClassFactory* _classFactory, const std::string& _ownerServiceName)
	:ResultMetadataAccess(_collectionName,&_modelComponent,_classFactory), m_requiresUpdateMetadataCampaign(false), m_ownerServiceName(_ownerServiceName)
{}

ot::UID ResultCollectionExtender::buildSeriesMetadata(std::list<DatasetDescription*>& _datasetDescriptions, const std::string& _seriesName, std::list<std::shared_ptr<MetadataEntry>>& _seriesMetadata)
{
	for (DatasetDescription* datasetDescription : _datasetDescriptions)
	{
		ot::UIDList parameterIDs = addCampaignContextDataToParameters(*datasetDescription);
		addCampaignContextDataToQuantities(*datasetDescription, parameterIDs);
	}
	const ot::UID newSeriesID = createNewSeries(_datasetDescriptions, _seriesName, _seriesMetadata);
	return newSeriesID;
}

ot::UID ResultCollectionExtender::buildSeriesMetadata(std::list<DatasetDescription*>& _datasetDescriptions, const std::string& _seriesName, std::list<std::shared_ptr<MetadataEntry>>&& _seriesMetadata)
{
	for (DatasetDescription* datasetDescription : _datasetDescriptions)
	{
		ot::UIDList parameterIDs = addCampaignContextDataToParameters(*datasetDescription);
		addCampaignContextDataToQuantities(*datasetDescription, parameterIDs);
	}
	const ot::UID newSeriesID = createNewSeries(_datasetDescriptions, _seriesName, _seriesMetadata);
	return newSeriesID;
}

bool ResultCollectionExtender::campaignMetadataWithSameNameExists(std::shared_ptr<MetadataEntry> _otherMetadata)
{
	auto& allMetadata = m_metadataCampaign.getMetaData();
	return allMetadata.find(_otherMetadata->getEntryName()) != allMetadata.end();
}

bool ResultCollectionExtender::campaignMetadataWithSameValueExists(std::shared_ptr<MetadataEntry> _otherMetadata)
{
	auto& allMetadata = m_metadataCampaign.getMetaData();
	auto metadataEntryByName = allMetadata.find(_otherMetadata->getEntryName());
	if (metadataEntryByName == allMetadata.end())
	{
		return false;
	}
	else
	{
		std::shared_ptr<MetadataEntry> metadataEntry = metadataEntryByName->second;
		MetadataEntryComperator comperator;
		return comperator(metadataEntry, _otherMetadata);
	}
}

void ResultCollectionExtender::processDataPoints(DatasetDescription* _dataDescription, uint64_t _seriesMetadataIndex)
{
	std::list<ot::Variable> sharedParameterValues;
	std::list<ot::UID> parameterIndicesConstant, parameterIndicesNonConstant;
	std::list<std::list<ot::Variable>::const_iterator> allParameterValueIt;

	auto parameterDescriptions = _dataDescription->getParameters();

	std::map < uint64_t, std::list<std::string>> parameterNamesByNumberOfParameterValues;

	for (const auto& parameterDescription : parameterDescriptions)
	{
		auto& parameterMetadata = parameterDescription->getMetadataParameter();
		const ot::UID parameterID = parameterMetadata.parameterUID;

		bool parameterIsConstantForDataset = parameterDescription->getParameterConstantForDataset();
		if (parameterIsConstantForDataset)
		{
			parameterIndicesConstant.push_back(parameterID);
			auto parameterValues = parameterDescription->getMetadataParameter().values;
			assert(parameterValues.size() == 1);
			sharedParameterValues.push_back(*parameterValues.begin());
		}
		else
		{
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
	QuantityContainerSerialiser quantityContainerSerialiser(m_collectionName);

	std::list<ot::UID> parameterIndices = parameterIndicesConstant;
	parameterIndices.insert(parameterIndices.end(), parameterIndicesNonConstant.begin(), parameterIndicesNonConstant.end());
	quantityContainerSerialiser.storeDataPoints(_seriesMetadataIndex, parameterIndices, sharedParameterValues, allParameterValueIt, numberOfParameter, currentQuantityDescription);
}

void ResultCollectionExtender::addCampaignMetadata(std::shared_ptr<MetadataEntry> _metadata)
{
	m_requiresUpdateMetadataCampaign = true;
	m_metadataCampaign.addMetaInformation(_metadata->getEntryName(), _metadata);
}

void ResultCollectionExtender::storeCampaignChanges()
{
	MetadataEntityInterface entityCreator(m_ownerServiceName);
	if (m_requiresUpdateMetadataCampaign && m_seriesMetadataForStorage.size() != 0)
	{
		entityCreator.storeCampaign(*m_modelComponent, m_metadataCampaign, m_seriesMetadataForStorage, m_saveModel);
	}
	else if (m_requiresUpdateMetadataCampaign)
	{
		entityCreator.storeCampaign(*m_modelComponent, m_metadataCampaign);
	}
	else if (m_seriesMetadataForStorage.size() != 0)
	{
		entityCreator.storeCampaign(*m_modelComponent, m_seriesMetadataForStorage, m_saveModel);
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

	//Muss die bereits behandelten aus der dataset liste berücksichtigen, ansonsten könnte labeluneindeutig werden
	ot::UIDList dependingParameterIDs;
	auto& allParameterDescriptions = _dataDescription.getParameters();

	for (auto& parameterDescription : allParameterDescriptions)
	{
		MetadataParameter& parameter = parameterDescription->getMetadataParameter();
		//Parameter was not dealed with.
		if (parameter.parameterUID == 0)
		{
			const std::string parameterLabel = parameter.parameterName;
			const MetadataParameter* existingParameter = findMetadataParameter(parameterLabel);

			//Parameter exists in the campaign.
			if (existingParameter != nullptr)
			{
				if (!(*existingParameter == parameter))
				{
					//Parameter have the same name, but are in fact different. We need to set a new label and ID
					int counter = 1;
					std::string newParameterLabel = "";
					do
					{
						newParameterLabel = parameter.parameterName + "_" + std::to_string(counter);
						counter++;
						existingParameter = findMetadataParameter(newParameterLabel);
					} while (existingParameter != nullptr);
					parameter.parameterLabel = newParameterLabel;
					parameter.parameterUID = findNextFreeParameterIndex();
				}
				else
				{
					//Parameter are identically
					parameter.parameterLabel = existingParameter->parameterLabel;
					parameter.parameterUID = existingParameter->parameterUID;
				}
			}
			else
			{
				parameter.parameterLabel = parameter.parameterName;
				parameter.parameterUID = findNextFreeParameterIndex();
			}
		}

		dependingParameterIDs.push_back(parameter.parameterUID);
	}
	return dependingParameterIDs;
}

void ResultCollectionExtender::addCampaignContextDataToQuantities(DatasetDescription& _dataDescription, ot::UIDList& _dependingParameterIDs)
{
	QuantityDescription* quantityDescription = _dataDescription.getQuantityDescription();
	MetadataQuantity& quantity = quantityDescription->getMetadataQuantity();

	std::string quantityLabel = quantity.quantityName;
	const MetadataQuantity* existingQuantity = findMetadataQuantity(quantityLabel);

	if (existingQuantity != nullptr)
	{
		//Quantity exists already by name in campaign and is in fact identically with the existing one.
		if (*existingQuantity == quantity)
		{
			quantityLabel = existingQuantity->quantityLabel; //value descriptions are identical and the first one is later used to set the quantityUID.
		}
		else
		{
			//Quantities have the same name, but are in fact different. We need to set a new label and ID
			int counter = 1;
			const MetadataQuantity* stillExisting = nullptr;
			do
			{
				quantityLabel = quantity.quantityName + "_" + std::to_string(counter);
				counter++;
				stillExisting= findMetadataQuantity(quantityLabel);
			} while (stillExisting != nullptr);
			quantity.quantityLabel = quantityLabel;

			ot::UID newQuantityIndex = 0;
			
			//First possible difference: The quantities are the same, except of a difference in the value descriptions:
			//We need to figure out which of the value descriptions is different or newly added.
			for (auto& newValueDescription : quantity.valueDescriptions)
			{
				//Check if this valuedescription is the same in both quanities and 
				for (auto& existingValueDescription : existingQuantity->valueDescriptions)
				{
					if (newValueDescription == existingValueDescription)
					{
						newValueDescription.quantityIndex = existingValueDescription.quantityIndex;
						newValueDescription.quantityValueLabel = existingValueDescription.quantityValueLabel;
						break;
					}
				}
				
				//This value description is indeed new
				if (newValueDescription.quantityIndex == 0)
				{
					newValueDescription.quantityIndex = findNextFreeQuantityIndex();
					//The first new value description ID is also used for the quantity.
					if (newQuantityIndex == 0)
					{
						newQuantityIndex = newValueDescription.quantityIndex;
					}
					newValueDescription.quantityValueLabel = newValueDescription.quantityValueName;
				}
			}

			//Second possible difference: The quantities differ in the data dimensionality
			if (newQuantityIndex == 0)
			{
				newQuantityIndex = findNextFreeQuantityIndex();
			}
			quantity.quantityIndex = newQuantityIndex;
		}
	}
	else
	{

		quantity.quantityLabel = quantity.quantityName;
		for (auto& quantityValueDescription : quantity.valueDescriptions)
		{
			quantityValueDescription.quantityIndex = findNextFreeQuantityIndex();
			quantityValueDescription.quantityValueLabel = quantityValueDescription.quantityValueName;
		}
	}
	if (quantity.quantityIndex == 0)
	{
		quantity.quantityIndex = quantity.valueDescriptions.begin()->quantityIndex; //The entire quantity holds the index of the first value description.
	}
	quantity.dependingParameterIds = std::vector<ot::UID>(_dependingParameterIDs.begin(), _dependingParameterIDs.end());
}

ot::UID ResultCollectionExtender::createNewSeries(std::list<DatasetDescription*>& _dataDescription, const std::string& _seriesName, std::list<std::shared_ptr<MetadataEntry>>& _seriesMetadata)
{
	uint64_t seriesIndex;
	const MetadataSeries* existingSeries = findMetadataSeries(_seriesName);

	//Series with the same name exists already in the campaign.
	std::string seriesLabel;
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
	ot::UID seriesID = findNextFreeSeriesIndex();
	newSeries.setIndex(seriesID);
	
	AddMetadataToSeries(_dataDescription,newSeries);
	for (auto metadata : _seriesMetadata)
	{
		newSeries.addMetadata(metadata);
	}

	m_metadataCampaign.addSeriesMetadata(std::move(newSeries));
	m_seriesMetadataForStorage.push_back(&m_metadataCampaign.getSeriesMetadata().back());
	m_metadataCampaign.updateMetadataOverviewFromLastAddedSeries();

	return seriesID;
}

void ResultCollectionExtender::AddMetadataToSeries(std::list<DatasetDescription*>& _dataDescription, MetadataSeries& _newSeries)
{
	for (DatasetDescription* dataDescription : _dataDescription)
	{
		QuantityDescription* quantityDescription = dataDescription->getQuantityDescription();
		MetadataQuantity& quantity = quantityDescription->getMetadataQuantity();
		_newSeries.addQuantity(quantity);


		auto& allParameters = dataDescription->getParameters();
		for (auto parameterDescription : allParameters)
		{
			MetadataParameter& parameter =	parameterDescription->getMetadataParameter();
			//By know all parameter have UIDs, but still some parameter may be shared between different datasets.
			//Thus, before we add a parameter to the series, we check if that already happened.
			const std::list<MetadataParameter>& alreadyAddedParameters = _newSeries.getParameter();
			bool found = false;
			for (auto& alreadyAddedParameter : alreadyAddedParameters)
			{
				if (alreadyAddedParameter.parameterUID == parameter.parameterUID)
				{
					found = true;
					break;
				}
			}

			if (!found)
			{
				MetadataParameter newParameter(parameter);
				newParameter.values.sort();
				newParameter.values.unique();
				_newSeries.addParameter(std::move(newParameter));
			}
		}
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

