#include "AdvancedQueryBuilder.h"
#include "OTCore/VariableToBSONStringConverter.h"
#include <cassert>
#include <stdarg.h>

BsonViewOrValue AdvancedQueryBuilder::CreateComparison(const std::string& comparator, const ot::Variable& variable)
{
	auto mongoComparator = _mongoDBComparators.find(comparator);
	if (mongoComparator == _mongoDBComparators.end() || mongoComparator->second == "$in" || mongoComparator->second == "$nin")
	{
		assert(0);
		throw std::exception("Not supported comparator selected for comparison query.");
	}
	else
	{
		return GenerateFilterQuery(mongoComparator->second, variable);
	 
	}
}

BsonViewOrValue AdvancedQueryBuilder::ConnectWithAND (std::list<BsonViewOrValue>&& values)
{
	const std::string mongoOperator = "$and";
	return BuildBsonArray(mongoOperator, std::move(values));
}

BsonViewOrValue AdvancedQueryBuilder::ConnectWithOR(std::list<BsonViewOrValue>&& values)
{
	const std::string mongoOperator = "$or";
	return BuildBsonArray(mongoOperator, std::move(values));
}


BsonViewOrValue AdvancedQueryBuilder::CreateComparisionEqualToAnyOf(const std::list<ot::Variable>& values)
{
	const std::string mongoComparator = "$in";
	return GenerateFilterQuery(mongoComparator, values);
}


BsonViewOrValue AdvancedQueryBuilder::CreateComparisionEqualNoneOf(const std::list<ot::Variable>& values)
{
	const std::string mongoComparator = "$nin";
	return GenerateFilterQuery(mongoComparator, values);
}

