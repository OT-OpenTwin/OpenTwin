// @otlicense
// File: DataCategorizationConsistencyChecker.cpp
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

#include "DataCategorizationConsistencyChecker.h"
#include "Documentation.h"
#include "EntityTableSelectedRanges.h"
#include <set>

bool DataCategorizationConsistencyChecker::isValidAllMSMDHaveParameterAndQuantities(std::map<std::string, MetadataAssemblyData>& allMetadataAssembliesByName)
{
	bool foundIssue = false;

	for (const auto& element : allMetadataAssembliesByName)
	{
		const MetadataAssemblyData* metadataAssembly = &element.second;
		
		if (metadataAssembly->m_dataCategory == EntityParameterizedDataCategorization::DataCategorie::measurementSeriesMetadata)
		{
			if (metadataAssembly->m_next == nullptr)
			{
				Documentation::INSTANCE()->AddToDocumentation("MSMD \"" + element.first + "\" has no parameter assigned.\n");
				foundIssue = true;
			}
			else
			{
				if (metadataAssembly->m_next->m_next == nullptr)
				{
					Documentation::INSTANCE()->AddToDocumentation("MSMD \"" + element.first + "\" has no quantities assigned.\n");
					foundIssue = true;
				}
			}
		}
	}
	return !foundIssue;
}

bool DataCategorizationConsistencyChecker::isValidAllParameterAndQuantitiesReferenceSameTable(std::map<std::string, MetadataAssemblyData>& allMetadataAssembliesByName)
{
	bool issueFound = false;
	for (const auto& element : allMetadataAssembliesByName)
	{
		if (element.second.m_dataCategory != EntityParameterizedDataCategorization::DataCategorie::measurementSeriesMetadata)
		{
			continue;
		}
		std::set<std::string> tableNames;
		const MetadataAssemblyData* metadataAssembly = &element.second;
		if (metadataAssembly->m_next == nullptr || metadataAssembly->m_next->m_next == nullptr)
		{
			continue;
		}
		auto allSelectionRangesParameter = metadataAssembly->m_next->m_allSelectionRanges;
		auto allSelectionRangesQuantities= metadataAssembly->m_next->m_next->m_allSelectionRanges;

		for (const auto& tableSelectionEntity : allSelectionRangesParameter)
		{
			tableNames.insert(tableSelectionEntity->getTableName());
		}
		for (const auto& tableSelectionEntity : allSelectionRangesQuantities)
		{
			tableNames.insert(tableSelectionEntity->getTableName());
		}

		if (tableNames.size() != 1)
		{
			Documentation::INSTANCE()->AddToDocumentation("MSMD \"" + element.first + "\" the parameter and quantities are not in one table. Following table names were found: \n");
			for (const std::string& tableName : tableNames)
			{
				Documentation::INSTANCE()->AddToDocumentation(tableName);
			}
			issueFound = true;
		}
	}
	return !issueFound;
}

bool DataCategorizationConsistencyChecker::isValidQuantityAndParameterNumberMatches(KeyValuesExtractor& parameterData, KeyValuesExtractor& quantityData)
{

	std::map<uint64_t,std::list<std::string>> numberOfParameter;
	AddFieldNameByFieldvalueSize(parameterData.getFields(),numberOfParameter);
		
	std::map<uint64_t, std::list<std::string>> numberOfQuantities;
	AddFieldNameByFieldvalueSize(quantityData.getFields(),numberOfQuantities);
		
	if (numberOfQuantities.size() == 1 && numberOfParameter.size() == 1 && numberOfParameter.begin()->first == numberOfQuantities.begin()->first)
	{
		return true;
	}
	else
	{
		Documentation::INSTANCE()->AddToDocumentation("Quantities and parameter don't have the same number of entries.\n");
		for (const auto& quantities : numberOfQuantities)
		{
			const uint64_t& numberOfEntries = quantities.first;
			Documentation::INSTANCE()->AddToDocumentation(std::to_string(numberOfEntries) + " entries for the following quantities:\n");
			const std::list<std::string>& quantityNames = quantities.second;
			for (const std::string& quantityName : quantityNames)
			{
				Documentation::INSTANCE()->AddToDocumentation(quantityName + "\n");
			}
		}
		for (const auto& parameter : numberOfParameter)
		{
			const uint64_t& numberOfEntries = parameter.first;
			Documentation::INSTANCE()->AddToDocumentation(std::to_string(numberOfEntries) + " entries for the following parameter:\n");
			const std::list<std::string>& quantityNames = parameter.second;
			for (const std::string& quantityName : quantityNames)
			{
				Documentation::INSTANCE()->AddToDocumentation(quantityName + "\n");
			}
		}
		return false;
	}
	
}
