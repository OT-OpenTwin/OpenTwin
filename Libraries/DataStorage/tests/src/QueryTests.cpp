// @otlicense
// File: QueryTests.cpp
// 
// License:
// Copyright 2025 by OpenTwin
//  
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
//  
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// @otlicense-end

#include <iostream>
#include "gtest/gtest.h"

#include "Unittest/UnittestDocumentAccess.h"
#include "Helper/QueryBuilder.h"
#include "OTCore/JSON.h"

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

	std::vector<std::string> filter{"Quantity"};
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

	std::vector<std::string> filter{"Quantity"};
	auto select = queryBuilder.GenerateSelectQuery(filter, true);

	auto response = dbAccess.GetAllDocuments(query, select, 0);
	auto allEntries = response.getResult();
	ot::JsonDocument doc;
	doc.fromJson(allEntries);
	auto allDocuments =	doc["Documents"].GetArray();
	auto numberOfDocuments = allDocuments.Size();

	EXPECT_EQ(numberOfDocuments, expectedNumberOfDocuments);
}