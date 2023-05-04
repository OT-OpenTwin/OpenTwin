#include "QuantityContainerCreator.h"

QuantityContainerCreator::QuantityContainerCreator(int32_t& msmdIndex, std::set<std::string>& parameterAbbreviations, int32_t containerSize)
	: _msmdIndex(msmdIndex), _parameterAbbreviations(parameterAbbreviations), _containerSize(containerSize), _isFlatCollection(_containerSize == 1)
{
	
}

void QuantityContainerCreator::AddToQuantityContainer(int32_t& quantityIndex, std::list<int32_t>& parameterValueIndices, std::string& value)
{
	AddToQuantityContainer(_stringQuantContainers, quantityIndex, parameterValueIndices, value);
}

void QuantityContainerCreator::AddToQuantityContainer(int32_t& quantityIndex, std::list<int32_t>& parameterValueIndices, double& value)
{
	AddToQuantityContainer(_doubleQuantContainers, quantityIndex, parameterValueIndices, value);
}

void QuantityContainerCreator::AddToQuantityContainer(int32_t& quantityIndex, std::list<int32_t>& parameterValueIndices, int32_t& value)
{
	AddToQuantityContainer(_int32QuantContainers, quantityIndex, parameterValueIndices, value);
}

void QuantityContainerCreator::AddToQuantityContainer(int32_t& quantityIndex, std::list<int32_t>& parameterValueIndices, int64_t& value)
{
	AddToQuantityContainer(_int64QuantContainers, quantityIndex, parameterValueIndices, value);
}

void QuantityContainerCreator::Flush(DataStorageAPI::ResultDataStorageAPI& storageAPI)
{
	for (auto& qc : _stringQuantContainers)
	{
		DataStorageAPI::DataStorageResponse response = storageAPI.InsertDocumentToResultStorage(qc._mongoDocument, _checkForDocumentExistenceBeforeInsert, _quequeDocumentsWhenInserting);
		if (!response.getSuccess())
		{
			throw std::exception("Insertion of quantity container failed.");
		}
	}
	_stringQuantContainers.clear();

	for (auto& qc : _doubleQuantContainers)
	{
	
		DataStorageAPI::DataStorageResponse response = storageAPI.InsertDocumentToResultStorage(qc._mongoDocument, _checkForDocumentExistenceBeforeInsert, _quequeDocumentsWhenInserting);
		if (!response.getSuccess())
		{
			throw std::exception("Insertion of quantity container failed.");
		}
	}
	_doubleQuantContainers.clear();

	for (auto& qc : _int32QuantContainers)
	{
		DataStorageAPI::DataStorageResponse response = storageAPI.InsertDocumentToResultStorage(qc._mongoDocument, _checkForDocumentExistenceBeforeInsert, _quequeDocumentsWhenInserting);
		if (!response.getSuccess())
		{
			throw std::exception("Insertion of quantity container failed.");
		}
	}
	_int32QuantContainers.clear();

	for (auto& qc : _int64QuantContainers)
	{
		DataStorageAPI::DataStorageResponse response = storageAPI.InsertDocumentToResultStorage(qc._mongoDocument, _checkForDocumentExistenceBeforeInsert, _quequeDocumentsWhenInserting);
		if (!response.getSuccess())
		{
			throw std::exception("Insertion of quantity container failed.");
		}
	}
	_int64QuantContainers.clear();
}
