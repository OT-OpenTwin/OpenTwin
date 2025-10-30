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
#include "OTCore/Variable.h"
#include "Helper/QueryBuilder.h"
#include "OTCore/ComparisionSymbols.h"
#include "OTCore/ValueComparisionDefinition.h"
class __declspec(dllexport) AdvancedQueryBuilder : public DataStorageAPI::QueryBuilder
{
public:
	BsonViewOrValue createComparison(const ValueComparisionDefinition& _valueComparision);

	BsonViewOrValue connectWithAND(std::list<BsonViewOrValue>&& values);
	BsonViewOrValue connectWithOR(std::list<BsonViewOrValue>&& values);

private:
	inline static const std::map<std::string, std::string> m_mongoDBComparators = { {"<","$lt"},{"<=","$lte"},{">=","$gte"},{">","$gt"},{"=","$eq"}, {"!=", "$ne"}, {ot::ComparisionSymbols::g_anyOneOfComparator,"$in"}, {ot::ComparisionSymbols::g_noneOfComparator,"$nin"} };
	BsonViewOrValue buildRangeQuery(const ValueComparisionDefinition& _definition);
	
	std::list<ot::Variable> getVariableListFromValue(const ValueComparisionDefinition& _definition);
	
	BsonViewOrValue createComparisionEqualNoneOf(const std::list<ot::Variable>& values);
	BsonViewOrValue createComparisionEqualToAnyOf(const std::list<ot::Variable>& values);
};
