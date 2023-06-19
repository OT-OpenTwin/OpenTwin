#include "MetadataAssemblyRangeData.h"

void MetadataAssemblyRangeData::LoadAllRangeSelectionInformation(const std::list<std::shared_ptr<EntityTableSelectedRanges>>& allRanges, std::map<std::string, std::shared_ptr<EntityParameterizedDataTable>>& allTables)
{
	std::map<std::string, std::list<std::string>> allFields;
	std::map<std::string, std::string> rangeTypesByRangeNames;

	std::map<std::string, std::map<std::uint32_t, std::string>> allFieldsSorted;
	for (const auto& range : allRanges)
	{
		auto table = allTables[range->getTableName()];
		const std::vector<std::string> fieldKeys = ExtractFieldsFromRange(range, table, allFieldsSorted);
		const std::string& type = range->getSelectedType();
		for (const std::string fieldKey : fieldKeys)
		{
			rangeTypesByRangeNames[fieldKey] = type;
		}
	}

	for (auto newField : allFieldsSorted)
	{
		const std::string& fieldKey = newField.first;
		std::map<std::uint32_t, std::string>& fieldValues = newField.second;
		for (auto& fieldValue : fieldValues)
		{
			allFields[fieldKey].push_back(std::move(fieldValue.second));
		}
	}

	TransformSelectedDataIntoSelectedDataType(allFields, rangeTypesByRangeNames);
}

uint64_t MetadataAssemblyRangeData::getNumberOfFields() const
{
	return _stringFields.size() + _doubleFields.size() + _int32Fields.size() + _int64Fields.size();
}

std::vector<std::string> MetadataAssemblyRangeData::ExtractFieldsFromRange(std::shared_ptr<EntityTableSelectedRanges> range, std::shared_ptr<EntityParameterizedDataTable> table, std::map<std::string, std::map<std::uint32_t, std::string>>& outAllSortedFields)
{
	std::vector<std::string> fieldKey;
	int offsetDueToHeader = 1;
	uint32_t selection[4];
	range->getSelectedRange(selection[0], selection[1], selection[2], selection[3]);
	
	std::map<std::string, std::map<std::uint32_t,std::string>> extractedField;

	if (range->getTableOrientation() == EntityParameterizedDataTable::GetHeaderOrientation(EntityParameterizedDataTable::HeaderOrientation::horizontal))
	{
		for (uint32_t column = selection[2]; column <= selection[3]; column++)
		{
			fieldKey.push_back(table->getTableData()->getValue(0, column));
			for (uint32_t row = selection[0] + offsetDueToHeader; row <= selection[1] + offsetDueToHeader; row++)
			{
				const std::string& value = table->getTableData()->getValue(row, column);
				outAllSortedFields[fieldKey.back()].insert({row, value});
			}
		}
	}
	else
	{
		for (uint32_t row = selection[0]; row <= selection[1]; row++)
		{
			fieldKey.push_back(table->getTableData()->getValue(row, 0));
			for (uint32_t column = selection[2] + offsetDueToHeader; column <= selection[3] + offsetDueToHeader; column++)
			{
				const std::string& value = table->getTableData()->getValue(row, column);
				outAllSortedFields[fieldKey.back()].insert({column, value});
			}
		}
	}


	return fieldKey;
}

void MetadataAssemblyRangeData::TransformSelectedDataIntoSelectedDataType(std::map<std::string, std::list<std::string>>& allFields, std::map<std::string, std::string>& rangeTypesByRangeNames)
{
	auto currentField = allFields.begin();
	while(currentField != allFields.end())
	{
		const std::string& fieldName = currentField->first;
		const std::list<std::string>& fieldValuesRaw = currentField->second;
		if (rangeTypesByRangeNames[fieldName] == ot::TypeNames::getDoubleTypeName())
		{
			std::list<double> fieldValues;
			for (const std::string& fieldValueString : fieldValuesRaw)
			{
				fieldValues.push_back(std::stod(fieldValueString));
			}
			_doubleFields.insert({ fieldName, fieldValues });
			currentField = allFields.erase(currentField);
		}
		else if (rangeTypesByRangeNames[fieldName] == ot::TypeNames::getInt32TypeName())
		{
			std::list<int32_t> fieldValues;
			for (std::string fieldValueString : fieldValuesRaw)
			{
				fieldValues.push_back(std::stoi(fieldValueString));
			}
			_int32Fields.insert({fieldName, fieldValues });
			currentField = allFields.erase(currentField);
		}
		else if (rangeTypesByRangeNames[fieldName] == ot::TypeNames::getInt64TypeName())
		{
			std::list<int64_t> fieldValues;
			for (std::string fieldValueString : fieldValuesRaw)
			{
				fieldValues.push_back(std::stoll(fieldValueString));
			}
			_int64Fields.insert({ fieldName, fieldValues });
			currentField = allFields.erase(currentField);
		}
		else if (rangeTypesByRangeNames[fieldName] == ot::TypeNames::getStringTypeName())
		{
			_stringFields.insert({ fieldName, fieldValuesRaw});
			currentField = allFields.erase(currentField);
		}
	}
	assert(allFields.size() == 0);
}
