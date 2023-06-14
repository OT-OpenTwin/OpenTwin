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
		
		if (metadataAssembly->dataCategory == EntityParameterizedDataCategorization::DataCategorie::measurementSeriesMetadata)
		{
			if (metadataAssembly->next == nullptr)
			{
				Documentation::INSTANCE()->AddToDocumentation("MSMD \"" + element.first + "\" has no parameter assigned.\n");
				foundIssue = true;
			}
			else
			{
				if (metadataAssembly->next->next == nullptr)
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
		if (element.second.dataCategory != EntityParameterizedDataCategorization::DataCategorie::measurementSeriesMetadata)
		{
			continue;
		}
		std::set<std::string> tableNames;
		const MetadataAssemblyData* metadataAssembly = &element.second;
		if (metadataAssembly->next == nullptr || metadataAssembly->next->next == nullptr)
		{
			continue;
		}
		auto allSelectionRangesParameter = metadataAssembly->next->allSelectionRanges;
		auto allSelectionRangesQuantities= metadataAssembly->next->next->allSelectionRanges;

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
