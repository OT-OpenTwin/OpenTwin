// @otlicense
// File: FixtureAdvancedQueryBuilder.h
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
