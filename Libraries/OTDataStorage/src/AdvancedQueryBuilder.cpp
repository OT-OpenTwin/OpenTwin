// @otlicense
// File: AdvancedQueryBuilder.cpp
// 
// License:
// Copyright 2025 by OpenTwin
//  
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
//  
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// @otlicense-end

// OpenTwin header
#include "OTCore/String.h"
#include "OTCore/Variable/VariableToBSONStringConverter.h"
#include "OTCore/Variable/ExplicitStringValueConverter.h"
#include "OTDataStorage/AdvancedQueryBuilder.h"
#include "OTCore/ComplexNumbers/ComplexNumberConversion.h"
#include "OTCore/ComplexNumbers/ComplexNumberFormat.h"
#include "OTCore/Tuple/TupleDescriptionComplex.h"
#include "OTCore/Tuple/TupleFactory.h"
// std header
#include <cassert>
#include <stdarg.h>
#include "bsoncxx/json.hpp"

std::list<std::string> AdvancedQueryBuilder::extractMongoComparators(ot::ValueComparisonDescription& _definition)
{
	std::list<std::string> mongoDBComparators;
	const std::string comparator = _definition.getComparator();

	// Guards
	if (comparator == ot::ComparisonSymbols::g_anyOneOfComparator ||
		comparator == ot::ComparisonSymbols::g_rangeComparator ||
		comparator == ot::ComparisonSymbols::g_noneOfComparator)
	{
		noTupleAllowedCheck(_definition);
	}

	if (queryTargetsEntireTuple(_definition) && comparator != "=")
	{
		throw std::exception(("A query for a complex number requires both values specified and separated by a \"" + m_listSplitToken + "\".").c_str());
	}

	// Transform to mongodb comparators
	if (comparator == ot::ComparisonSymbols::g_rangeComparator)
	{
		std::string valueStr = _definition.getValue();
		const std::string& type = _definition.getTupleInstance().getTupleElementDataTypes().front();

		valueStr.erase(std::remove(valueStr.begin(), valueStr.end(), ' '), valueStr.end());
		char openingBracket = valueStr[0];
		char closingBracket = valueStr[valueStr.size() - 1];
		valueStr.erase(valueStr.begin());
		valueStr.erase(valueStr.end() - 1);
		size_t posDelimiter = valueStr.find(",");
		const bool notationIsCorrect = (openingBracket == '(' || openingBracket == '[') && (closingBracket == ')' || closingBracket == ']') && posDelimiter != std::string::npos;

		if (notationIsCorrect)
		{
			std::string correspondingComparator;
			if (openingBracket == '(') {
				correspondingComparator = ">";
			}
			else {
				correspondingComparator = ">=";
			}
			auto mongoComparator = m_mongoDBComparators.find(correspondingComparator);
			assert(mongoComparator != m_mongoDBComparators.end());
			mongoDBComparators.push_back(mongoComparator->second);

			if (closingBracket == ')') {
				correspondingComparator = "<";
			}
			else {
				correspondingComparator = "<=";
			}
			mongoComparator = m_mongoDBComparators.find(correspondingComparator);
			assert(mongoComparator != m_mongoDBComparators.end());
			mongoDBComparators.push_back(mongoComparator->second);
		}
		else 
		{
			throw std::invalid_argument("Query for interval incorrect. The interval has to follow the English notation, e.g.: (2,3.5].");
		}
		//Remove the brackets from the value string
		_definition.setValue(valueStr.substr(1, valueStr.size() - 2));
	}
	else if (comparator == ot::ComparisonSymbols::g_anyOneOfComparator)
	{
		mongoDBComparators.push_back("$in");

	}
	else if (comparator == ot::ComparisonSymbols::g_noneOfComparator)
	{
		mongoDBComparators.push_back("$nin");
	}
	else
	{
		auto mongoComparator = m_mongoDBComparators.find(comparator);
		mongoDBComparators.push_back(mongoComparator->second);
	}
	
	return mongoDBComparators;
}

std::vector<ot::Variable> AdvancedQueryBuilder::getListOfValuesFromString(const ot::ValueComparisonDescription& _definition)
{
	const std::string allValues = _definition.getValue();
	std::list<std::string> separatedValues = ot::String::split(allValues, m_listSplitToken);
	std::vector<ot::Variable> values;
	values.reserve(separatedValues.size());

	const std::vector<std::string>& dataTypes = 	_definition.getTupleInstance().getTupleElementDataTypes();
	auto dataType = dataTypes.begin();
	if (separatedValues.size() > dataType->size())
	{
		throw std::exception("Failed to transform string to value list. Number of data type definitions mismatch the number of listed values.");
	}

	for (const std::string& separatedValue : separatedValues)
	{
		ot::Variable value = ot::ExplicitStringValueConverter::setValueFromString(separatedValue, *dataType);
		if (dataType != dataTypes.end())
		{
			dataType++;
		}
		values.push_back(value);

		if (values.empty()) 
		{
			throw std::invalid_argument("Value in value list is empty.");
		}
	}

	return values;
}

