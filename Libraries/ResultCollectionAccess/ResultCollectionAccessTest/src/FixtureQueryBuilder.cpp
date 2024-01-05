#include "FixtureQueryBuilder.h"


FixtureQueryBuilder::FixtureQueryBuilder()
	:_dbAccess("QueryTests")
{
}

FixtureQueryBuilder::~FixtureQueryBuilder()
{
}

DataStorageAPI::DataStorageResponse FixtureQueryBuilder::ExecuteQuery(BsonViewOrValue& query, BsonViewOrValue& select)
{
	const auto documents = _dbAccess.GetAllDocuments(query,select, 0);
	return documents;
}
