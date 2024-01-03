#include <iostream>
#include "gtest/gtest.h"
#include "OTCore/Variable.h"
#include "OTCore/VariableToJSONConverter.h"
#include "OTCore/JSONToVariableConverter.h"
#include "FixtureVariable.h"

#include <string>

TEST(VariableTest, VariableToJSON)
{
	ot::JsonDocument (doc);
	ot::Variable var = 5;
	ot::VariableToJSONConverter converter;
	rapidjson::Value result = converter(var,doc.GetAllocator());
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

double calculate(double start, double decrement, int count)
{
	for (int i = 0; i < count; ++i)
		start -= decrement;
	return start;
}

//float calculate(float start, float decrement, int count)
//{
//	for (int i = 0; i < count; ++i)
//		start -= decrement;
//	return start;
//}

TEST(VariableTest, DoubleComparision)
{
	const int total = 10000;
	int count = 0;
	for (auto i = 0; i < total; ++i)
	{
		double expected = (i / 10.0);
		double actual = calculate(9.0 + expected, 0.1, 90);
		if(ot::Variable(actual) == ot::Variable(expected))
		{
			++count;
		}
	}
	EXPECT_EQ(count, total);
}

TEST_F(FixtureVariable, UniqueList)
{
	std::list< ot::Variable> list = GetSMA8_333_FA_Amplitudes();
	list.sort();
	list.unique();

	const size_t expectedNumberOfEntries = 327;
	const size_t actualNumberOfEntries = list.size();

	EXPECT_EQ(expectedNumberOfEntries,actualNumberOfEntries);
}

TEST(VariableTest, FloatComparision)
{
	const int total = 10000;
	int count = 0;
	for (auto i = 0; i < total; ++i)
	{
		float expected = (i / 10.0f);
		float actual = calculate(9.0f + expected, 0.01f, 900);
		if (ot::Variable(actual) == ot::Variable(expected))
		{
			++count;
		}
	}
	EXPECT_EQ(count, total);
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