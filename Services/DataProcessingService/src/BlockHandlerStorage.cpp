#include "BlockHandler.h"
#include "BlockHandlerStorage.h"
#include "Application.h"

#include "OTCore/GenericDataStructSingle.h"
#include "OTCore/GenericDataStructVector.h"
#include "OTCore/GenericDataStructMatrix.h"

#include "QuantityDescriptionMatrix.h"
#include "QuantityDescriptionCurve.h"
#include "OTCore/FolderNames.h"

BlockHandlerStorage::BlockHandlerStorage(EntityBlockStorage* blockEntityStorage, const HandlerMap& handlerMap)
	:BlockHandler(blockEntityStorage,handlerMap),m_blockEntityStorage(blockEntityStorage)
{
	m_connectorsQuantity = blockEntityStorage->getConnectorsQuantity();
	m_connectorsParameter = blockEntityStorage->getConnectorsParameter();
	m_connectorsMetadata = blockEntityStorage->getConnectorsMetadata();
}

BlockHandlerStorage::~BlockHandlerStorage()
{
	
}

bool BlockHandlerStorage::executeSpecialized()
{

	if (allInputsAvailable())
	{
		auto& classFactory = Application::instance()->getClassFactory();
		const auto modelComponent = Application::instance()->modelComponent();
		const std::string collectionName = Application::instance()->getCollectionName();
		ResultCollectionExtender resultCollectionExtender(collectionName, *modelComponent, &classFactory, OT_INFO_SERVICE_TYPE_DataProcessingService);

		_uiComponent->displayMessage("Executing Storage Block: " + _blockName);
		std::list<DatasetDescription> datasets = std::move(createDatasets());
		const std::string seriesName = ot::FolderNames::DatasetFolder + "/" + m_blockEntityStorage->getSeriesName();
		std::list<std::shared_ptr<MetadataEntry>>seriesMetadata;
		ot::UID seriesID = resultCollectionExtender.buildSeriesMetadata(datasets, seriesName, seriesMetadata);
		
		for (DatasetDescription& dataset : datasets)
		{
			resultCollectionExtender.processDataPoints(&dataset, seriesID);
		}
		resultCollectionExtender.storeCampaignChanges();
		return true;
	}
	else
	{
		return false;
	}
}

bool BlockHandlerStorage::allInputsAvailable()
{
	for (auto& connector : m_connectorsParameter)
	{
		if (_dataPerPort.find(connector.getConnectorName()) == _dataPerPort.end())
		{
			return false;
		}
	}
	
	for (auto& connector : m_connectorsQuantity)
	{
		if (_dataPerPort.find(connector.getConnectorName()) == _dataPerPort.end())
		{
			return false;
		}
	}

	for (auto& connector : m_connectorsMetadata)
	{
		if (_dataPerPort.find(connector.getConnectorName()) == _dataPerPort.end())
		{
			return false;
		}
	}

	return true;
}

std::list<std::shared_ptr<ParameterDescription>> BlockHandlerStorage::createAllParameter()
{
	std::list<std::shared_ptr<ParameterDescription>> allParameter;
	for (auto& connector : m_connectorsParameter)
	{
		
		MetadataParameter newParameter = extractParameter(connector);
		
		const std::string parameterName = connector.getConnectorName();
		auto portData = _dataPerPort.find(parameterName);
		assert(portData != _dataPerPort.end()); //Should have been checked before
		const MetadataParameter* metadataParameterFromPort = portData->second.m_parameter;
		const ParameterProperties selectedProperties = m_blockEntityStorage->getPropertiesParameter(parameterName);
		const std::string portLabel = connector.getConnectorTitle();

		//Now we assign the parameter values and check if the characteristics are matching.
		const std::string errorMessageInvalidNumberOfEntries = "The parameter port " + portLabel + " has been selected as constant through the dataset, but the port received more than one value, which is inconsistent.";
		const std::string errorMessageNoValuesReceived = "The parameter port " + portLabel + " has not received any values.";
		const std::string errorMessageInvalidFormat= "The parameter port " + portLabel + " received a matrix of data, which is not supported. Only single values or lists of values are allowed.";
		
		
		auto& pipelineData = portData->second.m_data;
		auto firstDataEntry = *pipelineData.begin();
		
		if (pipelineData.size() == 0)
		{
			throw std::exception(errorMessageNoValuesReceived.c_str());
		}

		ot::GenericDataStructSingle* singleEntry = dynamic_cast<ot::GenericDataStructSingle*>(firstDataEntry.get());
		if (singleEntry != nullptr)
		{
			newParameter.typeName = singleEntry->getValue().getTypeName();
			if (pipelineData.size() > 1 && selectedProperties.m_propertyConstParameter)
			{
				throw std::exception(errorMessageInvalidNumberOfEntries.c_str());
			}
			else
			{
				for (auto dataEntry : pipelineData)
				{
					singleEntry = dynamic_cast<ot::GenericDataStructSingle*>(dataEntry.get());
					assert(singleEntry != nullptr);
					const ot::Variable& value =	singleEntry->getValue();
					assert(value.getTypeName() == newParameter.typeName);
					newParameter.values.push_back(value);
				}
			}
		}

		ot::GenericDataStructVector* vectorEntry = dynamic_cast<ot::GenericDataStructVector*>(firstDataEntry.get());
		if (vectorEntry != nullptr)
		{
			if (vectorEntry->getNumberOfEntries() == 0)
			{
				throw std::exception(errorMessageNoValuesReceived.c_str());
			}
			newParameter.typeName =	vectorEntry->getValue(0).getTypeName();

			if ((pipelineData.size() > 1 && vectorEntry->getNumberOfEntries() > 1)) //Essentially a matrix of entries
			{
				throw std::exception(errorMessageInvalidFormat.c_str());
			}
			else if ((pipelineData.size() > 1 || vectorEntry->getNumberOfEntries() > 1) && selectedProperties.m_propertyConstParameter) //More then one entry + const parameter property
			{
				throw std::exception(errorMessageInvalidNumberOfEntries.c_str());
			}
			else
			{
				for (auto dataEntry : pipelineData)
				{
					vectorEntry = dynamic_cast<ot::GenericDataStructVector*>(dataEntry.get());
					std::vector<ot::Variable> values =	vectorEntry->getValues();
					assert(values.begin()->getTypeName() == newParameter.typeName);
					newParameter.values.insert(newParameter.values.end(), values.begin(), values.end());
				}
			}
		}
		
		ot::GenericDataStructMatrix* matrixEntry = dynamic_cast<ot::GenericDataStructMatrix*>(firstDataEntry.get());
		if (matrixEntry != nullptr)
		{
			throw std::exception(errorMessageInvalidFormat.c_str());
		}
		allParameter.push_back(std::make_shared< ParameterDescription>(newParameter, selectedProperties.m_propertyConstParameter));
	}
	return allParameter;
}

