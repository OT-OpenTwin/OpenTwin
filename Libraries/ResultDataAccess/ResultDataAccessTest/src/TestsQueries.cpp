// @otlicense
// File: TestsQueries.cpp
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

#include <gtest/gtest.h>
#include "FixtureAdvancedQueryBuilder.h"
#include "OTCore/JSON.h"
#include "AdvancedQueryBuilder.h"

TEST_F(FixtureAdvancedQueryBuilder, ComparisionGreaterThan)
{
	const int expectedNumberOfDocuments = 33;
	const std::string fieldName = "Values";
	AdvancedQueryBuilder builder;
	ot::Variable value(0);
	std::string comparator = ">";
	auto comparision = builder.CreateComparison(comparator, value);
	auto query = builder.GenerateFilterQuery(fieldName, std::move(comparision));

	std::vector<std::string> filter{fieldName};
	auto select = builder.GenerateSelectQuery(filter, true);
	auto response = ExecuteQuery(query, select);

	ot::JsonDocument doc;
	doc.fromJson(response.getResult());
	auto allEntries = doc["Documents"].GetArray();
	auto numberOfDocuments = allEntries.Size();
	EXPECT_EQ(numberOfDocuments, expectedNumberOfDocuments);
}

TEST_F(FixtureAdvancedQueryBuilder, ComparisionEqual)
{
	const int expectedNumberOfDocuments = 264;
	const std::string fieldName = "P_33964739625369635";
	AdvancedQueryBuilder builder;
	ot::Variable value(1000);
	std::string comparator = "==";
	auto comparision = builder.CreateComparison(comparator, value);
	auto query = builder.GenerateFilterQuery(fieldName, std::move(comparision));

	std::vector<std::string> filter{ fieldName };
	auto select = builder.GenerateSelectQuery(filter, true);
	auto response = ExecuteQuery(query, select);

	ot::JsonDocument doc;
	doc.fromJson(response.getResult());
	auto allEntries = doc["Documents"].GetArray();
	auto numberOfDocuments = allEntries.Size();
	EXPECT_EQ(numberOfDocuments, expectedNumberOfDocuments);
}


TEST_F(FixtureAdvancedQueryBuilder, ComparisionNotEqual)
{
	const int expectedNumberOfDocuments = 47622;
	const std::string fieldName = "P_33964739625369635";
	AdvancedQueryBuilder builder;
	ot::Variable value(1000);
	std::string comparator = "!=";
	auto comparision = builder.CreateComparison(comparator, value);
	auto query = builder.GenerateFilterQuery(fieldName, std::move(comparision));

	std::vector<std::string> filter{ fieldName };
	auto select = builder.GenerateSelectQuery(filter, true);
	auto response = ExecuteQuery(query, select);

	ot::JsonDocument doc;
	doc.fromJson(response.getResult());
	auto allEntries = doc["Documents"].GetArray();
	auto numberOfDocuments = allEntries.Size();
	EXPECT_EQ(numberOfDocuments, expectedNumberOfDocuments);
}


TEST_F(FixtureAdvancedQueryBuilder, ComparisionAnyOf)
{
	const int expectedNumberOfDocuments = 7255;
	const std::string fieldName = "Quantity";
	AdvancedQueryBuilder builder;
	std::list<ot::Variable> values{ ot::Variable(33964739625369600),ot::Variable(33964739625369604) };
	auto comparision = builder.CreateComparisionEqualToAnyOf(values);
	auto query = builder.GenerateFilterQuery(fieldName, std::move(comparision));

	std::vector<std::string> filter{ fieldName };
	auto select = builder.GenerateSelectQuery(filter, true);
	auto response = ExecuteQuery(query, select);

	ot::JsonDocument doc;
	doc.fromJson(response.getResult());
	auto allEntries = doc["Documents"].GetArray();
	auto numberOfDocuments = allEntries.Size();
	EXPECT_EQ(numberOfDocuments, expectedNumberOfDocuments);
}

TEST_F(FixtureAdvancedQueryBuilder, ComparisionNotAnyOf)
{
	const int expectedNumberOfDocuments = 40631;
	const std::string fieldName = "Quantity";
	AdvancedQueryBuilder builder;
	std::list<ot::Variable> values{ ot::Variable(33964739625369600),ot::Variable(33964739625369604) };
	auto comparision = builder.CreateComparisionEqualNoneOf(values);
	auto query = builder.GenerateFilterQuery(fieldName, std::move(comparision));

	std::vector<std::string> filter{ fieldName };
	auto select = builder.GenerateSelectQuery(filter, true);
	auto response = ExecuteQuery(query, select);

	ot::JsonDocument doc;
	doc.fromJson(response.getResult());
	auto allEntries = doc["Documents"].GetArray();
	auto numberOfDocuments = allEntries.Size();
	EXPECT_EQ(numberOfDocuments, expectedNumberOfDocuments);
}

TEST_F(FixtureAdvancedQueryBuilder, ComparisionAND)
{
	const int expectedNumberOfDocuments = 16;
	const std::string fieldNameQ = "Quantity";
	const std::string fieldNameP = "P_33964739625369635";
	AdvancedQueryBuilder builder;
	ot::Variable quantityValue (33964739625369600);
	ot::Variable parameterValue (1000);
	auto quantityQuery = builder.GenerateFilterQuery(fieldNameQ, quantityValue);
	auto parameterQuery = builder.GenerateFilterQuery(fieldNameP, parameterValue);
	
	auto query = builder.ConnectWithAND({ quantityQuery,parameterQuery });

	std::vector<std::string> filter{ fieldNameP, fieldNameQ };
	auto select = builder.GenerateSelectQuery(filter, true);
	auto response = ExecuteQuery(query, select);

	ot::JsonDocument doc;
	doc.fromJson(response.getResult());
	auto allEntries = doc["Documents"].GetArray();
	auto numberOfDocuments = allEntries.Size();
	EXPECT_EQ(numberOfDocuments, expectedNumberOfDocuments);
}

TEST_F(FixtureAdvancedQueryBuilder, ComparisionOR)
{
	const int expectedNumberOfDocuments = 3150;
	const std::string fieldNameQ = "Quantity";
	const std::string fieldNameP = "P_33964739625369635";
	AdvancedQueryBuilder builder;
	ot::Variable quantityValue(33964739625369600);
	ot::Variable parameterValue(1000);
	auto quantityQuery = builder.GenerateFilterQuery(fieldNameQ, quantityValue);
	auto parameterQuery = builder.GenerateFilterQuery(fieldNameP, parameterValue);

	auto query = builder.ConnectWithOR({ quantityQuery,parameterQuery });

	std::vector<std::string> filter{ fieldNameP, fieldNameQ };
	auto select = builder.GenerateSelectQuery(filter, true);
	auto response = ExecuteQuery(query, select);

	ot::JsonDocument doc;
	doc.fromJson(response.getResult());
	auto allEntries = doc["Documents"].GetArray();
	auto numberOfDocuments = allEntries.Size();
	EXPECT_EQ(numberOfDocuments, expectedNumberOfDocuments);
}