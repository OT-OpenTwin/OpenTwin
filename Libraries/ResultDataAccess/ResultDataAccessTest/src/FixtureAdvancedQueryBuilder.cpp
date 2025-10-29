// @otlicense

#include "FixtureAdvancedQueryBuilder.h"


FixtureAdvancedQueryBuilder::FixtureAdvancedQueryBuilder()
	:_dbAccess("QueryTests")
{
}

FixtureAdvancedQueryBuilder::~FixtureAdvancedQueryBuilder()
{
}

DataStorageAPI::DataStorageResponse FixtureAdvancedQueryBuilder::ExecuteQuery(BsonViewOrValue& query, BsonViewOrValue& select)
{
	const auto documents = _dbAccess.GetAllDocuments(query,select, 0);
	return documents;
}
