// @otlicense

#pragma once
#include <map>
#include <string>
#include "MetadataAssemblyData.h"
#include "KeyValuesExtractor.h"

class DataCategorizationConsistencyChecker
{
public:
	/// <summary>
	/// Guarantees that the next pointer in the MetadataAssemblyData structs are all != nullptr.
	/// </summary>
	/// <param name="allMetadataAssembliesByName"></param>
	/// <returns></returns>
	bool isValidAllMSMDHaveParameterAndQuantities(std::map<std::string, MetadataAssemblyData>& allMetadataAssembliesByName);
	bool isValidAllParameterAndQuantitiesReferenceSameTable(std::map<std::string, MetadataAssemblyData>& allMetadataAssembliesByName);
	bool isValidQuantityAndParameterNumberMatches(KeyValuesExtractor& parameterData, KeyValuesExtractor& quantityData);

private:
	template <class T>
	void AddFieldNameByFieldvalueSize(const std::map <std::string, std::list<T>>* fields ,std::map<uint64_t, std::list<std::string>>& outNumberOfParameter);
};

template<class T>
inline void DataCategorizationConsistencyChecker::AddFieldNameByFieldvalueSize(const std::map<std::string, std::list<T>>* fields, std::map<uint64_t, std::list<std::string>>& outNumberOfParameter)
{
	for (const auto& field : *fields)
	{
		const std::list<T>& values = field.second;
		const std::string& name = field.first;
		outNumberOfParameter[values.size()].push_back(name);
	}
}
