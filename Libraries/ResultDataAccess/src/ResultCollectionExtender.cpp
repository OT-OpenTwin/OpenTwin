#include "ResultCollectionExtender.h"
#include "MetadataEntryComperator.h"
#include <set>
#include <cassert>
#include <algorithm>
#include "MetadataEntityInterface.h"

#include "QuantityDescriptionCurve.h"
#include "QuantityDescriptionSParameter.h"
#include "OTCore/DefensiveProgramming.h"
//#include "Document/DocumentAccess.h"
ResultCollectionExtender::ResultCollectionExtender(const std::string& _collectionName, ot::components::ModelComponent& _modelComponent, ClassFactory* _classFactory, const std::string& _ownerServiceName)
	:ResultMetadataAccess(_collectionName,&_modelComponent,_classFactory), m_requiresUpdateMetadataCampaign(false), m_ownerServiceName(_ownerServiceName), m_dataStorageAccess("Projects", _collectionName)
{
	m_quantityContainer.reserve(m_bufferSize);
}

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

void ResultCollectionExtender::processDataPoints(DatasetDescription1D& _dataDescription, uint64_t _seriesMetadataIndex)
{
	std::list<ot::UID> parameterIndices;
	std::list<ot::Variable> sharedParameterValues;

	//In the extraction of parameters, the first is the x-axis. This is the only varying parameter for a curve
	auto xParameter = _dataDescription.getXAxisParameter();
	assert(xParameter->values.size() > 1);

	//Add the first parameter abbreviation, which is corresponding to the x-axis parameter
	const std::string xAxisParameterName = xParameter->parameterName;
	parameterIndices.push_back(xParameter->parameterUID);

	//Add all the other parameters
	auto allParameter = _dataDescription.getParameters();
	if (allParameter.size() > 1)
	{
		//The first parameter is the x-axis parameter
		auto parameter = allParameter.begin()++;
		for (; parameter != allParameter.end(); parameter++)
		{
			//Those parameter which are fixed for one curve, so they are suppose to have only one entry.
			if ((*parameter)->values.size() != 1)
			{
				//Too harsh ?
				std::string message = "Cannot import data. Appart from the x-axis parameter, there is the parameter " + (*parameter)->parameterName + ", which has more then one value within a single curve.";
				throw std::exception(message.c_str());
			}
			else
			{
				parameterIndices.push_back((*parameter)->parameterUID);
				sharedParameterValues.push_back((*parameter)->values.front());
			}
		}
	}
	

	QuantityDescription* currentQuantityDescription = _dataDescription.getQuantityDescription();
	MetadataQuantity& quantityMetadata = currentQuantityDescription->getMetadataQuantity();
	auto xParameterValue = xParameter->values.begin();
	size_t numberOfParameter = xParameter->values.size();

	flushQuantityContainer();
	QuantityDescriptionCurve* curveDescription = dynamic_cast<QuantityDescriptionCurve*>(currentQuantityDescription);
	if (curveDescription != nullptr)
	{
		const std::vector<ot::Variable>& realValues = curveDescription->getQuantityValuesReal();
		const std::vector<ot::Variable>& imagValues = curveDescription->getQuantityValuesImag();
		std::vector<ot::Variable>::const_iterator realValueItt, imagValueItt;

		bool hasImagValues(false), hasRealValues(false);
		ot::UID imagQuantityID(0), realQuantityID(0);

		if (!imagValues.empty())
		{
			assert(numberOfParameter == imagValues.size());
			hasImagValues = true;
			imagValueItt = imagValues.begin();
			auto imagValueDescription = quantityMetadata.valueDescriptions.begin()++;
			imagQuantityID = imagValueDescription->quantityIndex;
		}
		if (!realValues.empty())
		{
			assert(numberOfParameter == realValues.size());
			hasRealValues = true;
			realValueItt = realValues.begin();
			auto realValueDescription = quantityMetadata.valueDescriptions.begin();
			realQuantityID = realValueDescription->quantityIndex;
		}
		if (!hasRealValues && !hasImagValues)
		{
			return;
		}

		setBucketSize(1);

		for (size_t i = 0; i < numberOfParameter; i++)
		{
			std::list<ot::Variable> currentParameterValues{ *xParameterValue };
			currentParameterValues.insert(currentParameterValues.end(), sharedParameterValues.begin(), sharedParameterValues.end());
			if (hasRealValues)
			{
				addQuantityContainer(_seriesMetadataIndex, parameterIndices, currentParameterValues, realQuantityID, *realValueItt);
				if (realValueItt != realValues.end())
				{
					realValueItt++;
				}
			}

			if (hasImagValues)
			{
				addQuantityContainer(_seriesMetadataIndex, parameterIndices, currentParameterValues, imagQuantityID, *imagValueItt);
				if (imagValueItt != imagValues.end())
				{
					imagValueItt++;
				}
			}
		}
	}
	else
	{
		QuantityDescriptionSParameter* sParameterDescription = dynamic_cast<QuantityDescriptionSParameter*>(currentQuantityDescription);
		assert(sParameterDescription != nullptr);
		PRE(sParameterDescription->getNumberOfFirstValues() == sParameterDescription->getNumberOfSecondValues());

		const uint32_t numberOfPorts = sParameterDescription->getMetadataQuantity().dataDimensions.front();
		setBucketSize(numberOfPorts * numberOfPorts);
		auto firstValueDescription = quantityMetadata.valueDescriptions.begin();
		auto secondValueDescription = quantityMetadata.valueDescriptions.begin()++;

		for (size_t i = 0; i < numberOfParameter; i++)
		{
			std::list<ot::Variable> currentParameterValues{ *xParameterValue };
			currentParameterValues.insert(currentParameterValues.end(), sharedParameterValues.begin(), sharedParameterValues.end());

			for (uint32_t row = 0; row < numberOfPorts; row++)
			{
				for (uint32_t column = 0; column < numberOfPorts; column++)
				{
					const ot::Variable& quantityValueEntry = sParameterDescription->getFirstValue(i, row, column);
					addQuantityContainer(_seriesMetadataIndex, parameterIndices, currentParameterValues, firstValueDescription->quantityIndex, quantityValueEntry);
				}
			}


			for (uint32_t row = 0; row < numberOfPorts; row++)
			{
				for (uint32_t column = 0; column < numberOfPorts; column++)
				{
					const ot::Variable& quantityValueEntry = sParameterDescription->getSecondValue(i, row, column);
					addQuantityContainer(_seriesMetadataIndex, parameterIndices, currentParameterValues, secondValueDescription->quantityIndex, quantityValueEntry);
				}
			}


			if (xParameterValue != xParameter->values.end())
			{
				xParameterValue++;
			}
		}
	}
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

ot::UIDList ResultCollectionExtender::addCampaignContextDataToParameters(DatasetDescription& _dataDescription)
{

	//Muss die bereits behandelten aus der dataset liste berücksichtigen, ansonsten könnte labeluneindeutig werden
	ot::UIDList dependingParameterIDs;
	auto& allParameters = _dataDescription.getParameters();

	for (auto& parameter : allParameters)
	{
		//Parameter was not dealed with.
		if (parameter->parameterUID == 0)
		{
			const std::string parameterLabel = parameter->parameterName;
			const MetadataParameter* existingParameter = findMetadataParameter(parameterLabel);

			//Parameter exists in the campaign.
			if (existingParameter != nullptr)
			{
				if (!(*existingParameter == *parameter))
				{
					//Parameter have the same name, but are in fact different. We need to set a new label and ID
					int counter = 1;
					std::string newParameterLabel = "";
					do
					{
						newParameterLabel = parameter->parameterName + "_" + std::to_string(counter);
						counter++;
						existingParameter = findMetadataParameter(newParameterLabel);
					} while (existingParameter != nullptr);
					parameter->parameterLabel = newParameterLabel;
					parameter->parameterUID = findNextFreeParameterIndex();
				}
				else
				{
					//Parameter are identically
					parameter->parameterLabel = existingParameter->parameterLabel;
					parameter->parameterUID = existingParameter->parameterUID;
				}
			}
			else
			{
				parameter->parameterLabel = parameter->parameterName;
				parameter->parameterUID = findNextFreeParameterIndex();
			}
		}

		dependingParameterIDs.push_back(parameter->parameterUID);
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
		for (auto parameter : allParameters)
		{
			//By know all parameter have UIDs, but still some parameter may be shared between different datasets.
			//Thus, before we add a parameter to the series, we check if that already happened.
			const std::list<MetadataParameter>& alreadyAddedParameters = _newSeries.getParameter();
			bool found = false;
			for (auto& alreadyAddedParameter : alreadyAddedParameters)
			{
				if (alreadyAddedParameter.parameterUID == parameter->parameterUID)
				{
					found = true;
					break;
				}
			}

			if (!found)
			{
				_newSeries.addParameter(*parameter);
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

void ResultCollectionExtender::flushQuantityContainer()
{
	for (auto& qc : m_quantityContainer)
	{
		DataStorageAPI::DataStorageResponse response = m_dataStorageAccess.InsertDocumentToResultStorage(qc.getMongoDocument(), false, true);
		if (!response.getSuccess())
		{
			throw std::exception("Insertion of quantity container failed.");
		}
	}
	m_dataStorageAccess.FlushQueuedData();
	m_quantityContainer.clear();
}

void ResultCollectionExtender::addQuantityContainer(ot::UID _seriesIndex, std::list<ot::UID>& _parameterIDs, std::list<ot::Variable>&& _parameterValues, uint64_t _quantityIndex, const ot::Variable& _quantityValue)
{
	if (m_quantityContainer.size() == 0)
	{
		QuantityContainer newContainer(_seriesIndex, _parameterIDs, std::move(_parameterValues), _quantityIndex);
		newContainer.addValue(_quantityValue);
		m_quantityContainer.push_back(std::move(newContainer));
	}
	else
	{
		QuantityContainer* lastAddedQuantityContainer = &m_quantityContainer.back();
		const uint64_t lastAddedQuantityContainerStoredValues = lastAddedQuantityContainer->getValueArraySize();
		if (lastAddedQuantityContainerStoredValues == m_bucketSize)
		{
			if (m_quantityContainer.size() == m_bufferSize)
			{
				flushQuantityContainer();
			}
			QuantityContainer newContainer(_seriesIndex, _parameterIDs, std::move(_parameterValues), _quantityIndex);
			newContainer.addValue(_quantityValue);
			m_quantityContainer.push_back(std::move(newContainer));
		}
		else
		{
			lastAddedQuantityContainer->addValue(_quantityValue);
		}
	}
}

void ResultCollectionExtender::addQuantityContainer(ot::UID _seriesIndex, std::list<ot::UID>& _parameterIDs, std::list<ot::Variable>& _parameterValues, uint64_t _quantityIndex, const ot::Variable& _quantityValue)
{
	if (m_quantityContainer.size() == 0)
	{
		QuantityContainer newContainer(_seriesIndex, _parameterIDs, _parameterValues, _quantityIndex);
		newContainer.addValue(_quantityValue);
		m_quantityContainer.push_back(std::move(newContainer));
	}
	else
	{
		QuantityContainer* lastAddedQuantityContainer = &m_quantityContainer.back();
		const uint64_t lastAddedQuantityContainerStoredValues = lastAddedQuantityContainer->getValueArraySize();
		if (lastAddedQuantityContainerStoredValues == m_bucketSize)
		{
			if (m_quantityContainer.size() == m_bufferSize)
			{
				flushQuantityContainer();
			}
			QuantityContainer newContainer(_seriesIndex, _parameterIDs, _parameterValues, _quantityIndex);
			newContainer.addValue(_quantityValue);
			m_quantityContainer.push_back(std::move(newContainer));
		}
		else
		{
			lastAddedQuantityContainer->addValue(_quantityValue);
		}
	}
}
