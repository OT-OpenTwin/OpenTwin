// @otlicense

#pragma once
#include <gtest/gtest.h>
#include "Unittest/UnittestDocumentAccess.h"
#include "Helper\UniqueFileName.h"
#include "Helper\QueryBuilder.h"
#include "Helper\BsonValuesHelper.h"

class FixtureAdvancedQueryBuilder : public testing::Test
{
public:
	FixtureAdvancedQueryBuilder();
	~FixtureAdvancedQueryBuilder();
	DataStorageAPI::DataStorageResponse ExecuteQuery(BsonViewOrValue& query, BsonViewOrValue& select);

private:
	
	const std::string _collectionName = "QueryTests";
	UnittestDocumentAccess _dbAccess;
};
