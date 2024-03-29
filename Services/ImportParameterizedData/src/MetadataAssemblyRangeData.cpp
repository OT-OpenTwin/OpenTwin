#include "MetadataAssemblyRangeData.h"
#include "LocaleSettingsSwitch.h"

MetadataAssemblyRangeData::MetadataAssemblyRangeData()
	: _decimalDelimmiter(*std::localeconv()->decimal_point)
{
}

void MetadataAssemblyRangeData::LoadAllRangeSelectionInformation(const std::list<std::shared_ptr<EntityTableSelectedRanges>>& allRanges, std::map<std::string, std::shared_ptr<EntityParameterizedDataTable>>& allTables)
{
	for (auto tableByName : allTables)
	{
		const std::string& tableName = tableByName.first;
		const std::shared_ptr<EntityParameterizedDataTable> table = tableByName.second;
		
		//Sort out all ranges associated with the this table
		std::list<std::shared_ptr<EntityTableSelectedRanges>> relevantSelections;
		for (const auto& range : allRanges)
		{
			if (range->getTableName() == tableName)
			{
				relevantSelections.push_back(range);
			}
		}
		
		//Extract content that is refered by the ranges
		std::map<std::string, std::map<std::uint32_t, std::string>> allFieldsSorted;
		std::map<std::string, std::string> rangeTypesByRangeNames;

		for (const auto& range : relevantSelections)
		{
			const std::vector<std::string> fieldKeys = ExtractFieldsFromRange(range, table, allFieldsSorted);
			const std::string& type = range->getSelectedType();
			for (const std::string fieldKey : fieldKeys)
			{
				rangeTypesByRangeNames[fieldKey] = type;
			}
		}
		
		//The values of all selection ranges are now sorted by their index (column or row, depending on header orientation). Now we drop the index information, since only the values are needed.
		std::map<std::string, std::list<std::string>> allFields;
		for (auto& newField : allFieldsSorted)
		{
			const std::string& fieldKey = newField.first;
			std::map<std::uint32_t, std::string>& fieldValues = newField.second;
			for (auto& fieldValue : fieldValues)
			{
				allFields[fieldKey].push_back(std::move(fieldValue.second));
			}
		}

		//For floating numbers it is essential to choose the correct decimal delimiter.
		const char selectedSeparator = table->getSelectedDecimalSeparator();
		LocaleSettingsSwitch decimalSeparatorSetting(selectedSeparator);

		TransformSelectedDataIntoSelectedDataType(allFields, rangeTypesByRangeNames);
	}

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
					const double valueOfCastedType = std::stod(fieldValueStringFixed);
					values.push_back(ot::Variable(valueOfCastedType));
				}
				else if (rangeTypesByRangeNames[fieldName] == ot::TypeNames::getInt32TypeName())
				{
					const int32_t valueOfCastedType = std::stoi(fieldValueStringFixed);
					values.push_back(ot::Variable(valueOfCastedType));
				}
				else if (rangeTypesByRangeNames[fieldName] == ot::TypeNames::getInt64TypeName())
				{
					const int64_t valueOfCastedType = std::stoll(fieldValueStringFixed);
					values.push_back(ot::Variable(valueOfCastedType));
				}
			}
		}
		_fields.insert({ fieldName,values });
		currentField = allFields.erase(currentField);
	}
	assert(allFields.size() == 0);
}
