#include "KeyValuesExtractor.h"
#include "DataCategorizationHandler.h"
#include "OTCore/StringToNumericCheck.h"
#include "OTCore/StringToVariableConverter.h"
#include "Documentation.h"
#include "OTServiceFoundation/TableIndexSchemata.h"


void KeyValuesExtractor::loadAllRangeSelectionInformation(const MetadataAssemblyData& _assemblyData , std::map<std::string, std::shared_ptr<IVisualisationTable>>& _allTablesByName)
{
	bool allEntriesSuccessfullyTransformed = true;
	for (auto tableByName : _allTablesByName)
	{
		const std::string& tableName = tableByName.first;
		const std::shared_ptr<IVisualisationTable> table = tableByName.second;

		ot::GenericDataStructMatrix tableContent =	table->getTable();

		
		//Sort out all ranges associated with the this table
		std::list<std::shared_ptr<EntityTableSelectedRanges>> relevantSelections;
		auto& allRanges = _assemblyData.m_allSelectionRanges;
		bool allRangesAreValid = true;
		std::string defectRanges("");
		for (const auto& range : allRanges)
		{
			if (range->getTableName() == tableName)
			{
				relevantSelections.push_back(range);
				bool rangeIsValid =	isRangeWithinTableDimensions(range,tableContent);
				allRangesAreValid &= rangeIsValid;
				if (rangeIsValid)
				{
					defectRanges += range->getName() + ", ";
				}
			}
		}
		
		if (!allRangesAreValid)
		{
			const std::string message = defectRanges.substr(defectRanges.size() - 2);
			throw std::exception(message.c_str());
		}

		//Extract content that is refered by the ranges
		std::map<std::string, std::map<std::uint32_t, ot::Variable>> allFieldsSorted;
		std::map<std::string, std::string> rangeTypesByRangeNames;

		for (const auto& range : relevantSelections)
		{
			const std::vector<std::string> fieldKeys = extractFieldsFromRange(range, tableContent, allFieldsSorted);
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

bool KeyValuesExtractor::isRangeWithinTableDimensions(std::shared_ptr<EntityTableSelectedRanges> _range, const ot::GenericDataStructMatrix& _tableData)
{
	int32_t maxColumns = static_cast<int32_t>(_tableData.getNumberOfColumns());
	int32_t maxRows = static_cast<int32_t>(_tableData.getNumberOfRows());

	ot::TableRange userRange =	_range->getSelectedRange();
	ot::TableRange matrixRange = ot::TableIndexSchemata::userRangeToMatrixRange(userRange, _range->getTableHeaderMode());

	bool rowsAreValid = matrixRange.getTopRow() >= 0 &&
		matrixRange.getBottomRow() < maxRows;
	
	bool columnsAreValid = matrixRange.getLeftColumn() >= 0 &&
		matrixRange.getRightColumn() < maxColumns;

	bool selectionIsValid = true;
	if (!_range->getSelectEntireRow())
	{
		selectionIsValid &= columnsAreValid;
	}
	if (!_range->getSelectEntireColumn())
	{
		selectionIsValid &= rowsAreValid;
	}
	
	return selectionIsValid;
}

std::vector<std::string> KeyValuesExtractor::extractFieldsFromRange(std::shared_ptr<EntityTableSelectedRanges> _range, const ot::GenericDataStructMatrix& _tableData, std::map<std::string, std::map<std::uint32_t, ot::Variable>>& _outAllSortedFields)
{
	std::vector<std::string> fieldKeys;
	
	ot::TableRange userRange = _range->getSelectedRange();
	ot::TableCfg::TableHeaderMode headerOrientation = _range->getTableHeaderMode();
	ot::TableRange matrixRange = ot::TableIndexSchemata::userRangeToMatrixRange(userRange,headerOrientation);
	
	std::map<std::string, std::map<std::uint32_t,std::string>> extractedField;

	//Setup the min/max values of the interation range, considering the property that allows to select an entire row/column
	uint32_t minColumn(-1), maxColumn(-1), minRow(-1), maxRow(-1), minColumnOfTableContent(0), minRowOfTableContent(0);
	if (_range->getTableHeaderMode() == ot::TableCfg::TableHeaderMode::Horizontal)
	{
		minRowOfTableContent = 1;
	}
	if (_range->getTableHeaderMode() == ot::TableCfg::TableHeaderMode::Vertical)
	{
		minColumnOfTableContent = 1;
	}

	if (_range->getSelectEntireColumn())
	{
		minRow = minRowOfTableContent;
		maxRow = _tableData.getNumberOfRows() - 1;
	}
	else
	{

		minRow = static_cast<uint32_t>(matrixRange.getTopRow());
		maxRow = static_cast<uint32_t>(matrixRange.getBottomRow());
	}

	if (_range->getSelectEntireRow())
	{
		minColumn = minColumnOfTableContent;
		maxColumn = _tableData.getNumberOfColumns() - 1;
	}
	else
	{

		minColumn = static_cast<uint32_t>(matrixRange.getLeftColumn());
		maxColumn = static_cast<uint32_t>(matrixRange.getRightColumn());
	}


	ot::MatrixEntryPointer matrixEntry;
	if (_range->getTableHeaderMode() == ot::TableCfg::TableHeaderMode::Horizontal)
	{
		fieldKeys.reserve(maxColumn - minColumn + 1);
		for (matrixEntry.m_column = minColumn; matrixEntry.m_column <= maxColumn; matrixEntry.m_column++)
		{
			matrixEntry.m_row = 0;
			const ot::Variable& entry = _tableData.getValue(matrixEntry);
			assert(entry.isConstCharPtr());
			std::string fieldKey = entry.getConstCharPtr();
			cleanFieldKey(fieldKey);
			fieldKeys.push_back(fieldKey);
			for (matrixEntry.m_row = minRow; matrixEntry.m_row <= maxRow; matrixEntry.m_row++)
			{
				const ot::Variable& entry = _tableData.getValue(matrixEntry);
				_outAllSortedFields[fieldKey].insert({matrixEntry.m_row, entry});
			}
		}
	}
	else
	{
		fieldKeys.reserve(maxRow - minRow + 1);
		for (matrixEntry.m_row = minRow; matrixEntry.m_row <= maxRow; matrixEntry.m_row++)
		{
			matrixEntry.m_column = 0;
			const ot::Variable& entry = _tableData.getValue(matrixEntry);
			assert(entry.isConstCharPtr());
			std::string fieldKey = entry.getConstCharPtr();
			cleanFieldKey(fieldKey);
			fieldKeys.push_back(fieldKey);
			for (matrixEntry.m_column = minColumn; matrixEntry.m_column <= maxColumn; matrixEntry.m_column++)
			{
				const ot::Variable& value = _tableData.getValue(matrixEntry);
				_outAllSortedFields[fieldKey].insert({ matrixEntry.m_column, value});
			}
		}
	}
	fieldKeys.shrink_to_fit();
	return fieldKeys;
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

void KeyValuesExtractor::cleanFieldKey(std::string& _key)
{
	_key.erase(std::remove(_key.begin(), _key.end(), '"'), _key.end());
}
