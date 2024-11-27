#pragma once
#include "EntityTableSelectedRanges.h"
#include "IVisualisationTable.h"

#include <map>
#include <list>
#include <memory>
#include <stdint.h>
#include "OTCore/Variable.h"
#include "MetadataAssemblyData.h"

class KeyValuesExtractor
{
public:
	KeyValuesExtractor() = default;
	~KeyValuesExtractor() = default;
	KeyValuesExtractor(const KeyValuesExtractor& other) = delete;
	KeyValuesExtractor(const KeyValuesExtractor&& other) = delete;
	KeyValuesExtractor& operator=(const KeyValuesExtractor& other) = delete;
	KeyValuesExtractor& operator=(const KeyValuesExtractor&& other) = delete;

	//! @brief Tries to transforms the selections into the selected data types.
	//! @throws If a failure accures in the data transformation
	void loadAllRangeSelectionInformation(const MetadataAssemblyData& _assemblyData, std::map<std::string, std::shared_ptr<IVisualisationTable>>& _allTablesByName);


	std::map<std::string, std::list<ot::Variable>>* getFields() { return &m_fields; };

	uint64_t getNumberOfFields() const;

private:

	//! @brief Extracts the selected ranges from the table. All cell values are string values. 
	//! They are sorted by a key value, which is the corresponding value in either the first row or collumn of the table.range 
	//! Each is contained in a map with its index (row or column) as key. 
	std::vector<std::string> extractFieldsFromRange(std::shared_ptr<EntityTableSelectedRanges> _range, std::shared_ptr<IVisualisationTable> _table, std::map<std::string, std::map<std::uint32_t, ot::Variable>>& _outAllSortedFields);

	//! @brief Empties allFields and adds its content after casting the values to the corresponding member attribute.
	//! @return true, if all conversions were possible. False, if a conversion failed
	bool transformSelectedDataIntoSelectedDataType(std::map<std::string, std::list<ot::Variable>>& _allFields, std::map<std::string, std::string>& _rangeTypesByRangeNames, const char _decimalSeparator);
	
	std::map<std::string,std::list<ot::Variable>> m_fields;
};


