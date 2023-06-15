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

	std::map<std::string, std::list<std::string>> GetStringFields() const { return _stringFields; };
	std::map<std::string, std::list<double>> GetDoubleFields() const { return _doubleFields; };
	std::map<std::string, std::list<int32_t>> GetInt32Fields() const { return _int32Fields; };
	std::map<std::string, std::list<int64_t>> GetInt64Fields() const { return _int64Fields; };

	uint64_t getNumberOfFields() const;
private:
	std::map<std::string, std::list<std::string>> ExtractFieldsFromRange(std::shared_ptr<EntityTableSelectedRanges> range, std::shared_ptr<EntityParameterizedDataTable> table);
	void TransformSelectedDataIntoSelectedDataType(std::map<std::string, std::list<std::string>>& allFields, std::map<std::string, std::string>& rangeTypesByRangeNames);
	
	std::map<std::string,std::list<std::string>> _stringFields;
	std::map<std::string,std::list<double>> _doubleFields;
	std::map<std::string,std::list<int32_t>> _int32Fields;
	std::map<std::string,std::list<int64_t>> _int64Fields;

};


