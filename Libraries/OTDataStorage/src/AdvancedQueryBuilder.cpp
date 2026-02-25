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
#include "OTCore/VariableToBSONStringConverter.h"
#include "OTCore/ExplicitStringValueConverter.h"
#include "OTDataStorage/AdvancedQueryBuilder.h"
#include "OTCore/ComplexNumbers/ComplexNumberConversion.h"
#include "OTCore/ComplexNumbers/ComplexNumberFormat.h"
#include "OTCore/Tuple/TupleDescriptionComplex.h"
#include "OTCore/Tuple/TupleFactory.h"
// std header
#include <cassert>
#include <stdarg.h>
#include "bsoncxx/json.hpp"

BsonViewOrValue AdvancedQueryBuilder::createComparison(const ot::ValueComparisonDefinition& _valueComparison)
{
	const std::string comparator = _valueComparison.getComparator();
	BsonViewOrValue comparison;
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
			const TupleInstance& tupleInstanceQuery = _valueComparison.getQueryTupleDescription();
			if (!tupleInstanceQuery.isSingle())
			{
				//Better here a converter that transforms the tuple into an array and also does the number transformations if indicated by the valueDescription.
				TupleDescription* tupleDescription = TupleFactory::create(tupleInstanceQuery.getTupleTypeName());
				TupleDescriptionComplex* complexTupleDescription = dynamic_cast<TupleDescriptionComplex*>(tupleDescription);
				assert(complexTupleDescription != nullptr);
				
				const TupleInstance& tupleInstanceStorage = _valueComparison.getStoredTupleDescription();
				std::list<ot::Variable> queryValues = getListOfValuesFromString(_valueComparison.getValue(), tupleInstanceQuery.getTupleElementDataTypes());
				const std::string& targetElement = _valueComparison.getTupleTargetElement();
				//Either someone enters two values as an array to query (either full complex number or only one of the two parts), or a single value shall be queried.
				if (queryValues.size() == 2)
				{
					// Process: 
					// 0) If formats match, don't transform
					// 1) Transform query into the internal std::complex format. Base is the value string that was entered in the property.
					// 2) Transform the complex value to match the storage format.
					std::vector<double> transformedQueryValues(2);
					if (tupleInstanceQuery.getTupleFormatName() == tupleInstanceStorage.getTupleFormatName() && tupleInstanceQuery.getTupleUnits() == tupleInstanceStorage.getTupleUnits())
					{
						//0)
						assert(queryValues.front().isDouble() && queryValues.back().isDouble());

						transformedQueryValues[0] = queryValues.front().getDouble();
						transformedQueryValues[1] = queryValues.back().getDouble();
					}
					else
					{
						//1) Currently ignores the necessary dB backtransformation
						const std::string potentialAngleUnit = tupleInstanceQuery.getTupleUnits().back();
						std::complex<double> complexNumber = ot::ComplexNumberConversion::fromString(_valueComparison.getValue(), ot::ComplexNumbers::getFormatFromString(tupleInstanceQuery.getTupleFormatName()),potentialAngleUnit);

						//2)
						const std::string storedFormat = tupleInstanceStorage.getTupleFormatName();
						const std::vector<std::string>& tupleUnits = tupleInstanceStorage.getTupleUnits();

						const std::string& targetElement = _valueComparison.getTupleTargetElement();
						if (storedFormat == ot::ComplexNumbers::getFormatString(ot::ComplexNumberFormat::Polar))
						{
							ot::ComplexNumberDefinition cplxPolarFormat = ot::ComplexNumberConversion::cartesianToPolar(complexNumber);
							transformedQueryValues[0] = cplxPolarFormat.m_firstValue;
							transformedQueryValues[1] = cplxPolarFormat.m_secondValue;
						}
						else
						{
							transformedQueryValues[0] = complexNumber.real();
							transformedQueryValues[1] = complexNumber.imag();
						}
						//Now build the query depending on the target tuple element.
						if (targetElement == tupleInstanceStorage.getTupleTypeName())
						{
							//Here we query the entire array of values. only equality comparator allowed.
							assert(comparator == "=");
							comparison = GenerateFilterQuery(mongoComparator->second, { transformedQueryValues[0] , transformedQueryValues[1] });
						}
						else
						{
							auto tupleElementNames = complexTupleDescription->getTupleElementNames(tupleInstanceStorage.getTupleFormatName());
							auto pos = std::find(tupleElementNames.begin(), tupleElementNames.end(), targetElement) - tupleElementNames.begin();
					
							//Here we need a query of the structure: {value : { $elemMatch:{"0":{$gt : 0}}}
							//This would query for values > 0 in the first entry of the value array. E.g. real value > 0.
							BsonViewOrValue compareWithValue = GenerateFilterQuery(mongoComparator->second, transformedQueryValues[pos]);
							BsonViewOrValue equalIndex = GenerateFilterQuery(std::to_string(pos), compareWithValue.view());
							comparison = GenerateFilterQuery("$elemMatch", equalIndex.view());
						}
					}
				}
				else if (queryValues.size() == 1)
				{
					assert(queryValues.front().isDouble());
					double queryValue = queryValues.front().getDouble();
					if (targetElement == tupleInstanceStorage.getTupleTypeName())
					{
						throw std::exception(("A query for a complex number requires both values specified and separated by a \""+m_listSplitToken+"\".").c_str());
					}
					else
					{
						auto tupleElementNames = complexTupleDescription->getTupleElementNames(tupleInstanceStorage.getTupleFormatName());
						auto pos = std::find(tupleElementNames.begin(), tupleElementNames.end(), targetElement) - tupleElementNames.begin();

						//Here we need a query of the structure: {value : { $elemMatch:{"0":{$gt : 0}}}
						//This would query for values > 0 in the first entry of the value array. E.g. real value > 0.
						BsonViewOrValue compareWithValue = GenerateFilterQuery(mongoComparator->second, queryValue);
						BsonViewOrValue equalIndex = GenerateFilterQuery(std::to_string(pos), compareWithValue.view());
						comparison = GenerateFilterQuery("$elemMatch", equalIndex.view());
					}
				}
				else
				{
					throw std::exception("Querying a complex number requires either two values or a single value.");
				}
			}
			else
			{
				ot::Variable value = ot::ExplicitStringValueConverter::setValueFromString(_valueComparison.getValue(), _valueComparison.getType());
				comparison = GenerateFilterQuery(mongoComparator->second, value);
			}
		}
	}
	
	//Now we add the field name to the comparision, forming the final query
	BsonViewOrValue finalQuery = GenerateFilterQuery(_valueComparison.getName(), std::move(comparison));

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

BsonViewOrValue AdvancedQueryBuilder::buildRangeQuery(const ot::ValueComparisonDefinition& _definition)
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

std::list<ot::Variable> AdvancedQueryBuilder::getListOfValuesFromString(const std::string& _allValues, const std::vector<std::string>& _dataTypes)
{
	std::list<std::string> separatedValues =	ot::String::split(_allValues, m_listSplitToken);
	std::list<ot::Variable> values;
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


std::list<ot::Variable> AdvancedQueryBuilder::getVariableListFromValue(const ot::ValueComparisonDefinition& _definition)
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

void AdvancedQueryBuilder::noTupleAllowedCheck(const ot::ValueComparisonDefinition& _definition)
{
	if (!_definition.getStoredTupleDescription().isSingle())
	{
		throw std::invalid_argument("The selected comparision does not support tuple values.");
	}
}


BsonViewOrValue AdvancedQueryBuilder::createComparisionEqualNoneOf(const std::list<ot::Variable>& values)
{
	const std::string mongoComparator = "$nin";
	return GenerateFilterQuery(mongoComparator, values);
}

