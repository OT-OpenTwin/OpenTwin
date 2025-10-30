// @otlicense
// File: DataCategorizationConsistencyChecker.h
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
