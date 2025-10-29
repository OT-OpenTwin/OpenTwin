// @otlicense

#include "FixtureBsonArrayTypeGetterWrapper.h"
#include "BsonArrayTypeGetterWrapper.h"
#include <stdint.h>

TEST_F(FixtureBsonArrayTypeGetterWrapper, TestUnsuportedTypeReaction)
{
	auto bsonArray = GetFloatArray();
	EXPECT_ANY_THROW(BsonArrayTypeGetterWrapper<float>::getValue(bsonArray[0]));
}

TEST_F(FixtureBsonArrayTypeGetterWrapper, TestInt32Array)
{
	auto bsonArray = GetInt32Array();
	auto val = BsonArrayTypeGetterWrapper<int32_t>::getValue(bsonArray[0]);
	EXPECT_EQ(typeid(val).name(),typeid(int32_t).name());
}

TEST_F(FixtureBsonArrayTypeGetterWrapper, TestIntArray)
{
	auto bsonArray = GetInt32Array();
	auto val = BsonArrayTypeGetterWrapper<int>::getValue(bsonArray[0]);
	EXPECT_EQ(typeid(val).name(), typeid(int32_t).name());
}

TEST_F(FixtureBsonArrayTypeGetterWrapper, TestInt64Array)
{
	auto bsonArray = GetInt64Array();
	auto val = BsonArrayTypeGetterWrapper<int64_t>::getValue(bsonArray[0]);
	EXPECT_EQ(typeid(val).name(), typeid(int64_t).name());
}

TEST_F(FixtureBsonArrayTypeGetterWrapper, TestUTF8StringArray)
{
	auto bsonArray = GetUTF8StringArray();
	auto val = BsonArrayTypeGetterWrapper<std::string>::getValue(bsonArray[0]);
	EXPECT_EQ(typeid(val).name(), typeid(std::string).name());
}


TEST_F(FixtureBsonArrayTypeGetterWrapper, TestDoubleArray)
{
	auto bsonArray = GetDoubleArray();
	auto val = BsonArrayTypeGetterWrapper<double>::getValue(bsonArray[0]);
	EXPECT_EQ(typeid(val).name(), typeid(double).name());
}