std::string AdvancedQueryBuilder::createFieldName(const ot::ValueComparisonDescription& _definition)
{
	std::string fieldName = _definition.getName();
	bool isTuple = !_definition.getTupleInstance().isSingle();

	if (isTuple && !queryTargetsEntireTuple(_definition))
	{
		//Better here a converter that transforms the tuple into an array and also does the number transformations if indicated by the valueDescription.
		ot::TupleDescription* tupleDescription = TupleFactory::create(_definition.getTupleInstance().getTupleTypeName());
		ot::TupleDescriptionComplex* complexTupleDescription = dynamic_cast<ot::TupleDescriptionComplex*>(tupleDescription);
		assert(complexTupleDescription != nullptr);
		const std::string targetElement = _definition.getTupleTarget();

		auto tupleElementNames = complexTupleDescription->getTupleElementNames(_definition.getTupleInstance().getTupleFormatName());
		auto pos = std::find(tupleElementNames.begin(), tupleElementNames.end(), targetElement) - tupleElementNames.begin();

		fieldName = fieldName + "." + std::to_string(pos);
	}

	//Additionally we would need to check fo the matrix target here.

	return fieldName;
}

BsonViewOrValue AdvancedQueryBuilder::createComparison(const std::string _fieldName, const std::list<std::string>& _comparators, std::list<ot::Variable> _values)
{

	BsonViewOrValue comparison;
	if (_comparators.size() > _values.size())
	{
		assert(false);
		throw std::exception("Not enough values defined for query.");
	}
	else if (_comparators.size() == 1 && _values.size() > 1)
	{
		// Here we have an array compare (Tuple, in or not in). Guard against invalid operators on this field is part of extractMongoComparators
		comparison = GenerateFilterQuery(_comparators.front(), _values);
	}
	else
	{
		if (_comparators.size() != _values.size())
		{
			assert(false);
			throw std::exception("Failed to create comparison.");
		}
		std::list< BsonViewOrValue> valueComparisons;
		auto value = _values.begin();
		for (const std::string& comparator : _comparators)
		{
			auto singleComparison = GenerateFilterQuery(comparator, *value);
			valueComparisons.push_back(singleComparison);
			if (value != _values.end())
			{
				value++;
			}
		}

		if (valueComparisons.size() > 1)
		{
			comparison = connectWithAND(std::move(valueComparisons));
		}
		else if (valueComparisons.size() == 0)
		{
			assert(false);
			throw std::exception("No comparison to create");
		}
		else
		{

			comparison = valueComparisons.front();
		}
	}

	//Now we add the field name to the comparision, forming the final query
	BsonViewOrValue finalQuery = GenerateFilterQuery(_fieldName, std::move(comparison));

#ifdef _DEBUG
	std::string queryString = bsoncxx::to_json(finalQuery.view());
#endif
	return finalQuery;
}

BsonViewOrValue AdvancedQueryBuilder::createComparison(const ot::ValueComparisonDescription& _decription)
{
	ot::ValueComparisonDescription valueComparisonCleaned = _decription;
	std::list<std::string> mongoDBComparators = extractMongoComparators(valueComparisonCleaned);
	std::vector<ot::Variable> values = getListOfValuesFromString(valueComparisonCleaned);
	const std::string fieldName = createFieldName(valueComparisonCleaned);
	return createComparison(fieldName, mongoDBComparators, { values.begin(),values.end() });
}

BsonViewOrValue AdvancedQueryBuilder::connectWithAND(std::list<BsonViewOrValue>&& values)
{
	const std::string mongoOperator = "$and";
	return BuildBsonArray(mongoOperator, std::move(values));
}

BsonViewOrValue AdvancedQueryBuilder::connectWithOR(std::list<BsonViewOrValue>&& values)
{
	const std::string mongoOperator = "$or";
	return BuildBsonArray(mongoOperator, std::move(values));
}

std::list<ot::Variable> AdvancedQueryBuilder::getVariableListFromValue(const ot::ValueComparisonDescription& _definition)
{
	std::string valueStr = _definition.getValue();
	const std::string& type = _definition.getTupleInstance().getTupleElementDataTypes().front();
	valueStr.erase(std::remove(valueStr.begin(), valueStr.end(), ' '), valueStr.end());

	// Determine delimiter
	char delimiter = ',';
	if (valueStr.find(';') != std::string::npos) {
		delimiter = ';';
	}

	// Get values
	std::list<std::string> valueStrings = ot::String::split(valueStr, delimiter);
	std::list<ot::Variable> values;

	for (const std::string& valueStr : valueStrings) {
		ot::Variable value = ot::ExplicitStringValueConverter::setValueFromString(valueStr, _definition.getTupleInstance().getTupleElementDataTypes().front());
		values.push_back(value);
	}
	if (values.empty()) {
		throw std::invalid_argument("Query for contains incorrect. No values provided.");
	}

	return values;
}

bool AdvancedQueryBuilder::queryTargetsEntireTuple(const ot::ValueComparisonDescription& _definition)
{
	return _definition.getTupleTarget() == _definition.getTupleInstance().getTupleTypeName();
}

void AdvancedQueryBuilder::noTupleAllowedCheck(const ot::ValueComparisonDescription& _definition)
{
	if (!_definition.getTupleInstance().isSingle())
	{
		throw std::invalid_argument("The selected comparision does not support tuple values.");
	}
}
