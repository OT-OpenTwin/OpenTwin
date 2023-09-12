#include <iostream>
#include "gtest/gtest.h"
#include "OpenTwinCore/Variable.h"
#include "OpenTwinCore/VariableToJSONConverter.h"
#include "OpenTwinCore/JSONToVariableConverter.h"
#include "FixtureVariable.h"

#include <string>

TEST(VariableTest, VariableToJSON)
{
	OT_rJSON_createDOC(doc);
	ot::Variable var = 5;
	ot::VariableToJSONConverter converter;
	rapidjson::Value result = converter(var,doc);
	EXPECT_TRUE(result.IsInt());
	EXPECT_EQ(result.GetInt(), var.getInt32());
}


TEST(VariableTest, JSONToVariable)
{
	int expected = 5;
	rapidjson::Value var(expected);

	ot::JSONToVariableConverter converter;
	ot::Variable result = converter(var);
	
	EXPECT_TRUE(result.isInt32());
	EXPECT_EQ(var.GetInt(), result.getInt32());
}

TEST(VariableTest, JSONToVariableNotSupportedType)
{
	rapidjson::Value var;
	ot::JSONToVariableConverter converter;
	
	EXPECT_ANY_THROW(converter(var));
}

TEST_P(FixtureVariable, Equal)
{
	ot::Variable isValue = GetParam();
	auto expectedValues= GetExpectedEqualEntries();
	
	bool equal = false;
	int count = 0;
	for (ot::Variable& var : expectedValues)
	{
		if (var == isValue)
		{
			equal = true;
			count++;
		}
	}
	EXPECT_EQ(count, 1) << "Failed with type: " + isValue.getTypeName();
	EXPECT_TRUE(equal) << "Failed with type: " + isValue.getTypeName();
}

TEST_P(FixtureVariable, Larger)
{
	ot::Variable isValue = GetParam();
	if (isValue.isBool())
	{
		EXPECT_TRUE(true);
	}
	else
	{
		auto expectedValues = GetExpectedLargerEntries();
		bool larger = false;
		int count = 0;
		for (ot::Variable& var : expectedValues)
		{
			if (var > isValue)
			{
				larger = true;
				count++;
			}
		}
		EXPECT_EQ(count, 1) << "Failed with type: " + isValue.getTypeName();
		EXPECT_TRUE(larger) << "Failed with type: " + isValue.getTypeName();
	}
}

TEST_P(FixtureVariable, Smaller)
{
	ot::Variable isValue = GetParam();
	if (isValue.isBool())
	{
		EXPECT_TRUE(true);
	}
	else
	{
		auto expectedValues = GetExpectedLargerEntries();
		bool larger = false;
		int count = 0;
		for (ot::Variable& var : expectedValues)
		{
			if (isValue < var)
			{
				larger = true;
				count++;
			}
		}
		EXPECT_EQ(count, 1) << "Failed with type: " + isValue.getTypeName();
		EXPECT_TRUE(larger) << "Failed with type: " + isValue.getTypeName();
	}
}