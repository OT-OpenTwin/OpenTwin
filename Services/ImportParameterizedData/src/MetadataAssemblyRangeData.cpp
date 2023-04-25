#include "MetadataAssemblyRangeData.h"

void MetadataAssemblyRangeData::LoadAllRangeSelectionInformation(std::list<std::shared_ptr<EntityTableSelectedRanges>>& allRanges, std::map<std::string, std::shared_ptr<EntityParameterizedDataTable>>& allTables)
{
	std::map<std::string, std::list<std::string>> allFields;
	std::map<std::string, std::string> rangeTypesByRangeNames;

	for (auto range : allRanges)
	{
		auto table = allTables[range->getTableName()];
		auto newFields = ExtractFieldsFromRange(range, table);
		for (auto newField : newFields)
		{
			if (allFields.find(newField.first) != allFields.end())
			{
				allFields[newField.first].merge(newField.second);
			}
			else
			{
				allFields.insert({ newField.first, newField.second });
				std::string type = range->getSelectedType();
				rangeTypesByRangeNames[newField.first] = type;
			}
		}
	}

	TransformSelectedDataIntoSelectedDataType(allFields, rangeTypesByRangeNames);
}

std::map<std::string, std::list<std::string>> MetadataAssemblyRangeData::ExtractFieldsFromRange(std::shared_ptr<EntityTableSelectedRanges> range, std::shared_ptr<EntityParameterizedDataTable> table)
{
	int offsetDueToHeader = 1;
	uint32_t selection[4];
	range->getSelectedRange(selection[0], selection[1], selection[2], selection[3]);
	std::map<std::string, std::list<std::string>> extractedField;

	if (range->getTableOrientation() == EntityParameterizedDataTable::GetHeaderOrientation(EntityParameterizedDataTable::HeaderOrientation::horizontal))
	{
		for (uint32_t column = selection[2]; column <= selection[3]; column++)
		{
			for (uint32_t row = selection[0]; row <= selection[1]; row++)
			{
				extractedField[table->getTableData()->getValue(0, column)].push_back(table->getTableData()->getValue(row + offsetDueToHeader, column));
			}
		}
	}
	else
	{
		for (uint32_t row = selection[0]; row <= selection[1]; row++)
		{
			for (uint32_t column = selection[2]; column <= selection[3]; column++)
			{
				extractedField[table->getTableData()->getValue(row, 0)].push_back(table->getTableData()->getValue(row, column + offsetDueToHeader));
			}
		}
	}
	return std::move(extractedField);
}

void MetadataAssemblyRangeData::TransformSelectedDataIntoSelectedDataType(std::map<std::string, std::list<std::string>>& allFields, std::map<std::string, std::string>& rangeTypesByRangeNames)
{
	auto it = allFields.begin();
	while(it != allFields.end())
	{
		if (rangeTypesByRangeNames[(*it).first] == ot::TypeNames::getDoubleTypeName())
		{
			std::list<double> fieldValues;
			for (std::string fieldValueString : (*it).second)
			{
				fieldValues.push_back(std::stod(fieldValueString));
			}
			_doubleFields.insert({ (*it).first, fieldValues });
			it = allFields.erase(it);
		}
		else if (rangeTypesByRangeNames[(*it).first] == ot::TypeNames::getInt32TypeName())
		{
			std::list<int32_t> fieldValues;
			for (std::string fieldValueString : (*it).second)
			{
				fieldValues.push_back(std::stoi(fieldValueString));
			}
			_int32Fields.insert({ (*it).first, fieldValues });
			it = allFields.erase(it);
		}
		else if (rangeTypesByRangeNames[(*it).first] == ot::TypeNames::getInt64TypeName())
		{
			std::list<int64_t> fieldValues;
			for (std::string fieldValueString : (*it).second)
			{
				fieldValues.push_back(std::stoll(fieldValueString));
			}
			_int64Fields.insert({ (*it).first, fieldValues });
			it = allFields.erase(it);
		}
		else if (rangeTypesByRangeNames[(*it).first] == ot::TypeNames::getStringTypeName())
		{
			_stringFields.insert({ (*it).first, (*it).second});
			it = allFields.erase(it);
		}
	}
}
