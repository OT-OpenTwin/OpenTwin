// @otlicense
// File: BsonArrayTypeGetterWrapperTest.cpp
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

#include "FixtureBsonArrayTypeGetterWrapper.h"
#include "BsonArrayTypeGetterWrapper.h"
#include <stdint.h>

TEST_F(FixtureBsonArrayTypeGetterWrapper, TestUnsuportedTypeReaction)
{
	const auto& bsonArray = GetFloatArray();
	EXPECT_ANY_THROW(BsonArrayTypeGetterWrapper<float>::getValue(bsonArray[0]));
}

TEST_F(FixtureBsonArrayTypeGetterWrapper, TestInt32Array)
{
	const auto& bsonArray = GetInt32Array();
	auto val = BsonArrayTypeGetterWrapper<int32_t>::getValue(bsonArray[0]);
	EXPECT_EQ(typeid(val).name(),typeid(int32_t).name());
}

TEST_F(FixtureBsonArrayTypeGetterWrapper, TestIntArray)
{
	const auto& bsonArray = GetInt32Array();
	auto val = BsonArrayTypeGetterWrapper<int>::getValue(bsonArray[0]);
	EXPECT_EQ(typeid(val).name(), typeid(int32_t).name());
}

TEST_F(FixtureBsonArrayTypeGetterWrapper, TestInt64Array)
{
	const auto& bsonArray = GetInt64Array();
	auto val = BsonArrayTypeGetterWrapper<int64_t>::getValue(bsonArray[0]);
	EXPECT_EQ(typeid(val).name(), typeid(int64_t).name());
}

TEST_F(FixtureBsonArrayTypeGetterWrapper, TestUTF8StringArray)
{
	const auto& bsonArray = GetUTF8StringArray();
	auto val = BsonArrayTypeGetterWrapper<std::string>::getValue(bsonArray[0]);
	EXPECT_EQ(typeid(val).name(), typeid(std::string).name());
}


TEST_F(FixtureBsonArrayTypeGetterWrapper, TestDoubleArray)
{
	const auto& bsonArray = GetDoubleArray();
	auto val = BsonArrayTypeGetterWrapper<double>::getValue(bsonArray[0]);
	EXPECT_EQ(typeid(val).name(), typeid(double).name());
}