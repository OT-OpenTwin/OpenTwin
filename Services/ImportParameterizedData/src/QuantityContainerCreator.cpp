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

void QuantityContainerCreator::Flush()
{
	for (auto& qc : _stringQuantContainers)
	{
		qc.StoreToDatabase();
		_stringQuantContainers.clear();
	}
	for (auto& qc : _doubleQuantContainers)
	{
		qc.StoreToDatabase();
		_doubleQuantContainers.clear();
	}
	for (auto& qc : _int32QuantContainers)
	{
		qc.StoreToDatabase();
		_int32QuantContainers.clear();
	}
	for (auto& qc : _int64QuantContainers)
	{
		qc.StoreToDatabase();
		_int64QuantContainers.clear();
	}
}