MetadataParameter BlockHandlerStorage::extractParameter(const ot::Connector& _connector)
{
	const std::string parameterName = _connector.getConnectorName();
	auto portData = _dataPerPort.find(parameterName);
	assert(portData != _dataPerPort.end()); //Should have been checked before
	const MetadataParameter* metadataParameterFromPort = portData->second.m_parameter;
	const ParameterProperties selectedProperties = m_blockEntityStorage->getPropertiesParameter(parameterName);

	MetadataParameter newParameter;
	if (metadataParameterFromPort != nullptr)
	{
		//Data stream carries relevant meta data with it. The user has the option to overwrite these meta data via the entity properties.

		if (selectedProperties.m_propertyName == "")
		{
			newParameter.parameterName = metadataParameterFromPort->parameterName;
		}
		else
		{
			newParameter.parameterName = selectedProperties.m_propertyName;
		}

		if (selectedProperties.m_propertyUnit == "")
		{
			newParameter.unit = metadataParameterFromPort->unit;
		}
		else
		{
			newParameter.unit = selectedProperties.m_propertyUnit;
		}
	}
	else
	{
		if (selectedProperties.m_propertyName == "" || selectedProperties.m_propertyUnit == "")
		{
			const std::string errorMessagePropertyMustBeSet = _connector.getConnectorTitle() + " did not receive meta data from the processing pipeline. All properties for this quantity have to be set manually, which is currently not the case.";
			throw std::exception(errorMessagePropertyMustBeSet.c_str());
		}
		newParameter.parameterName = selectedProperties.m_propertyName;
		newParameter.unit = selectedProperties.m_propertyUnit;
	}
	return newParameter;
}

std::list<DatasetDescription> BlockHandlerStorage::createDatasets()
{
	auto allParameter = createAllParameter();
	std::list<DatasetDescription> allDataDescriptions;

	for (auto& connector : m_connectorsQuantity)
	{

		std::string valueName(""), unit(""), name(""), dataTypeName("");
		extractQuantityProperties(connector, name, unit, valueName);
		QuantityDescription* quantityDescription = extractQuantityDescription(connector, dataTypeName);
		
		
		quantityDescription->setName(name);
		quantityDescription->addValueDescription(valueName, dataTypeName, unit);

		DatasetDescription dataset;
		dataset.setQuantityDescription(quantityDescription);
		dataset.addParameterDescriptions(allParameter);
		allDataDescriptions.push_back(std::move(dataset));
	}
	return allDataDescriptions;
}

