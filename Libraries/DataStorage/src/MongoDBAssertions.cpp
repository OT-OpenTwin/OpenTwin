// @otlicense
// File: MongoDBAssertions.cpp
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

#include "stdafx.h"
#include "MongoDBAssertions.h"
#include "../include/ArrayBinaryConverter.h"
#include <cassert>

using bsoncxx::builder::basic::kvp;
void DataStorageAPI::MongoDBAssertions::testMongoDBAssertions()
{
	auto MongoFile = Document{};
	auto fileView = MongoFile.view();
	assert(fileView.length() == 5 && "The Empty MongoDB size should be 5 Bytes");

	MongoFile.append(kvp("key", "value"));
	fileView = MongoFile.view();
	assert(fileView.length() == 20 && "The additional bytes required for UTF8 element should be 7 Bytes");
	
}
void DataStorageAPI::MongoDBAssertions::testMongoDBAssertions(mongocxx::client& client)
{
	testMongoDBAssertions();

	std::string dbName = "TestDB";
	std::string collName = "TestColl";
	mongocxx::database db = client[dbName];
	mongocxx::collection coll = db[collName];

	//Array size in Bytes 16777184
	auto MongoFile = Document{};
	size_t arraySize = 4194296;
	int* tempArr = new int[arraySize];
	MongoFile.append(kvp("key", DataStorageAPI::ArrayBinaryConverter::convertArrayToBinary(tempArr, arraySize)));
	auto fileView = MongoFile.view();
	assert(fileView.length() == 16777199 && "The MongoDB document size should be 16777199 Bytes");
	try {
		coll.insert_one(fileView);
	}
	catch (const std::exception) {
		assert(0 && "MongoDB should allow maximum document size of 16777199, excluding _id key");
	}
	//Cleaning the test database
	db.drop();
}
