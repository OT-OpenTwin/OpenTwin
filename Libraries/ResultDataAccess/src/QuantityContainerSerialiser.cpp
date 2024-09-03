#include "QuantityContainerSerialiser.h"


QuantityContainerSerialiser::QuantityContainerSerialiser(const std::string& _collectionName)
	:m_dataStorageAccess(_collectionName)
{
	m_quantityContainer.reserve(m_bufferSize);
}

void QuantityContainerSerialiser::storeDataPoints(ot::UID _seriesIndex, std::list<ot::UID>& _parameterIDs, std::list<ot::Variable>& _constParameterValues, std::list<std::list<ot::Variable>::const_iterator>& _changingParameterValues, uint64_t _numberOfParameterValues, QuantityDescription* _quantityDescription)
{
	auto curveDescription =	dynamic_cast<QuantityDescriptionCurve*>(_quantityDescription);
	if (curveDescription != nullptr)
	{
		storeDataPoints(_seriesIndex, _parameterIDs, _constParameterValues, _changingParameterValues, _numberOfParameterValues,curveDescription);
		return;
	}
	auto complexCurveDescription = dynamic_cast<QuantityDescriptionCurveComplex*>(_quantityDescription);
	if (complexCurveDescription != nullptr)
	{
		storeDataPoints(_seriesIndex, _parameterIDs, _constParameterValues, _changingParameterValues, _numberOfParameterValues, complexCurveDescription);
		return;
	}
	auto sparameterDescription = dynamic_cast<QuantityDescriptionSParameter*>(_quantityDescription);
	if (sparameterDescription != nullptr)
	{
		storeDataPoints(_seriesIndex, _parameterIDs, _constParameterValues, _changingParameterValues, _numberOfParameterValues, sparameterDescription);
		return;
	}

	flushQuantityContainer();
}

void QuantityContainerSerialiser::storeDataPoints(ot::UID _seriesIndex, std::list<ot::UID>& _parameterIDs, std::list<ot::Variable>& _constParameterValues, std::list<std::list<ot::Variable>::const_iterator>& _changingParameterValues, uint64_t _numberOfParameterValues, QuantityDescriptionCurve* _quantityDescription)
{
	const std::vector<ot::Variable>& dataValues = _quantityDescription->getDataPoints();

	if (dataValues.empty())
	{
		throw std::exception("Values are missing either the imaginary or the real part.");
	}

	if (_numberOfParameterValues != dataValues.size())
	{
		throw std::exception("Number of real values does not match the number of parameter values.");
	}
	
	std::vector<ot::Variable>::const_iterator dataValueItt(dataValues.begin());

	auto quantityMetadata = _quantityDescription->getMetadataQuantity();
	assert(quantityMetadata.valueDescriptions.size() == 1); //Quantities curve numbers require that a single value description is set.

	auto valueDescription = quantityMetadata.valueDescriptions.begin();
	ot::UID quantityID = valueDescription->quantityIndex;

	m_bucketSize = 1;

	for (uint64_t i = 0; i < _numberOfParameterValues; i++)
	{
		std::list<ot::Variable>currentParameterValues{ _constParameterValues.begin(), _constParameterValues.end() };
		for (auto& changingParameterValueIt : _changingParameterValues)
		{
			currentParameterValues.push_back(*changingParameterValueIt);
			if (i != _numberOfParameterValues - 1)
			{
				changingParameterValueIt++;
			}
		}

		addQuantityContainer(_seriesIndex, _parameterIDs, currentParameterValues, quantityID, *dataValueItt);

		if (i != _numberOfParameterValues - 1)
		{
			dataValueItt++;
		}
	}
}