QuantityDescription* BlockHandlerStorage::extractQuantityDescription(const ot::Connector& _connector, std::string& _outTypeName)
{
	const std::string portLabel = _connector.getConnectorTitle();
	const std::string quantityName = _connector.getConnectorName();
	auto portData = _dataPerPort.find(quantityName);

	//Now we assign the parameter values and check if the characteristics are matching.
	const std::string errorMessageNoValuesReceived = "The quantity port " + portLabel + " has not received any values.";

	auto& pipelineData = portData->second.m_data;
	auto firstDataEntry = *pipelineData.begin();

	if (pipelineData.size() == 0)
	{
		throw std::exception(errorMessageNoValuesReceived.c_str());
	}

	QuantityDescription* quantityDescription = nullptr;
	ot::GenericDataStructSingle* singleEntry = dynamic_cast<ot::GenericDataStructSingle*>(firstDataEntry.get());
	if (singleEntry != nullptr)
	{
		_outTypeName = singleEntry->getValue().getTypeName();
		auto quantityDescriptionCurve = std::make_unique<QuantityDescriptionCurve>();
		for (auto dataEntry : pipelineData)
		{
			singleEntry = dynamic_cast<ot::GenericDataStructSingle*>(dataEntry.get());
			ot::Variable value = singleEntry->getValue();
			quantityDescriptionCurve->addDatapoint(std::move(value));
		}
		quantityDescription = quantityDescriptionCurve.release();
	}

	ot::GenericDataStructVector* vectorEntry = dynamic_cast<ot::GenericDataStructVector*>(firstDataEntry.get());
	if (vectorEntry != nullptr)
	{
		if (vectorEntry->getNumberOfEntries() == 0)
		{
			throw std::exception(errorMessageNoValuesReceived.c_str());
		}

		if ((pipelineData.size() > 1 && vectorEntry->getNumberOfEntries() > 1)) //Essentially a matrix of entries
		{
			assert(false); //Should not be.
		}

		std::vector<ot::Variable> values = vectorEntry->getValues();
		_outTypeName = values.begin()->getTypeName();
		auto quantityDescriptionCurve = std::make_unique<QuantityDescriptionCurve>();
		quantityDescriptionCurve->setDataPoints({ values.begin(), values.end() });
		quantityDescription = quantityDescriptionCurve.release();
	}

	ot::GenericDataStructMatrix* matrixEntry = dynamic_cast<ot::GenericDataStructMatrix*>(firstDataEntry.get());
	if (matrixEntry != nullptr)
	{
		if (matrixEntry->getNumberOfEntries() == 0)
		{
			throw std::exception(errorMessageNoValuesReceived.c_str());
		}
		ot::MatrixEntryPointer pointer;
		pointer.m_column = 0;
		pointer.m_row = 0;
		const ot::Variable& value = matrixEntry->getValue(pointer);
		const uint32_t numberOfRows = matrixEntry->getNumberOfColumns();
		const uint32_t numberOfColumns = matrixEntry->getNumberOfRows();
		ot::MatrixEntryPointer matrixDimension;
		matrixDimension.m_column = numberOfColumns;
		matrixDimension.m_row= numberOfRows;
		_outTypeName = value.getTypeName();
		auto quantityDescriptionMatrix(std::make_unique<QuantityDescriptionMatrix>(matrixDimension, pipelineData.size()));
		for (auto dataEntry : pipelineData)
		{
			matrixEntry = dynamic_cast<ot::GenericDataStructMatrix*>(dataEntry.get());
			quantityDescriptionMatrix->addToValues(*matrixEntry);
			quantityDescription = quantityDescriptionMatrix.release();
		}
	}
	assert(quantityDescription != nullptr);
	return quantityDescription;
}

void BlockHandlerStorage::extractQuantityProperties(const ot::Connector& _connector, std::string& _outName, std::string& _outUnit, std::string& _outValueName)
{
	const std::string portLabel = _connector.getConnectorTitle();
	const std::string quantityName = _connector.getConnectorName();
	auto portData = _dataPerPort.find(quantityName);
	assert(portData != _dataPerPort.end()); //Should have been checked before
	const MetadataQuantity* metadataQuantityFromPort = portData->second.m_quantity;
	const MetadataQuantityValueDescription* quantityValueDescriptionFromPort = portData->second.m_quantityDescription;
	const QuantityProperties selectedProperties = m_blockEntityStorage->getPropertiesQuantity(quantityName);

	if (metadataQuantityFromPort != nullptr)
	{
		assert(quantityValueDescriptionFromPort != nullptr);
		if (selectedProperties.m_propertyName == "")
		{
			_outName = (metadataQuantityFromPort->quantityName);
		}
		else
		{
			_outName = (selectedProperties.m_propertyName);
		}

		if (selectedProperties.m_propertyType == "")
		{
			_outValueName = quantityValueDescriptionFromPort->quantityValueName;
		}
		else
		{
			_outValueName = selectedProperties.m_propertyType;
		}

		if (selectedProperties.m_propertyUnit == "")
		{
			_outUnit = quantityValueDescriptionFromPort->unit;
		}
		else
		{
			_outUnit = selectedProperties.m_propertyUnit;
		}
	}
	else
	{
		if (selectedProperties.m_propertyName == "")
		{
			const std::string errorMessagePropertyMustBeSet = portLabel + " did not receive meta data from the processing pipeline. All properties for this quantity have to be set manually, which is currently not the case.";
			throw std::exception(errorMessagePropertyMustBeSet.c_str());
		}
		_outName = selectedProperties.m_propertyName;

		if (selectedProperties.m_propertyUnit == "")
		{
			_outUnit = " ";
		}
		else
		{
			_outUnit = selectedProperties.m_propertyUnit;
		}
		if (selectedProperties.m_propertyType == "")
		{
			_outValueName = " ";
		}
		else
		{
			_outValueName = selectedProperties.m_propertyType;
		}
	}


}
