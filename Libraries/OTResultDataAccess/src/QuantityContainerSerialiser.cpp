// @otlicense
// File: QuantityContainerSerialiser.cpp
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
#include "OTResultDataAccess/QuantityContainerSerialiser.h"
#include "OTCore/Tuple/TupleDescriptionComplex.h"

QuantityContainerSerialiser::QuantityContainerSerialiser(const std::string& _collectionName, ResultImportLogger& _logger)
	:m_dataStorageAccess(_collectionName), m_logger(_logger)
{
	m_logger.log("Buffer for database write operation: " + std::to_string(m_bufferSize) + " documents");
	m_quantityContainer.reserve(m_bufferSize);
}

void QuantityContainerSerialiser::storeDataPoints(ot::UID _seriesIndex, std::list<ot::UID>& _parameterIDs, std::list<ot::Variable>& _constParameterValues, std::list<std::list<ot::Variable>::const_iterator>& _changingParameterValues, uint64_t _numberOfParameterValues, QuantityDescription* _quantityDescription)
{
	auto curveDescription =	dynamic_cast<QuantityDescriptionCurve*>(_quantityDescription);
	if (curveDescription != nullptr)
	{
		m_logger.log("Storing data points as curve.");
		storeDataPoints(_seriesIndex, _parameterIDs, _constParameterValues, _changingParameterValues, _numberOfParameterValues,curveDescription);
		flushQuantityContainer(); 
		return;
	}
	
	auto matrixParameterDescription = dynamic_cast<QuantityDescriptionMatrix*>(_quantityDescription);
	if (matrixParameterDescription != nullptr)
	{
		m_logger.log("Storing data points as matrix");
		storeDataPoints(_seriesIndex, _parameterIDs, _constParameterValues, _changingParameterValues, _numberOfParameterValues, matrixParameterDescription);
		flushQuantityContainer();
		return;
	}

	throw std::exception("Failed to store data points");
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
	assert(quantityMetadata.m_tupleDescription.isSingle()); //Curves should not have a tuple description, as they only have one value per quantity. 
	
	ot::UID quantityID = quantityMetadata.quantityIndex;

	m_bucketSize = 1;
	
	const size_t numberOfDocuments = _numberOfParameterValues;
	m_logger.log("Storing " + std::to_string(numberOfDocuments) + " documents");

	std::vector<ot::Variable>currentParameterValues{ _constParameterValues.begin(), _constParameterValues.end() };
	const size_t constCount = currentParameterValues.size();

	for (uint64_t i = 0; i < _numberOfParameterValues; i++)
	{
		currentParameterValues.resize(constCount);
		for (auto& changingParameterValueIt : _changingParameterValues)
		{
			currentParameterValues.push_back(*changingParameterValueIt);
			if (i != _numberOfParameterValues - 1)
			{
				changingParameterValueIt++;
			}
		}

		if (dataValueItt->isComplex())
		{
			std::list<ot::Variable> bothValues = { dataValueItt->getComplex().real(), dataValueItt->getComplex().imag() };
			addQuantityContainer(_seriesIndex, _parameterIDs, currentParameterValues, quantityMetadata.quantityIndex, bothValues);
		}
		else
		{
			addQuantityContainer(_seriesIndex, _parameterIDs, currentParameterValues, quantityID, *dataValueItt);
		}

		if (i != _numberOfParameterValues - 1)
		{
			dataValueItt++;
		}
	}
}

