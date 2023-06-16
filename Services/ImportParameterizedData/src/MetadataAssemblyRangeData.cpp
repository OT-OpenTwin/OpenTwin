#include "MetadataAssemblyRangeData.h"

void MetadataAssemblyRangeData::LoadAllRangeSelectionInformation(const std::list<std::shared_ptr<EntityTableSelectedRanges>>& allRanges, std::map<std::string, std::shared_ptr<EntityParameterizedDataTable>>& allTables)
{
	std::map<std::string, std::list<std::string>> allFields;
	std::map<std::string, std::string> rangeTypesByRangeNames;

	for (const auto& range : allRanges)
	{
		auto table = allTables[range->getTableName()];
		auto newFields = ExtractFieldsFromRange(range, table);
		for (auto newField : newFields)
		{
			const std::string& fieldKey = newField.first;
			std::list<std::string>& fieldValues = newField.second;
			if (allFields.find(fieldKey) != allFields.end())
			{
				allFields[fieldKey].splice(allFields[fieldKey].end(), fieldValues);
			}
			else
			{
				allFields.insert({ fieldKey, std::move(fieldValues)});
				const std::string& type = range->getSelectedType();
				rangeTypesByRangeNames[fieldKey] = type;
			}
		}
	}

	TransformSelectedDataIntoSelectedDataType(allFields, rangeTypesByRangeNames);
}

uint64_t MetadataAssemblyRangeData::getNumberOfFields() const
{
	return _stringFields.size() + _doubleFields.size() + _int32Fields.size() + _int64Fields.size();
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
			const std::string& fieldKey = table->getTableData()->getValue(0, column);
			for (uint32_t row = selection[0] + offsetDueToHeader; row <= selection[1] + offsetDueToHeader; row++)
			{
				const std::string& value = table->getTableData()->getValue(row, column);
				extractedField[fieldKey].push_back(value);
			}
		}
	}
	else
	{
		for (uint32_t row = selection[0]; row <= selection[1]; row++)
		{
			const std::string& fieldKey = table->getTableData()->getValue(row, 0);
			for (uint32_t column = selection[2] + offsetDueToHeader; column <= selection[3] + offsetDueToHeader; column++)
			{
				const std::string& value = table->getTableData()->getValue(row, column);
				extractedField[fieldKey].push_back(value);
			}
		}
	}
	return extractedField;
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
