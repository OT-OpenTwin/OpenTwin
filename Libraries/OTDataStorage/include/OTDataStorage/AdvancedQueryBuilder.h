// @otlicense
// File: AdvancedQueryBuilder.h
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

#pragma once
#include <string>
#include <list>
#include "OTCore/Variable/Variable.h"
#include "OTDataStorage/Helper/QueryBuilder.h"
#include "OTCore/ComparisonSymbols.h"
#include "OTCore/QueryDescription/ValueComparisonDescription.h"

class __declspec(dllexport) AdvancedQueryBuilder : public DataStorageAPI::QueryBuilder
{
public:

	//! @brief Finds MongoDB equivalents for the comparators.
	//! Checks if valid comparator for data type and tuple configuration
	//! @param _definition In case of a range comparison, the brackets are removed from the value
	//! @return list will have size 2 if comparator is range, otherwise size 1
	std::list<std::string> extractMongoComparators(ot::ValueComparisonDescription& _definition);
	
	//! @brief Extracts values (possible a list) from the string value and performs a type cast depending on the noted datatype
	//! Checks if number of values fit the tuple target configuration
	std::vector<ot::Variable> getListOfValuesFromString(const ot::ValueComparisonDescription& _definition);
	
	//! @brief Will consider tuple and matrix structure as well as target definitions
	std::string createFieldName(const ot::ValueComparisonDescription& _definition);
	
	//! @brief 
	//! 1) ExtractMongoComparators (clean value string)
	//! 2) Extract ot::Variables from value string (externally conduct unit-informed transformations if necessary)
	//! 3) Create field name 
	//! Checks if comparartors are <= and number of variables. In case of 2 values and 1 comparator a query for an array is conducted (must be equal sign)
	BsonViewOrValue createComparison(const std::string _fieldName,const std::list<std::string>& _comparators, std::list<ot::Variable> _values);
	
	
	BsonViewOrValue createComparison(const ot::ValueComparisonDescription& _decription);

	BsonViewOrValue connectWithAND(std::list<BsonViewOrValue>&& values);
	BsonViewOrValue connectWithOR(std::list<BsonViewOrValue>&& values);

private:
	inline static const std::map<std::string, std::string> m_mongoDBComparators = { {"<","$lt"},{"<=","$lte"},{">=","$gte"},{">","$gt"},{"=","$eq"}, {"!=", "$ne"}, {ot::ComparisonSymbols::g_anyOneOfComparator,"$in"}, {ot::ComparisonSymbols::g_noneOfComparator,"$nin"} };
	const std::string m_listSplitToken = ",";
	
	std::list<ot::Variable> getVariableListFromValue(const ot::ValueComparisonDescription& _definition);
	
	bool queryTargetsEntireTuple(const ot::ValueComparisonDescription& _definition);

	void noTupleAllowedCheck(const ot::ValueComparisonDescription& _definition);
};