void QuantityContainerSerialiser::storeDataPoints(ot::UID _seriesIndex, std::list<ot::UID>& _parameterIDs, std::list<ot::Variable>& _constParameterValues, std::list<std::list<ot::Variable>::const_iterator>& _changingParameterValues, const uint64_t _numberOfParameterValues, QuantityDescriptionMatrix* _quantityDescription)
{
	auto& quantityMetadata = _quantityDescription->getMetadataQuantity();
	
	m_logger.log("Storing " + std::to_string(_numberOfParameterValues) + " documents");

	m_bucketSize = 1;
	for (uint32_t dimension : quantityMetadata.dataDimensions)
	{
		m_bucketSize *= dimension;
	}
	
	std::vector<ot::GenericDataStructMatrix> quantityValueMatrices = _quantityDescription->getValues();
	
	std::vector<ot::Variable>currentParameterValues{ _constParameterValues.begin(), _constParameterValues.end() };
	const size_t constCount = currentParameterValues.size();

	for (size_t i = 0; i < _numberOfParameterValues; i++)
	{
		currentParameterValues.resize(constCount);
		for (auto& changingParameterValueIt : _changingParameterValues)
		{
			currentParameterValues.push_back(*changingParameterValueIt);
			if (i != _numberOfParameterValues - 1)
			{
				changingParameterValueIt++;
			}
		}

		ot::GenericDataStructMatrix& quantityValueMatrix = quantityValueMatrices[i];
		
		const std::vector<ot::Variable>& quantityValueMatrixEntry = quantityValueMatrix.getValues();
		for (uint32_t j = 0; j < quantityValueMatrix.getNumberOfEntries(); j++)
		{
			if (quantityValueMatrixEntry[j].isComplex())
			{
				std::list<ot::Variable> bothValues{ quantityValueMatrixEntry[j].getComplex().real(), quantityValueMatrixEntry[j].getComplex().imag()};
				addQuantityContainer(_seriesIndex, _parameterIDs, currentParameterValues, quantityMetadata.quantityIndex, bothValues);
			}
			else
			{
				addQuantityContainer(_seriesIndex, _parameterIDs, currentParameterValues, quantityMetadata.quantityIndex, quantityValueMatrixEntry[j]);
			}
		}
		
	}

}

void QuantityContainerSerialiser::flushQuantityContainer()
{
	for (auto& qc : m_quantityContainer)
	{
		DataStorageAPI::DataStorageResponse response = m_dataStorageAccess.insertDocumentToDataLakePartition(qc.getMongoDocument(), false, true);
		if (!response.getSuccess())
		{
			throw std::exception("Insertion of quantity container failed.");
		}
	}
	m_dataStorageAccess.flushQueuedData();
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

void QuantityContainerSerialiser::addQuantityContainer(ot::UID _seriesIndex, std::list<ot::UID>& _parameterIDs, std::vector<ot::Variable>& _parameterValues, uint64_t _quantityIndex, const ot::Variable& _quantityValue)
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

void QuantityContainerSerialiser::addQuantityContainer(ot::UID _seriesIndex, std::list<ot::UID>& _parameterIDs, std::list<ot::Variable>& _parameterValues, uint64_t _quantityIndex, const std::list<ot::Variable>& _quantityValues)
{
	if (m_quantityContainer.size() == 0)
	{
		QuantityContainer newContainer(_seriesIndex, _parameterIDs, _parameterValues, _quantityIndex);
		newContainer.addValue(_quantityValues);
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
			newContainer.addValue(_quantityValues);
			m_quantityContainer.push_back(std::move(newContainer));
		}
		else
		{
			lastAddedQuantityContainer->addValue(_quantityValues);
		}
	}
}

void QuantityContainerSerialiser::addQuantityContainer(ot::UID _seriesIndex, std::list<ot::UID>& _parameterIDs, std::vector<ot::Variable>& _parameterValues, uint64_t _quantityIndex, const std::list<ot::Variable>& _quantityValues)
{
	if (m_quantityContainer.size() == 0)
	{
		QuantityContainer newContainer(_seriesIndex, _parameterIDs, _parameterValues, _quantityIndex);
		newContainer.addValue(_quantityValues);
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
			newContainer.addValue(_quantityValues);
			m_quantityContainer.push_back(std::move(newContainer));
		}
		else
		{
			lastAddedQuantityContainer->addValue(_quantityValues);
		}
	}
}
