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
