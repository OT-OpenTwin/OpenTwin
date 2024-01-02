/*****************************************************************//**
 * \file   MetadataAssemblyRangeData.h
 * \brief	Extracts all selections from a table and groups sets of values by their key value. 
 *			The key value for a set is the value in either the first column or row corresponding to the selection. 
 *			Each selection has a property that indicates if the column or the row holds the key information.
 * 
 * \author Wagner
 * \date   June 2023
 *********************************************************************/
#pragma once
#include "EntityTableSelectedRanges.h"
#include "EntityParameterizedDataTable.h"

#include <map>
#include <list>
#include <memory>
#include <stdint.h>
#include "OTCore/Variable.h"

class MetadataAssemblyRangeData
{
public:
	MetadataAssemblyRangeData();
	MetadataAssemblyRangeData(const MetadataAssemblyRangeData& other) = delete;
	MetadataAssemblyRangeData(const MetadataAssemblyRangeData&& other) = delete;
	MetadataAssemblyRangeData& operator=(const MetadataAssemblyRangeData& other) = delete;
	MetadataAssemblyRangeData& operator=(const MetadataAssemblyRangeData&& other) = delete;

	void LoadAllRangeSelectionInformation(const std::list< std::shared_ptr<EntityTableSelectedRanges>>& allRanges, std::map<std::string, std::shared_ptr<EntityParameterizedDataTable>>& allTables);


	std::map<std::string, std::list<ot::Variable>>* getFields() { return &_fields; };

	uint64_t getNumberOfFields() const;

private:

	/// <summary>
	/// Extracts the selected ranges from the table. All cell values are string values. 
	/// They are sorted by a key value, which is the corresponding value in either the first row or collumn of the table.
	/// Each is contained in a map with its index (row or column) as key.
	/// </summary>
	/// <param name="range"></param>
	/// <param name="table"></param>
	/// <returns></returns>
	std::vector<std::string> ExtractFieldsFromRange(std::shared_ptr<EntityTableSelectedRanges> range, std::shared_ptr<EntityParameterizedDataTable> table, std::map<std::string, std::map<std::uint32_t, std::string>>& outAllSortedFields);

	/// <summary>
	/// Empties allFields and adds its content after casting the values to the corresponding member attribute.
	/// </summary>
	/// <param name="allFields"></param>
	/// <param name="rangeTypesByRangeNames"></param>
	void TransformSelectedDataIntoSelectedDataType(std::map<std::string, std::list<std::string>>& allFields, std::map<std::string, std::string>& rangeTypesByRangeNames);
	
	std::map<std::string,std::list<ot::Variable>> _fields;
	char _decimalDelimmiter;

};


