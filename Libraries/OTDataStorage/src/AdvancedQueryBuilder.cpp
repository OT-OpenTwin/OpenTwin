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

BsonViewOrValue AdvancedQueryBuilder::createComparison(const ot::ValueComparisonDescription& _valueComparison)
{
	const std::string comparator = _valueComparison.getComparator();
	BsonViewOrValue comparison;
	std::string fieldName = _valueComparison.getName();
	// The range operator is an own implementatation
	if (comparator == ot::ComparisonSymbols::g_rangeComparator)
	{
		noTupleAllowedCheck(_valueComparison);
		comparison = buildRangeQuery(_valueComparison); //Muss umgebaut werden, damit der Name nicht mehr mit Teil der range query ist.
	}
	else
	{
		//Now we translate the string comparator, displayed in the ui, into the corresponding mongo comparator
		auto mongoComparator = m_mongoDBComparators.find(comparator);
		if (mongoComparator == m_mongoDBComparators.end())
		{
			assert(0);
			throw std::exception("Not supported comparator selected for comparison query.");
		}
		else if (comparator == ot::ComparisonSymbols::g_anyOneOfComparator)
		{
			noTupleAllowedCheck(_valueComparison);
			std::list<ot::Variable> values = getVariableListFromValue(_valueComparison);
			comparison = createComparisionEqualToAnyOf(values);
			
		}
		else if (comparator == ot::ComparisonSymbols::g_noneOfComparator)
		{
			noTupleAllowedCheck(_valueComparison);
			std::list<ot::Variable> values = getVariableListFromValue(_valueComparison);
			auto compare = createComparisionEqualNoneOf(values);
			comparison = GenerateFilterQuery(_valueComparison.getName(), std::move(compare));
		}
		else
		{
			//In case of tuple values, the query needs to be assembled differently
			const TupleInstance& tupleInstanceQuery = _valueComparison.getTupleInstance();
			if (!tupleInstanceQuery.isSingle())
			{
				//Better here a converter that transforms the tuple into an array and also does the number transformations if indicated by the valueDescription.
				TupleDescription* tupleDescription = TupleFactory::create(tupleInstanceQuery.getTupleTypeName());
				TupleDescriptionComplex* complexTupleDescription = dynamic_cast<TupleDescriptionComplex*>(tupleDescription);
				assert(complexTupleDescription != nullptr);
				
				std::vector<ot::Variable> queryValues = getListOfValuesFromString(_valueComparison.getValue(), tupleInstanceQuery.getTupleElementDataTypes());
				const std::string& targetElement = _valueComparison.getTupleTarget();
				assert(queryValues.front().isDouble() && queryValues.back().isDouble());
				
				//Either someone enters two values as an array to query (either full complex number or only one of the two parts), or a single value shall be queried.
				if (queryValues.size() == 2)
				{
					//Now build the query depending on the target tuple element.
					if (targetElement == tupleDescription->getName())
					{
						//Here we query the entire array of values. only equality comparator allowed.
						assert(comparator == "=");
						comparison = GenerateFilterQuery(mongoComparator->second, { queryValues[0].getDouble() , queryValues[1].getDouble()});
					}
					else
					{
						auto tupleElementNames = complexTupleDescription->getTupleElementNames(tupleInstanceQuery.getTupleFormatName());
						auto pos = std::find(tupleElementNames.begin(), tupleElementNames.end(), targetElement) - tupleElementNames.begin();
					
						//Here we need a query of the structure: {value : { $elemMatch:{"0":{$gt : 0}}}
						//This would query for values > 0 in the first entry of the value array. E.g. real value > 0.
						BsonViewOrValue compareWithValue = GenerateFilterQuery(mongoComparator->second, queryValues[pos].getDouble());
						BsonViewOrValue equalIndex = GenerateFilterQuery(std::to_string(pos), compareWithValue.view());
						comparison = GenerateFilterQuery("$elemMatch", equalIndex.view());
					}
				}
				else if (queryValues.size() == 1)
				{
					assert(queryValues.front().isDouble());
					double queryValue = queryValues.front().getDouble();
					if (targetElement == tupleInstanceQuery.getTupleTypeName())
					{
						throw std::exception(("A query for a complex number requires both values specified and separated by a \""+m_listSplitToken+"\".").c_str());
					}
					else
					{
						auto tupleElementNames = complexTupleDescription->getTupleElementNames(tupleInstanceQuery.getTupleFormatName());
						auto pos = std::find(tupleElementNames.begin(), tupleElementNames.end(), targetElement) - tupleElementNames.begin();
						fieldName = fieldName + "." + std::to_string(pos);
						//Here we need a query of the structure: {value : { $elemMatch:{"0":{$gt : 0}}}
						//This would query for values > 0 in the first entry of the value array. E.g. real value > 0.
						comparison = GenerateFilterQuery(mongoComparator->second, queryValue);
					}
				}
				else
				{
					throw std::exception("Querying a complex number requires either two values or a single value.");
				}
			}
			else
			{
				const auto& tupleInstance = _valueComparison.getTupleInstance();
				ot::Variable value = ot::ExplicitStringValueConverter::setValueFromString(_valueComparison.getValue(), tupleInstance.getTupleElementDataTypes().front());
				comparison = GenerateFilterQuery(mongoComparator->second, value);
			}
		}
	}
	
	//Now we add the field name to the comparision, forming the final query
	BsonViewOrValue finalQuery = GenerateFilterQuery(fieldName, std::move(comparison));

#ifdef _DEBUG
	std::string queryString = bsoncxx::to_json(finalQuery.view());
#endif
	return finalQuery;
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

BsonViewOrValue AdvancedQueryBuilder::buildRangeQuery(const ot::ValueComparisonDescription& _definition)
{
	const std::string& name = _definition.getName();
	std::string valueStr = _definition.getValue();
	const std::string& type = _definition.getTupleInstance().getTupleElementDataTypes().front();

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

std::vector<ot::Variable> AdvancedQueryBuilder::getListOfValuesFromString(const std::string& _allValues, const std::vector<std::string>& _dataTypes)
{
	std::list<std::string> separatedValues = ot::String::split(_allValues, m_listSplitToken);
	std::vector<ot::Variable> values;
	values.reserve(separatedValues.size());

	auto dataType = _dataTypes.begin();
	if (separatedValues.size() > _dataTypes.size())
	{
		throw std::exception("Failed to transform string to value list. Number of data type definitions missmatch the number of listed values.");
	}

	for (const std::string& separatedValue : separatedValues)
	{
		ot::Variable value = ot::ExplicitStringValueConverter::setValueFromString(separatedValue, *dataType);
		if (dataType != _dataTypes.end())
		{
			dataType++;
		}
		values.push_back(value);

		if (values.empty()) {
			throw std::invalid_argument("Value in value list is empty.");
		}
	}

	return values;
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

BsonViewOrValue AdvancedQueryBuilder::createComparisionEqualToAnyOf(const std::list<ot::Variable>& values)
{
	const std::string mongoComparator = "$in";
	return GenerateFilterQuery(mongoComparator, values);
}

void AdvancedQueryBuilder::noTupleAllowedCheck(const ot::ValueComparisonDescription& _definition)
{
	if (!_definition.getTupleInstance().isSingle())
	{
		throw std::invalid_argument("The selected comparision does not support tuple values.");
	}
}


BsonViewOrValue AdvancedQueryBuilder::createComparisionEqualNoneOf(const std::list<ot::Variable>& values)
{
	const std::string mongoComparator = "$nin";
	return GenerateFilterQuery(mongoComparator, values);
}

