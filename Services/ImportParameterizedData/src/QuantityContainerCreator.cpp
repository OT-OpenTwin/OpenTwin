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
	uint64_t totalSize = _stringQuantContainers.size() + _doubleQuantContainers.size() + _int32QuantContainers.size() + _int64QuantContainers.size();
	if (totalSize > 0)
	{
		_updater->SetTotalNumberOfUpdates(5, totalSize);
		_triggerCounter = 0;

		FlushTypedList<>(_stringQuantContainers, storageAPI);
		FlushTypedList<>(_doubleQuantContainers, storageAPI);
		FlushTypedList<>(_int32QuantContainers , storageAPI);
		FlushTypedList<>(_int64QuantContainers , storageAPI);
	}
}

void QuantityContainerCreator::SetUpdateProgress(ProgressUpdater& updater)
{
	_updater = &updater;
}
