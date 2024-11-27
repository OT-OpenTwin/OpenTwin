#include "KeyValuesExtractor.h"
#include "DataCategorizationHandler.h"
#include "OTCore/StringToNumericCheck.h"
#include "OTCore/StringToVariableConverter.h"
#include "Documentation.h"


void KeyValuesExtractor::loadAllRangeSelectionInformation(const MetadataAssemblyData& _assemblyData , std::map<std::string, std::shared_ptr<IVisualisationTable>>& _allTablesByName)
{
	bool allEntriesSuccessfullyTransformed = true;
	for (auto tableByName : _allTablesByName)
	{
		const std::string& tableName = tableByName.first;
		const std::shared_ptr<IVisualisationTable> table = tableByName.second;
		
		//Sort out all ranges associated with the this table
		std::list<std::shared_ptr<EntityTableSelectedRanges>> relevantSelections;
		auto& allRanges = _assemblyData.m_allSelectionRanges;
		for (const auto& range : allRanges)
		{
			if (range->getTableName() == tableName)
			{
				relevantSelections.push_back(range);
			}
		}
		
		//Extract content that is refered by the ranges
		std::map<std::string, std::map<std::uint32_t, ot::Variable>> allFieldsSorted;
		std::map<std::string, std::string> rangeTypesByRangeNames;

		for (const auto& range : relevantSelections)
		{
			const std::vector<std::string> fieldKeys = extractFieldsFromRange(range, table, allFieldsSorted);
			const std::string& type = range->getSelectedType();
			
			for (const std::string fieldKey : fieldKeys)
			{
				rangeTypesByRangeNames[fieldKey] = type;
			}
		}
		
		//The values of all selection ranges are now sorted by their index (column or row, depending on header orientation). Now we drop the index information, since only the values are needed.
		std::map<std::string, std::list<ot::Variable>> allFields;
		for (auto& newField : allFieldsSorted)
		{
			const std::string& fieldKey = newField.first;
			std::map<std::uint32_t, ot::Variable>& fieldValues = newField.second;
			for (auto& fieldValue : fieldValues)
			{
				allFields[fieldKey].push_back(std::move(fieldValue.second));
			}
		}
		
		char decimalDelimiter = table->getDecimalDelimiter();
		bool entriesSuccessfullyTransformed = transformSelectedDataIntoSelectedDataType(allFields, rangeTypesByRangeNames, decimalDelimiter);
		if (!entriesSuccessfullyTransformed)
		{
			allEntriesSuccessfullyTransformed = false;
			Documentation::INSTANCE()->AddToDocumentation("The listed failure was detected in table: " + tableName + "\n");
		}
	}
	if (!allEntriesSuccessfullyTransformed)
	{
		throw std::exception("Failed to transform table entries into the desired format.");
	}
}

uint64_t KeyValuesExtractor::getNumberOfFields() const
{
	return m_fields.size();
}

std::vector<std::string> KeyValuesExtractor::extractFieldsFromRange(std::shared_ptr<EntityTableSelectedRanges> _range, std::shared_ptr<IVisualisationTable> _table, std::map<std::string, std::map<std::uint32_t, ot::Variable>>& _outAllSortedFields)
{
	std::vector<std::string> fieldKey;
	
	ot::TableRange userRange = _range->getSelectedRange();
	ot::TableHeaderOrientation headerOrientation=	_range->getTableOrientation();
	ot::TableRange matrixRange = DataCategorizationHandler::userRangeToMatrixRange(userRange,headerOrientation);
	
	std::map<std::string, std::map<std::uint32_t,std::string>> extractedField;

	uint32_t minColumn = static_cast<uint32_t>(matrixRange.getLeftColumn());
	uint32_t maxColumn = static_cast<uint32_t>(matrixRange.getRightColumn());
	uint32_t minRow = static_cast<uint32_t>(matrixRange.getTopRow());
	uint32_t maxRow = static_cast<uint32_t>(matrixRange.getBottomRow());

	const ot::GenericDataStructMatrix tableData = _table->getTable();

	ot::MatrixEntryPointer matrixEntry;
	if (_range->getTableOrientation() == ot::TableHeaderOrientation::horizontal)
	{
		fieldKey.reserve(maxColumn - minColumn + 1);
		for (matrixEntry.m_column = minColumn; matrixEntry.m_column <= maxColumn; matrixEntry.m_column++)
		{
			matrixEntry.m_row = 0;
			const ot::Variable& entry = tableData.getValue(matrixEntry);
			assert(entry.isConstCharPtr());
			fieldKey.push_back(entry.getConstCharPtr());
			for (matrixEntry.m_row = minRow; matrixEntry.m_row <= maxRow; matrixEntry.m_row++)
			{
				const ot::Variable& entry = tableData.getValue(matrixEntry);
				_outAllSortedFields[fieldKey.back()].insert({matrixEntry.m_row, entry});
			}
		}
	}
	else
	{
		fieldKey.reserve(maxRow - minRow + 1);
		for (matrixEntry.m_row = minRow; matrixEntry.m_row <= maxRow; matrixEntry.m_row++)
		{
			matrixEntry.m_column = 0;
			const ot::Variable& entry = tableData.getValue(matrixEntry);
			assert(entry.isConstCharPtr());
			fieldKey.push_back(entry.getConstCharPtr());
			for (matrixEntry.m_column = minColumn; matrixEntry.m_column <= maxColumn; matrixEntry.m_column++)
			{
				const ot::Variable& value = tableData.getValue(matrixEntry);
				_outAllSortedFields[fieldKey.back()].insert({ matrixEntry.m_column, value});
			}
		}
	}
	fieldKey.shrink_to_fit();
	return fieldKey;
}


