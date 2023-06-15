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

class MetadataAssemblyRangeData
{
public:
	void LoadAllRangeSelectionInformation(const std::list< std::shared_ptr<EntityTableSelectedRanges>>& allRanges, std::map<std::string, std::shared_ptr<EntityParameterizedDataTable>>& allTables);

	const std::map<std::string, std::list<std::string>>*	GetStringFields() const { return &_stringFields; };
	const std::map<std::string, std::list<double>>*			GetDoubleFields() const { return &_doubleFields; };
	const std::map<std::string, std::list<int32_t>>*		GetInt32Fields() const { return &_int32Fields; };
	const std::map<std::string, std::list<int64_t>>*		GetInt64Fields() const { return &_int64Fields; };

	uint64_t getNumberOfFields() const;

private:

	/// <summary>
	/// Extracts the selected ranges from the table. All cell values are string values. They are sorted by a key value, which is the corresponding value in either the first row or collumn of the table.
	/// </summary>
	/// <param name="range"></param>
	/// <param name="table"></param>
	/// <returns></returns>
	std::map<std::string, std::list<std::string>> ExtractFieldsFromRange(std::shared_ptr<EntityTableSelectedRanges> range, std::shared_ptr<EntityParameterizedDataTable> table);

	/// <summary>
	/// Empties allFields and adds its content after casting the values to the corresponding member attribute.
	/// </summary>
	/// <param name="allFields"></param>
	/// <param name="rangeTypesByRangeNames"></param>
	void TransformSelectedDataIntoSelectedDataType(std::map<std::string, std::list<std::string>>& allFields, std::map<std::string, std::string>& rangeTypesByRangeNames);
	
	std::map<std::string,std::list<std::string>> _stringFields;
	std::map<std::string,std::list<double>> _doubleFields;
	std::map<std::string,std::list<int32_t>> _int32Fields;
	std::map<std::string,std::list<int64_t>> _int64Fields;

};


