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
	return _fields.size();
}

std::vector<std::string> MetadataAssemblyRangeData::ExtractFieldsFromRange(std::shared_ptr<EntityTableSelectedRanges> range, std::shared_ptr<EntityParameterizedDataTable> table, std::map<std::string, std::map<std::uint32_t, std::string>>& outAllSortedFields)
{
	std::vector<std::string> fieldKey;
	uint32_t selection[4];
	range->getSelectedRange(selection[0], selection[1], selection[2], selection[3], table);

	std::map<std::string, std::map<std::uint32_t,std::string>> extractedField;

	if (range->getTableOrientation() == EntityParameterizedDataTable::GetHeaderOrientation(EntityParameterizedDataTable::HeaderOrientation::horizontal))
	{
		for (uint32_t column = selection[2]; column <= selection[3]; column++)
		{
			fieldKey.push_back(table->getTableData()->getValue(0, column));
			for (uint32_t row = selection[0]; row <= selection[1]; row++)
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
			for (uint32_t column = selection[2]; column <= selection[3]; column++)
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
		std::list<ot::Variable> values;
		for (const std::string& fieldValueString : fieldValuesRaw)
		{
			if (rangeTypesByRangeNames[fieldName] == ot::TypeNames::getStringTypeName())
			{
				values.push_back(ot::Variable(fieldValueString));
			}
			else
			{
				std::string fieldValueStringFixed;
				if (fieldValueString == "")
				{
					fieldValueStringFixed = "0";
				}
				else
				{
					fieldValueStringFixed = fieldValueString;
				}

				if (rangeTypesByRangeNames[fieldName] == ot::TypeNames::getDoubleTypeName())
				{
					values.push_back(ot::Variable(std::stod(fieldValueStringFixed)));
				}
				else if (rangeTypesByRangeNames[fieldName] == ot::TypeNames::getInt32TypeName())
				{
					values.push_back(ot::Variable(std::stoi(fieldValueStringFixed)));
				}
				else if (rangeTypesByRangeNames[fieldName] == ot::TypeNames::getInt64TypeName())
				{
					values.push_back(ot::Variable(std::stoll(fieldValueStringFixed)));
				}
			}
		}
		_fields.insert({ fieldName,values });
		currentField = allFields.erase(currentField);
	}
	assert(allFields.size() == 0);
}
