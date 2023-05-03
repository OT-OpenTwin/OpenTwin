#pragma once
#include <stdint.h>
#include <set>
#include <list>
#include <string>

#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/json.hpp>
#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/builder/basic/array.hpp>

template <class T>
class QuantityContainer
{
public:
	inline QuantityContainer(int32_t msmdIndex, std::set<std::string>& parameterAbbreviations, std::list<int32_t>& parameterValueIndices, int32_t quantityIndex, bool isFlatCollection);
	inline void StoreToDatabase();
	
	inline void AddValue(T value);
	inline bool ParameterValueIndicesAndQuantityIndexAreMatching(const std::list<int32_t>& parameterValueIndices, int32_t quantityIndex);
	inline size_t GetValueArraySize() const { return _values.size(); };
private:
	bsoncxx::builder::basic::document _mongoDocument;
	std::list<T> _values;
	bool _isFlatCollection;
	std::list<int32_t> _parameterValueIndices;
	int32_t _quantityIndex;
};


template <class T>
inline QuantityContainer<T>::QuantityContainer(int32_t msmdIndex, std::set<std::string>& parameterAbbreviations, std::list<int32_t>& parameterValueIndices, int32_t quantityIndex, bool isFlatCollection)
	: _parameterValueIndices(parameterValueIndices), _isFlatCollection(isFlatCollection), _quantityIndex(quantityIndex)
{
	
	_mongoDocument.append(bsoncxx::builder::basic::kvp("SchemaType", "QuantityContainer"));
	_mongoDocument.append(bsoncxx::builder::basic::kvp("SchemaVersion", 1));
	_mongoDocument.append(bsoncxx::builder::basic::kvp("MSMD", msmdIndex));
	_mongoDocument.append(bsoncxx::builder::basic::kvp("Quantity", _quantityIndex));
	auto paramValIndex = parameterValueIndices.begin();
	for (const std::string& paramAbbrev : parameterAbbreviations)
	{
		_mongoDocument.append(bsoncxx::builder::basic::kvp(paramAbbrev, *paramValIndex));
		paramValIndex++;
	}
}

template<class T>
inline void QuantityContainer<T>::StoreToDatabase()
{
	if (!_isFlatCollection)
	{
		auto valueArray = bsoncxx::builder::basic::array();
		for (const T& value : _values)
		{
			valueArray.append(value);
		}
		_mongoDocument.append(bsoncxx::builder::basic::kvp("Values", valueArray));
	}
	else
	{
		_mongoDocument.append(bsoncxx::builder::basic::kvp("Value", *_values.begin()));
	}

}

template<class T>
inline void QuantityContainer<T>::AddValue(T value)
{
	_values.push_back(value);
}

template<class T>
inline bool QuantityContainer<T>::ParameterValueIndicesAndQuantityIndexAreMatching(const std::list<int32_t>& parameterValueIndices, int32_t quantityIndex)
{
	auto givenParameterIndexValue = _parameterValueIndices.begin();
	if (quantityIndex != _quantityIndex)
	{
		return false;
	}
	bool match = true;
	for (const auto& parameterValueIndex : parameterValueIndices)
	{
		if (parameterValueIndex != *givenParameterIndexValue)
		{
			match = false;
			break;
		}
		givenParameterIndexValue++;
	}

	return match;
}

