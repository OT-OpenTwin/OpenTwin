#pragma once
#include <stdint.h>
#include <set>
#include <string>
#include <list>

#include "QuantityContainer.h"

class QuantityContainerCreator
{
public:
	QuantityContainerCreator(int32_t& msmdIndex, std::set<std::string>& parameterAbbreviations, int32_t containerSize);
	void AddToQuantityContainer(int32_t& quantityIndex, std::list<int32_t>& parameterValueIndices, std::string& value);
	void AddToQuantityContainer(int32_t& quantityIndex, std::list<int32_t>& parameterValueIndices, double& value);
	void AddToQuantityContainer(int32_t& quantityIndex, std::list<int32_t>& parameterValueIndices, int32_t& value);
	void AddToQuantityContainer(int32_t& quantityIndex, std::list<int32_t>& parameterValueIndices, int64_t& value);

	void Flush();
private:
	std::list<QuantityContainer<std::string>> _stringQuantContainers;
	std::list<QuantityContainer<int32_t>> _int32QuantContainers;
	std::list<QuantityContainer<int64_t>> _int64QuantContainers;
	std::list<QuantityContainer<double>> _doubleQuantContainers;

	int32_t _msmdIndex;
	int32_t _containerSize;
	bool _isFlatCollection;
	std::set<std::string> _parameterAbbreviations;

	template <class T>
	inline void AddToQuantityContainer(std::list <QuantityContainer<T>>& quantityContainerOfType, int32_t& quantityIndex, std::list<int32_t>& parameterValueIndices, T& value);
};

template<class T>
inline void QuantityContainerCreator::AddToQuantityContainer(std::list<QuantityContainer<T>>& quantityContainerOfType, int32_t& quantityIndex, std::list<int32_t>& parameterValueIndices, T& value)
{
	for (auto& quantityContainer : quantityContainerOfType)
	{
		if (quantityContainer.ParameterValueIndicesAndQuantityIndexAreMatching(parameterValueIndices, quantityIndex))
		{
			if (quantityContainer.GetValueArraySize() >= _containerSize)
			{
				assert(0);// Container size should match with the number of parameter apperiences.
			}
			quantityContainer.AddValue(value);
		}
		else
		{

			quantityContainerOfType.push_back(QuantityContainer<T>(_msmdIndex, _parameterAbbreviations, parameterValueIndices, quantityIndex, _isFlatCollection));
			quantityContainerOfType.back().AddValue(value);
		}
	}
}
