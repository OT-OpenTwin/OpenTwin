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

#include "stdafx.h"
#include "AdvancedQueryBuilder.h"
#include "OTCore/VariableToBSONStringConverter.h"
#include "OTCore/ExplicitStringValueConverter.h"
#include <cassert>
#include <stdarg.h>
#include "OTCore/String.h"

BsonViewOrValue AdvancedQueryBuilder::createComparison(const ValueComparisionDefinition& _valueComparision)
{
	const std::string comparator = _valueComparision.getComparator();
	if (comparator == ot::ComparisionSymbols::g_rangeComparator)
	{
		return buildRangeQuery(_valueComparision);
	}

	auto mongoComparator = m_mongoDBComparators.find(comparator);
	if (mongoComparator == m_mongoDBComparators.end())
	{
		assert(0);
		throw std::exception("Not supported comparator selected for comparison query.");
	}
	else if (comparator == ot::ComparisionSymbols::g_anyOneOfComparator)
	{
		std::list<ot::Variable> values = getVariableListFromValue(_valueComparision);
		auto compare = createComparisionEqualToAnyOf(values);
		return GenerateFilterQuery(_valueComparision.getName(), std::move(compare));
	}
	else if (comparator == ot::ComparisionSymbols::g_noneOfComparator)
	{
		std::list<ot::Variable> values = getVariableListFromValue(_valueComparision);
		auto compare = createComparisionEqualNoneOf(values);
		return GenerateFilterQuery(_valueComparision.getName(), std::move(compare));
	}
	else
	{
		ot::Variable value = ot::ExplicitStringValueConverter::setValueFromString(_valueComparision.getValue(), _valueComparision.getType());
		auto comparision = GenerateFilterQuery(mongoComparator->second, value);
		return GenerateFilterQuery(_valueComparision.getName(), std::move(comparision));
	}
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

BsonViewOrValue AdvancedQueryBuilder::buildRangeQuery(const ValueComparisionDefinition& _definition)
{
	const std::string& name = _definition.getName();
	std::string valueStr = _definition.getValue();
	const std::string& type = _definition.getType();

	valueStr.erase(std::remove(valueStr.begin(), valueStr.end(), ' '), valueStr.end());
	char openingBracket = valueStr[0];
	char closingBracket = valueStr[valueStr.size() - 1];
	valueStr.erase(valueStr.begin());
	valueStr.erase(valueStr.end() - 1);
	size_t posDelimiter = valueStr.find(",");
	const bool notationIsCorrect = (openingBracket == '(' || openingBracket == '[') && (closingBracket == ')' || closingBracket == ']') && posDelimiter != std::string::npos;

	if (notationIsCorrect) {
		const bool dataTypeCompatible = type == ot::TypeNames::getInt32TypeName() || type == ot::TypeNames::getInt64TypeName() || type == ot::TypeNames::getDoubleTypeName() || type == ot::TypeNames::getFloatTypeName();
		if (dataTypeCompatible) {
			const std::string firstValue = valueStr.substr(0, posDelimiter);
			const std::string secondValue = valueStr.substr(posDelimiter + 1);

			ot::Variable vFirstValue = ot::ExplicitStringValueConverter::setValueFromString(firstValue, type);
			ot::Variable vSecondValue = ot::ExplicitStringValueConverter::setValueFromString(secondValue, type);

			std::string correspondingComparator;
			if (openingBracket == '(') {
				correspondingComparator = ">";
			}
			else {
				correspondingComparator = ">=";
			}
			auto mongoComparator = m_mongoDBComparators.find(correspondingComparator);
			assert(mongoComparator != m_mongoDBComparators.end());
			auto firstCompare = GenerateFilterQuery(mongoComparator->second, vFirstValue);
			auto firstCompareQuery = GenerateFilterQuery(name, std::move(firstCompare));

			if (closingBracket == ')') {
				correspondingComparator = "<";
			}
			else {
				correspondingComparator = "<=";
			}
			mongoComparator = m_mongoDBComparators.find(correspondingComparator);
			assert(mongoComparator != m_mongoDBComparators.end());
			auto secondCompare = GenerateFilterQuery(mongoComparator->second, vSecondValue);
			auto secondCompareQuery = GenerateFilterQuery(name, std::move(secondCompare));

			return connectWithAND({ firstCompareQuery,secondCompareQuery });
		}
		else {
			throw std::invalid_argument("Query for interval incorrect. The datatype of the selected field holds no numerical value.");
		}
	}
	else {
		throw std::invalid_argument("Query for interval incorrect. The interval has to follow the english notation, e.g.: (2,3.5].");
	}
}

std::list<ot::Variable> AdvancedQueryBuilder::getVariableListFromValue(const ValueComparisionDefinition& _definition)
{
	std::string valueStr = _definition.getValue();
	const std::string& type = _definition.getType();
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
		ot::Variable value = ot::ExplicitStringValueConverter::setValueFromString(valueStr, _definition.getType());
		values.push_back(value);
	}
	if (values.empty()) {
		throw std::invalid_argument("Query for contains incorrect. No values provided.");
	}

	return values;
}

BsonViewOrValue AdvancedQueryBuilder::createComparisionEqualToAnyOf(const std::list<ot::Variable>& values)
{
	const std::string mongoComparator = "$in";
	return GenerateFilterQuery(mongoComparator, values);
}


BsonViewOrValue AdvancedQueryBuilder::createComparisionEqualNoneOf(const std::list<ot::Variable>& values)
{
	const std::string mongoComparator = "$nin";
	return GenerateFilterQuery(mongoComparator, values);
}

