#pragma once
#include <stdint.h>
#include <set>
#include <string>
#include <list>

#include "QuantityContainer.h"
#include "ResultDataStorageAPI.h"

class QuantityContainerCreator
{
public:
	QuantityContainerCreator(int32_t& msmdIndex, std::set<std::string>& parameterAbbreviations, int32_t containerSize);
	void AddToQuantityContainer(int32_t& quantityIndex, std::list<int32_t>& parameterValueIndices, std::string& value);
	void AddToQuantityContainer(int32_t& quantityIndex, std::list<int32_t>& parameterValueIndices, double& value);
	void AddToQuantityContainer(int32_t& quantityIndex, std::list<int32_t>& parameterValueIndices, int32_t& value);
	void AddToQuantityContainer(int32_t& quantityIndex, std::list<int32_t>& parameterValueIndices, int64_t& value);

	void Flush(DataStorageAPI::ResultDataStorageAPI& storageAPI);
private:
	std::list<QuantityContainer<std::string>> _stringQuantContainers;
	std::list<QuantityContainer<int32_t>> _int32QuantContainers;
	std::list<QuantityContainer<int64_t>> _int64QuantContainers;
	std::list<QuantityContainer<double>> _doubleQuantContainers;

	int32_t _msmdIndex;
	int32_t _containerSize;
	bool _isFlatCollection;
	bool _checkForDocumentExistenceBeforeInsert = false;
	bool _quequeDocumentsWhenInserting = false;
	std::set<std::string> _parameterAbbreviations;

	template <class T>
	inline void AddToQuantityContainer(std::list <QuantityContainer<T>>& quantityContainerOfType, int32_t& quantityIndex, std::list<int32_t>& parameterValueIndices, T& value);
};

template<class T>
inline void QuantityContainerCreator::AddToQuantityContainer(std::list<QuantityContainer<T>>& quantityContainerOfType, int32_t& quantityIndex, std::list<int32_t>& parameterValueIndices, T& value)
{
	bool doesAlreadyExist = false;
	//for (auto& quantityContainer : quantityContainerOfType)
	//{
	//	if (quantityContainer.ParameterValueIndicesAndQuantityIndexAreMatching(parameterValueIndices, quantityIndex))
	//	{
	//		doesAlreadyExist = true;
	//	}
	//}
	if(!doesAlreadyExist)
	{
		//if (quantityContainerOfType.back().GetValueArraySize() >= _containerSize)
		//{
		//	assert(0);// Container size should match with the number of parameter apperiences.
		//}
		quantityContainerOfType.push_back(QuantityContainer<T>(_msmdIndex, _parameterAbbreviations, parameterValueIndices, quantityIndex, _isFlatCollection));
		quantityContainerOfType.back().AddValue(value);
	}
}
