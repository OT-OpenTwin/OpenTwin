#include <iostream>
#include "gtest/gtest.h"

#include "Unittest/UnittestDocumentAccess.h"
#include "Helper/QueryBuilder.h"

using namespace std;

TEST(DBAccess, ConnectWithUnittestDB) {
	EXPECT_NO_THROW(UnittestDocumentAccess dbAccess("QueryTests"));
}

TEST(DBAccess, SimpleQuerySuccess) {
	UnittestDocumentAccess dbAccess("QueryTests");

	const std::string fieldName = "Quantity";
	const int64_t queryValue = 33964739625369600;
	const ot::Variable queryValueGeneric(queryValue);
	
	DataStorageAPI::QueryBuilder queryBuilder;
	auto query = queryBuilder.GenerateFilterQuery(fieldName, queryValueGeneric);

	std::vector<std::string> filter(0);
	auto select = queryBuilder.GenerateSelectQuery(filter, true);
	
	auto response = dbAccess.GetAllDocuments(query, select,1);
	EXPECT_TRUE(response.getSuccess());
}

TEST(DBAccess, SimpleQueryCorrectResult) {
	UnittestDocumentAccess dbAccess("QueryTests");
	const size_t expectedNumberOfDocuments = 2902;

	const std::string fieldName = "Quantity";
	const int64_t queryValue = 33964739625369600;
	const ot::Variable queryValueGeneric(queryValue);

	DataStorageAPI::QueryBuilder queryBuilder;
	auto query = queryBuilder.GenerateFilterQuery(fieldName, queryValueGeneric);

	std::vector<std::string> filter(0);
	auto select = queryBuilder.GenerateSelectQuery(filter, true);

	auto response = dbAccess.GetAllDocuments(query, select, 0);
	auto allEntries = response.getResult();
	ot::JsonDocument doc;
	doc.fromJson(allEntries);
	auto allDocuments =	doc["Documents"].GetArray();
	auto numberOfDocuments = allDocuments.Size();

	EXPECT_EQ(numberOfDocuments, expectedNumberOfDocuments);
}