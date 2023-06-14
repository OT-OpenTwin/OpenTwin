#pragma once
#include <map>
#include <string>
#include "MetadataAssemblyData.h"

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

};
