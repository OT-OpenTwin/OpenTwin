#pragma once
#include <string>
#include <list>
#include "OTCore/Variable.h"
#include "Helper/QueryBuilder.h"
#include "OTCore/ComparisionSymbols.h"
class __declspec(dllexport) AdvancedQueryBuilder : public DataStorageAPI::QueryBuilder
{
public:	
	BsonViewOrValue CreateComparison(const std::string& comparator, const ot::Variable& variable);

	/// <summary>
	/// Also returns documents that do not have the queried field.
	/// </summary>
	/// <param name="values"></param>
	/// <returns></returns>
	BsonViewOrValue CreateComparisionEqualNoneOf(const std::list<ot::Variable>& values);
	BsonViewOrValue CreateComparisionEqualToAnyOf(const std::list<ot::Variable>& values);
	BsonViewOrValue ConnectWithAND (std::list<BsonViewOrValue>&& values);
	BsonViewOrValue ConnectWithOR (std::list<BsonViewOrValue>&& values);

private:
	inline static const std::map<std::string, std::string> m_mongoDBComparators = { {"<","$lt"},{"<=","$lte"},{">=","$gte"},{">","$gt"},{"=","$eq"}, {"!=", "$ne"}, {ot::ComparisionSymbols::g_anyOneOfComparator,"$in"}, {ot::ComparisionSymbols::g_noneOfComparator,"$nin"} };
};
