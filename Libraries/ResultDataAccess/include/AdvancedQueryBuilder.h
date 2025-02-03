#pragma once
#include <string>
#include <list>
#include "OTCore/Variable.h"
#include "Helper/QueryBuilder.h"

class __declspec(dllexport) AdvancedQueryBuilder : public DataStorageAPI::QueryBuilder
{
public:
	static const std::list<std::string>& getComparators() { return m_comparators; }
	static std::string getAnyOfComparator() { return m_anyOfComparator; }
	static std::string getNoneOfComparator() { return m_noneOfComparator; }
	
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
	inline static const std::string m_anyOfComparator = "any of";
	inline static const std::string m_noneOfComparator = "not any of";
	inline static const std::list<std::string> m_comparators = { "<", "<=", "=", ">", ">=", "!=", m_anyOfComparator, m_noneOfComparator };;
	inline static const std::map<std::string, std::string> m_mongoDBComparators = { {"<","$lt"},{"<=","$lte"},{">=","$gte"},{">","$gt"},{"=","$eq"}, {"!=", "$ne"}, {m_anyOfComparator,"$in"}, {m_noneOfComparator,"$nin"} };
};