void QuantityContainerSerialiser::storeDataPoints(ot::UID _seriesIndex, std::list<ot::UID>& _parameterIDs, std::list<ot::Variable>& _constParameterValues, std::list<std::list<ot::Variable>::const_iterator>& _changingParameterValues, uint64_t _numberOfParameterValues, QuantityDescriptionCurveComplex* _quantityDescription)
{
	const std::vector<ot::Variable>& realValues = _quantityDescription->getQuantityValuesReal();
	const std::vector<ot::Variable>& imagValues = _quantityDescription->getQuantityValuesImag();
	
	if (imagValues.empty() || realValues.empty())
	{
		throw std::exception("Complex values are missing either the imaginary or the real part.");
	}

	if (imagValues.size() != realValues.size())
	{
		throw std::exception("Unequal amount of real and imaginary numbers.");
	}

	if (_numberOfParameterValues != realValues.size())
	{
		throw std::exception("Number of real values does not match the number of parameter values.");
	}
	if (_numberOfParameterValues != imagValues.size())
	{
		throw std::exception("Number of imaginary values does not match the number of parameter values.");
	}
	
	std::vector<ot::Variable>::const_iterator realValueItt(realValues.begin()), imagValueItt(imagValues.begin());
	
	auto quantityMetadata =	_quantityDescription->getMetadataQuantity();
	
	assert(quantityMetadata.valueDescriptions.size() == 2); //Quantities that hold complex numbers require two value descriptions. Should have been set upon creation.
	std::list<MetadataQuantityValueDescription>::iterator valueDescriptionIt =  quantityMetadata.valueDescriptions.begin();
	MetadataQuantityValueDescription& realValueDescription = *valueDescriptionIt++;
	ot::UID realQuantityID(realValueDescription.quantityIndex);

	MetadataQuantityValueDescription& imagValueDescription = *valueDescriptionIt;
	ot::UID imagQuantityID(imagValueDescription.quantityIndex);
	
	m_bucketSize = 1;

	for (uint64_t i = 0; i < _numberOfParameterValues; i++)
	{
		std::list<ot::Variable>currentParameterValues{ _constParameterValues.begin(), _constParameterValues.end() };
		for (auto& changingParameterValueIt : _changingParameterValues)
		{
			currentParameterValues.push_back(*changingParameterValueIt);
			if (i != _numberOfParameterValues - 1)
			{
				changingParameterValueIt++;
			}
		}
		
		addQuantityContainer(_seriesIndex, _parameterIDs, currentParameterValues, realQuantityID, *realValueItt);
		addQuantityContainer(_seriesIndex, _parameterIDs, currentParameterValues, imagQuantityID, *imagValueItt);
			
		if (i != _numberOfParameterValues - 1)
		{
			imagValueItt++;
			realValueItt++;
		}
	}
}

void QuantityContainerSerialiser::storeDataPoints(ot::UID _seriesIndex, std::list<ot::UID>& _parameterIDs, std::list<ot::Variable>& _constParameterValues, std::list<std::list<ot::Variable>::const_iterator>& _changingParameterValues, uint64_t _numberOfParameterValues, QuantityDescriptionSParameter* _quantityDescription)
{
	
	if (_quantityDescription->getNumberOfFirstValues() == 0 || _quantityDescription->getNumberOfSecondValues() == 0)
	{
		throw std::exception("Complex values are missing either the imaginary or the real part.");
	}

	if (_quantityDescription->getNumberOfFirstValues() != _quantityDescription->getNumberOfSecondValues())
	{
		throw std::exception("Unequal amount of real and imaginary numbers.");
	}

	if (_numberOfParameterValues != _quantityDescription->getNumberOfFirstValues())
	{
		throw std::exception("Number of the first values does not match the number of parameter values.");
	}
	if (_numberOfParameterValues != _quantityDescription->getNumberOfSecondValues())
	{
		throw std::exception("Number of the second values does not match the number of parameter values.");
	}

	auto& quantityMetadata = _quantityDescription->getMetadataQuantity();
	const uint32_t numberOfPorts = quantityMetadata.dataDimensions.front();
	m_bucketSize = numberOfPorts * numberOfPorts;
	std::list<MetadataQuantityValueDescription>::iterator valueDescriptionIt = quantityMetadata.valueDescriptions.begin();;
	MetadataQuantityValueDescription& firstValueDescription = *valueDescriptionIt++;
	MetadataQuantityValueDescription& secondValueDescription = *valueDescriptionIt;

	for (size_t i = 0; i < _numberOfParameterValues; i++)
	{
		std::list<ot::Variable>currentParameterValues{ _constParameterValues.begin(), _constParameterValues.end() };
		for (auto& changingParameterValueIt : _changingParameterValues)
		{
			currentParameterValues.push_back(*changingParameterValueIt);
			if (i != _numberOfParameterValues - 1)
			{
				changingParameterValueIt++;
			}
		}


		std::vector<ot::Variable> quantityValueEntriesFirst = _quantityDescription->getFirstValues(i);
		for (ot::Variable& quantityValueEntry : quantityValueEntriesFirst)
		{
			addQuantityContainer(_seriesIndex, _parameterIDs, currentParameterValues, firstValueDescription.quantityIndex, quantityValueEntry);
		}
		
		std::vector<ot::Variable> quantityValueEntriesSecond = _quantityDescription->getSecondValues(i);
		for (ot::Variable& quantityValueEntry : quantityValueEntriesSecond)
		{
			addQuantityContainer(_seriesIndex, _parameterIDs, currentParameterValues, secondValueDescription.quantityIndex, quantityValueEntry);
		}
	}
}

void QuantityContainerSerialiser::flushQuantityContainer()
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

void QuantityContainerSerialiser::addQuantityContainer(ot::UID _seriesIndex, std::list<ot::UID>& _parameterIDs, std::list<ot::Variable>&& _parameterValues, uint64_t _quantityIndex, const ot::Variable& _quantityValue)
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

void QuantityContainerSerialiser::addQuantityContainer(ot::UID _seriesIndex, std::list<ot::UID>& _parameterIDs, std::list<ot::Variable>& _parameterValues, uint64_t _quantityIndex, const ot::Variable& _quantityValue)
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
