#pragma once
#include <gtest/gtest.h>
#include "Unittest/UnittestDocumentAccess.h"
#include "Helper\UniqueFileName.h"
#include "Helper\QueryBuilder.h"
#include "Helper\BsonValuesHelper.h"

class FixtureQueryBuilder : public testing::Test
{
public:
	FixtureQueryBuilder();
	~FixtureQueryBuilder();
	DataStorageAPI::DataStorageResponse ExecuteQuery(BsonViewOrValue& query, BsonViewOrValue& select);

private:
	
	const std::string _collectionName = "QueryTests";
	UnittestDocumentAccess _dbAccess;
};
