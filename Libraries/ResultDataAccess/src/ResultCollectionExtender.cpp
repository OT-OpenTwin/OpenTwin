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
#include "OTCore/VariableToStringConverter.h"

ResultCollectionExtender::ResultCollectionExtender(const std::string& _collectionName, ot::components::ModelComponent& _modelComponent, ClassFactory* _classFactory, const std::string& _ownerServiceName)
	:ResultCollectionMetadataAccess(_collectionName,&_modelComponent,_classFactory), m_requiresUpdateMetadataCampaign(false), m_ownerServiceName(_ownerServiceName)
{}

ot::UID ResultCollectionExtender::buildSeriesMetadata(std::list<DatasetDescription>& _datasetDescriptions, const std::string& _seriesName, std::list<std::shared_ptr<MetadataEntry>>& _seriesMetadata)
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
	return newSeriesID;
}

ot::UID ResultCollectionExtender::buildSeriesMetadata(std::list<DatasetDescription>& _datasetDescriptions, const std::string& _seriesName, std::list<std::shared_ptr<MetadataEntry>>&& _seriesMetadata)
{
	return buildSeriesMetadata(_datasetDescriptions, _seriesName, _seriesMetadata);
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

	ot::UIDList dependingParameterIDs;
	auto& allParameterDescriptions = _dataDescription.getParameters();
	assert(allParameterDescriptions.size() > 0);
	for (auto& parameterDescription : allParameterDescriptions)
	{
		MetadataParameter& newParameter = parameterDescription->getMetadataParameter();
		//Parameter was not dealed with.
		if (newParameter.parameterUID == 0)
		{
			const std::string parameterName = newParameter.parameterName;
			std::list<MetadataParameter*> existingParameterInSeries = findParameterWithSameName(parameterName);

			//Since multiple dataset descriptions are considered, it may be that duplicates exists amongst them
			std::list<MetadataParameter*> existingParameters;
			if (!existingParameterInSeries.empty())
			{
				m_logger.log("Parameter name " + parameterName + " already exists in this campaign");
			}
			existingParameters.splice(existingParameters.end(), existingParameterInSeries);
			auto parameterUpForStorageByName = m_parameterUpForStorageByName.find(parameterName);
			if (parameterUpForStorageByName != m_parameterUpForStorageByName.end())
			{
				m_logger.log("Parameter name " + parameterName + " appears more then once in the dataset");
				std::list<MetadataParameter*> existingParameterUpForStorage = parameterUpForStorageByName->second;
				existingParameters.splice(existingParameters.end(), existingParameterUpForStorage);
			}

			//Parameter exists in the campaign.
			if (existingParameters.size()>0)
			{
				MetadataParameter* identicalParameter = nullptr;
				for (auto parameter : existingParameters)
				{
					if (newParameter == *parameter)
					{
						m_logger.log("Parameter " + parameterName + " is identical to an already existing parameter entry.");
						identicalParameter = parameter;
					}
				}

				if (identicalParameter == nullptr)
				{
					//Parameter have the same name, but are in fact different. We need to set a new label and ID
					int counter = 1;
					bool newLabelFound = true;
					std::string newParameterLabel = "";
					do
					{
						newParameterLabel = newParameter.parameterName+ "_" + std::to_string(counter);
						counter++;
						newLabelFound = true;
						for (MetadataParameter* existingParameter: existingParameters)
						{
							if (existingParameter->parameterLabel == newParameterLabel)
							{
								newLabelFound = false;
								break;
							}
						}

					} while (!newLabelFound);
					newParameter.parameterLabel = newParameterLabel;
					m_logger.log("Parameter was given a new unique label: " + newParameterLabel);
				}
				else
				{
					//Parameter are identically
					newParameter.parameterLabel = identicalParameter->parameterLabel;
					newParameter.parameterUID = identicalParameter->parameterUID;
				}
			}
			else
			{
				newParameter.parameterLabel = newParameter.parameterName;
			}
			if (newParameter.parameterUID == 0)
			{
				m_logger.log("Parameter " + parameterName + " stored as new parameter with label: " + newParameter.parameterLabel);
				newParameter.parameterUID = findNextFreeParameterIndex();
				m_parameterUpForStorageByName[newParameter.parameterName].push_back(&newParameter);
			}
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

	std::string quantityName = newQuantity.quantityName;
	std::list<MetadataQuantity*> existingQuantitiesInSeries = findQuantityWithSameName(quantityName);
	
	if (!existingQuantitiesInSeries.empty())
	{
		m_logger.log("Quantity name " + quantityName+ " already exists in this campaign");
	}
	//Since multiple dataset descriptions are considered, it may be that duplicates exists amongst them
	std::list<MetadataQuantity*> existingQuantities;
	existingQuantities.splice(existingQuantities.end(),existingQuantitiesInSeries);
	auto quantityUpForStorageByName = m_quantitiesUpForStorageByName.find(quantityName);
	if (quantityUpForStorageByName != m_quantitiesUpForStorageByName.end())
	{
		m_logger.log("Quantity name " + quantityName + " appears more then once in the dataset");
		std::list<MetadataQuantity*> existingQuantityUpForStorage = quantityUpForStorageByName->second;
		existingQuantities.splice(existingQuantities.end(), existingQuantityUpForStorage);
	}
		
	if (existingQuantities.size() >0)
	{
		MetadataQuantity* identicalQuantity =  nullptr;
		for (MetadataQuantity* existingQuantity : existingQuantities)
		{
			if (newQuantity == *existingQuantity)
			{
				m_logger.log("Quantity " + quantityName + " is identical to an already existing quantity entry.");
				identicalQuantity = existingQuantity;
			}
		}

		if (identicalQuantity != nullptr)
		{
			//Quantity exists already by name in campaign and is in fact identically with the existing one.
			newQuantity.quantityLabel =  identicalQuantity->quantityLabel;
			newQuantity.quantityIndex=  identicalQuantity->quantityIndex;
			newQuantity.valueDescriptions = identicalQuantity->valueDescriptions;
			newQuantity.dataDimensions = identicalQuantity->dataDimensions;
			
		}
		else
		{
			//Quantities have the same name, but are in fact different. We need to set a new label and ID
			int counter = 1;
			bool newLabelFound = true;
			std::string newQuantityLabel = "";
			do
			{
				newQuantityLabel = newQuantity.quantityName + "_" + std::to_string(counter);
				counter++;
				newLabelFound = true;
				for (MetadataQuantity* existingQuantity : existingQuantities)
				{
					if (existingQuantity->quantityLabel == newQuantityLabel)
					{
						newLabelFound = false;
						break;
					}
				}

			} while (!newLabelFound);
			newQuantity.quantityLabel = newQuantityLabel;
			m_logger.log("Quantity was given a new unique label: " + newQuantityLabel);
			//Discarded aproach, which copies the shared value descriptions
			// ot::UID newQuantityIndex = 0;
			////First possible difference: The quantities are the same, except of a difference in the value descriptions:
			////We need to figure out which of the value descriptions is different or newly added.
			////for (auto& newValueDescription : quantity.valueDescriptions)
			////{
			////	Check if this valuedescription is the same in both quanities and 
			////	for (auto& existingValueDescription : existingQuantity->valueDescriptions)
			////	{
			////		if (newValueDescription == existingValueDescription)
			////		{
			////			newValueDescription.quantityIndex = existingValueDescription.quantityIndex;
			////			newValueDescription.quantityValueLabel = existingValueDescription.quantityValueLabel;
			////			break;
			////		}
			////	}
			////	
			////	This value description is indeed new
			////	if (newValueDescription.quantityIndex == 0)
			////	{
			////		newValueDescription.quantityIndex = findNextFreeQuantityIndex();
			////		The first new value description ID is also used for the quantity.
			////		if (newQuantityIndex == 0)
			////		{
			////			newQuantityIndex = newValueDescription.quantityIndex;
			////		}
			////		newValueDescription.quantityValueLabel = newValueDescription.quantityValueName;
			////	}
			////}

			////Second possible difference: The quantities differ in the data dimensionality
			////if (newQuantityIndex == 0)
			////{
			////	newQuantityIndex = findNextFreeQuantityIndex();
			////}
			////quantity.quantityIndex = newQuantityIndex;
		}
	}
	else
	{
		newQuantity.quantityLabel = newQuantity.quantityName;
	}
	
	newQuantity.dependingParameterIds = std::vector<ot::UID>(_dependingParameterIDs.begin(), _dependingParameterIDs.end());
	m_logger.log("Quantity depends on " + std::to_string(_dependingParameterIDs.size()) + " parameter.");
	if (newQuantity.quantityIndex == 0)
	{
		std::set<std::string> valueDescriptionLabels;
		for (MetadataQuantityValueDescription& valueDescription : newQuantity.valueDescriptions)
		{
			std::string valueDescriptionLabel = valueDescription.quantityValueName;
			int count(0);
			bool labelExists = false;
			do
			{
				labelExists = valueDescriptionLabels.find(valueDescriptionLabel) != valueDescriptionLabels.end();
				if (labelExists)
				{
					valueDescriptionLabel = valueDescription.quantityValueName + "_" + std::to_string(count);
					count++;
				}
			} while (labelExists);
			valueDescription.quantityValueLabel = valueDescriptionLabel;
			m_logger.log("Quantity got new value description label" + valueDescriptionLabel);
			valueDescriptionLabels.insert(valueDescription.quantityValueLabel);
		}

		for (auto& quantityValueDescription : newQuantity.valueDescriptions)
		{
			quantityValueDescription.quantityIndex = findNextFreeQuantityIndex();
		}

		newQuantity.quantityIndex = newQuantity.valueDescriptions.begin()->quantityIndex;
	}

	//The quantity shall only be stored if the quantity was not stored for this series yet.
	quantityUpForStorageByName = m_quantitiesUpForStorageByName.find(quantityName);
	if (quantityUpForStorageByName != m_quantitiesUpForStorageByName.end())
	{
		m_logger.log("Quantity " + newQuantity.quantityName + " is stored as new quantity.");
		m_quantitiesUpForStorageByName[newQuantity.quantityName].push_back(&newQuantity);
	}
}

ot::UID ResultCollectionExtender::createNewSeries(std::list<DatasetDescription>& _dataDescription, const std::string& _seriesName, std::list<std::shared_ptr<MetadataEntry>>& _seriesMetadata)
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
	
	addMetadataToSeries(_dataDescription,newSeries);
	for (auto metadata : _seriesMetadata)
	{
		newSeries.addMetadata(metadata);
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
		_quantity.quantityName != "" &&
		_quantity.valueDescriptions.size() > 0;
	//List of meta data may be empty
	for (auto& valueDescription : _quantity.valueDescriptions)
	{
		correctlySet &=
			valueDescription.dataTypeName != "" &&
			valueDescription.quantityIndex != 0;
		//label, name and unit can be empty
	}
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

