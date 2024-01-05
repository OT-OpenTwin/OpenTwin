#pragma once
#include <string>
#include <list>
#include "OTCore/Variable.h"
#include "Helper/QueryBuilder.h"

class __declspec(dllexport) AdvancedQueryBuilder : public DataStorageAPI::QueryBuilder
{
public:
	const std::list<std::string>& getComparators() { return _comparators; }

	bsoncxx::document::view_or_value CreateComparision(const std::string& comparator, const ot::Variable& variable);
	//bsoncxx::document::view_or_value ConnectWithAND (bsoncxx::document::view_or_value&, ... );
//	DataStorageAPI::BsonViewOrValue CreateComparisionEqualNoneOf(const std::list<ot::Variable>& values);
//	DataStorageAPI::BsonViewOrValue CreateComparisionEqualToAnyOf(const std::list<ot::Variable>& values);
//	
private:
	const std::list<std::string> _comparators{ "<", "<=", "=", ">", ">=", "!=","any of", "not any of" };
	const std::map<std::string, std::string> _mongoDBComparators{ {"<","$lt"},{"<=","$lte"},{">=","$gte"},{">","$gt"},{"==","$eq"}, {"!=", "$ne"}, {"any of","$in"}, {"not any of","$nin"}};

};