bool KeyValuesExtractor::transformSelectedDataIntoSelectedDataType(std::map<std::string, std::list<ot::Variable>>& _allFields, std::map<std::string, std::string>& _rangeTypesByRangeNames, const char _decimalSeparator)
{
	bool allFieldsConverted = true;
	auto currentField = _allFields.begin();
	const std::string errorBase = "Failed to convert value: ";
	while(currentField != _allFields.end())
	{
		const std::string& fieldName = currentField->first;
		const std::list<ot::Variable>& fieldValuesRaw = currentField->second;
		std::list<ot::Variable> values;
		for (const ot::Variable& fieldValueRaw : fieldValuesRaw)
		{
			assert(fieldValueRaw.isConstCharPtr());
			if (_rangeTypesByRangeNames[fieldName] == ot::TypeNames::getStringTypeName())
			{
				values.push_back(fieldValueRaw);
			}
			else
			{
				std::string fieldValueStringFixed = fieldValueRaw.getConstCharPtr();
				ot::StringToVariableConverter converter;
				converter.normaliseNumericString(fieldValueStringFixed, _decimalSeparator);
				if (fieldValueStringFixed == "")
				{
					fieldValueStringFixed = "0";
				}

				if (_rangeTypesByRangeNames[fieldName] == ot::TypeNames::getDoubleTypeName())
				{
					const bool dataTypeFits =	ot::StringToNumericCheck::fitsInDouble(fieldValueStringFixed);
					if (dataTypeFits)
					{
						const double valueOfCastedType = std::stod(fieldValueStringFixed);
						values.push_back(ot::Variable(valueOfCastedType));
					}
					else
					{
						Documentation::INSTANCE()->AddToDocumentation(errorBase + fieldName + " = " + fieldValueRaw.getConstCharPtr() + " intended type: " + _rangeTypesByRangeNames[fieldName] + "\n");
						allFieldsConverted = false;
					}
				}
				else if (_rangeTypesByRangeNames[fieldName] == ot::TypeNames::getFloatTypeName())
				{
					const bool dataTypeFits = ot::StringToNumericCheck::fitsInFloat(fieldValueStringFixed);
					if (dataTypeFits)
					{
						const double valueOfCastedType = std::stof(fieldValueStringFixed);
						values.push_back(ot::Variable(valueOfCastedType));
					}
					else
					{
						Documentation::INSTANCE()->AddToDocumentation(errorBase + fieldName + " = " + fieldValueRaw.getConstCharPtr() + " intended type: " + _rangeTypesByRangeNames[fieldName] + "\n");
						allFieldsConverted = false;
					}
				}
				else if (_rangeTypesByRangeNames[fieldName] == ot::TypeNames::getInt32TypeName())
				{
					bool dataTypeFits = ot::StringToNumericCheck::fitsInInt32(fieldValueStringFixed);
					if (dataTypeFits)
					{
						const double valueOfCastedType = std::stoi(fieldValueStringFixed);
						values.push_back(ot::Variable(valueOfCastedType));
					}
					else
					{
						Documentation::INSTANCE()->AddToDocumentation(errorBase + fieldName + " = " + fieldValueRaw.getConstCharPtr() + " intended type: " + _rangeTypesByRangeNames[fieldName] + "\n");
						allFieldsConverted = false;
					}
				}
				else if (_rangeTypesByRangeNames[fieldName] == ot::TypeNames::getInt64TypeName())
				{
					bool dataTypeFits = ot::StringToNumericCheck::fitsInInt64(fieldValueStringFixed);
					if (dataTypeFits)
					{
						const int64_t valueOfCastedType = std::stoll(fieldValueStringFixed);
						values.push_back(ot::Variable(valueOfCastedType));
					}
					else
					{
						Documentation::INSTANCE()->AddToDocumentation(errorBase + fieldName + " = " + fieldValueRaw.getConstCharPtr() + " intended type: " + _rangeTypesByRangeNames[fieldName] + "\n");
						allFieldsConverted = false;
					}
				}
			}
		}
		m_fields.insert({ fieldName,values });
		currentField = _allFields.erase(currentField);
	}
	assert(_allFields.size() == 0);
	return allFieldsConverted;
}
