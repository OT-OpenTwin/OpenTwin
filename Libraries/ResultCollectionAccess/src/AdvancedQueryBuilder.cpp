#include "AdvancedQueryBuilder.h"
#include "OTCore/VariableToBSONStringConverter.h"
#include <cassert>

bsoncxx::document::view_or_value AdvancedQueryBuilder::CreateComparision(const std::string& comparator, const ot::Variable& variable)
{
	//auto mongoComparator = _mongoDBComparators.find(comparator);
	//if (mongoComparator == _mongoDBComparators.end() || mongoComparator->second == "$in" || mongoComparator->second == "$nin")
	//{
	//	assert(0);
	//	throw std::exception("Not supported comparator selected for comparision query.");
	//}
	//else
	//{
	//	return GenerateFilterQuery(mongoComparator->second, variable);
	return GenerateFilterQuery(comparator, variable);
	// 
	//}
}

//


//DataStorageAPI::BsonViewOrValue AdvancedQueryBuilder::CreateComparisionEqualToAnyOf(const std::list<ot::Variable>& values)
//{
//	const std::string mongoComparator = "$in";
//	return GenerateFilterQuery(mongoComparator, values);
//}
//
//
//DataStorageAPI::BsonViewOrValue AdvancedQueryBuilder::CreateComparisionEqualNoneOf(const std::list<ot::Variable>& values)
//{
//	const std::string mongoComparator = "$nin";
//	return GenerateFilterQuery(mongoComparator, values);
//}