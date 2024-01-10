#pragma once
#include <string>
#include <list>
#include "OTCore/Variable.h"
#include "Helper/QueryBuilder.h"

class __declspec(dllexport) AdvancedQueryBuilder : public DataStorageAPI::QueryBuilder
{
public:
	static const std::list<std::string>& getComparators() { return _comparators; }
	static std::string getAnyOfComparator() { return _anyOfComparator; }
	static std::string getNoneOfComparator() { return _noneOfComparator; }

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
	inline static const std::string _anyOfComparator = "not any of";
	inline static const std::string _noneOfComparator = "any of";
	inline static const std::list<std::string> _comparators = { "<", "<=", "=", ">", ">=", "!=", _anyOfComparator, _noneOfComparator };;
	inline static const std::map<std::string, std::string> _mongoDBComparators = { {"<","$lt"},{"<=","$lte"},{">=","$gte"},{">","$gt"},{"=","$eq"}, {"!=", "$ne"}, {_anyOfComparator,"$in"}, {_noneOfComparator,"$nin"} };;

};
