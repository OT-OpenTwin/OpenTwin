#include <iostream>
#include "gtest/gtest.h"
#include "OTCore/Variable.h"
#include "OTCore/VariableToJSONConverter.h"
#include "OTCore/JSONToVariableConverter.h"
#include "OTCore/StringToVariableConverter.h"

#include "FixtureVariable.h"

#include <string>
#include <limits>

TEST(VariableTest, CopyAssignOperator)
{
	ot::Variable first(5);
	ot::Variable second(0);
	second = first;
	EXPECT_EQ(first, second);
}

TEST(VariableTest, VariableToJSON)
{
	ot::JsonDocument (doc);
	ot::Variable var = 5;
	ot::VariableToJSONConverter converter;
	rapidjson::Value result = converter(var,doc.GetAllocator());
	EXPECT_TRUE(result.IsInt());
	EXPECT_EQ(result.GetInt(), var.getInt32());
}

TEST(VariableTest, VariableInt64ToJSON)
{
	ot::JsonDocument(doc);
	int64_t temp = 13;
	ot::Variable var(temp);
	ot::VariableToJSONConverter converter;
	rapidjson::Value result = converter(var, doc.GetAllocator());
	EXPECT_TRUE(result.IsInt64());
	EXPECT_EQ(result.GetInt64(), var.getInt64());
}

TEST(VariableTest, VariableInt32ToJSON)
{
	ot::JsonDocument(doc);
	int32_t temp = 13;
	ot::Variable var(temp);
	ot::VariableToJSONConverter converter;
	rapidjson::Value result = converter(var, doc.GetAllocator());
	EXPECT_TRUE(result.IsInt());
	EXPECT_EQ(result.GetInt(), var.getInt32());
}
TEST(VariableTest, VariableIntDoubleToJSON)
{
	ot::JsonDocument(doc);
	double temp = 13.;
	ot::Variable var(temp);
	ot::VariableToJSONConverter converter;
	rapidjson::Value result = converter(var, doc.GetAllocator());
	EXPECT_TRUE(result.IsDouble());
	EXPECT_EQ(result.GetDouble(), var.getDouble());
}

TEST(VariableTest, VariableIntFloatToJSON)
{
	ot::JsonDocument(doc);
	float temp = 13.f;
	ot::Variable var(temp);
	ot::VariableToJSONConverter converter;
	rapidjson::Value result = converter(var, doc.GetAllocator());
	EXPECT_TRUE(result.IsFloat());
	EXPECT_EQ(result.GetFloat(), var.getFloat());
}

TEST(VariableTest, VariableComplexToJSON)
{
	ot::JsonDocument(doc);
	ot::complex value(3.5, -0.2);
	ot::Variable var(value);

	ot::VariableToJSONConverter converter;
	rapidjson::Value result = converter(var, doc.GetAllocator());
	EXPECT_TRUE(result.IsObject());
	ot::JSONToVariableConverter backConverter;
	ot::Variable actual = backConverter(result);
	EXPECT_TRUE(actual.isComplex());

	EXPECT_EQ(actual.getComplex(), var.getComplex());
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

float calculate(float start, float decrement, int count)
{
	for (int i = 0; i < count; i++) {
		start -= decrement;
	}
	return start;
}

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

/*
TEST(VariableTest, FloatComparision)
{
	const int total = 10000;
	int count = 0;
	for (auto i = 0; i < total; ++i)
	{
		float expected = (i / 10.0f);
		float actual = (float)calculate(9.0f + expected, 0.01f, 900);
		if (ot::Variable(actual) == ot::Variable(expected))
		{
			++count;
		}
	}
	EXPECT_EQ(count, total);
}
*/

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

TEST(StringToVariable, StringToInt32Variable)
{	
	constexpr const int32_t expectedValue = std::numeric_limits<int32_t>::max();
	ot::StringToVariableConverter converter;
	const ot::Variable actualValue = converter(std::to_string(expectedValue), '.');
	
	EXPECT_TRUE(actualValue.isInt32());
	EXPECT_EQ(actualValue.getInt32(), expectedValue);
}

TEST(StringToVariable, StringToInt64Variable)
{
	//const int64_t expectedValue = std::numeric_limits<int64_t>::max();
	constexpr const long long expectedValue = std::numeric_limits<long long>::max();
	ot::StringToVariableConverter converter;
	const ot::Variable actualValue = converter(std::to_string(expectedValue), '.');
	
	EXPECT_TRUE(actualValue.isInt64());
	EXPECT_EQ(actualValue.getInt64(), expectedValue);
}

TEST(StringToVariable, StringToFloatVariable)
{
	const float expectedValue = 4.f;
	ot::StringToVariableConverter converter;
	const ot::Variable actualValue = converter(std::to_string(expectedValue), '.');

	EXPECT_TRUE(actualValue.isFloat());
	EXPECT_TRUE(actualValue.getFloat() == expectedValue);
}

TEST(StringToVariable, StringToDoubleVariable)
{
	constexpr const double expectedValue = std::numeric_limits<double>::max();
	ot::Variable expectedVariable(expectedValue);
	ot::StringToVariableConverter converter;
	const ot::Variable actualValue = converter(std::to_string(expectedValue), '.');

	EXPECT_TRUE(actualValue.isDouble());
	EXPECT_TRUE(actualValue == expectedVariable);
}

TEST(StringToVariable, StringToStringVariable)
{
	const std::string expectedValue = "\"4\"";
	ot::StringToVariableConverter converter;
	const ot::Variable actualValue = converter(expectedValue, '.');

	EXPECT_TRUE(actualValue.isConstCharPtr());
	EXPECT_EQ(actualValue.getConstCharPtr(), expectedValue);
}

TEST(StringToVariable, String)
{
	ot::StringToVariableConverter converter;
	const ot::Variable actualValue = converter("F", '.');
	EXPECT_TRUE(actualValue.isConstCharPtr());
}

TEST(StringToVariable, StringLoadTest)
{
	ot::StringToVariableConverter converter;
	for (int i = 0; i < 10000000; i++)
	{
		const ot::Variable actualValue("someContent");
	}
	EXPECT_TRUE(true);
}



TEST(StringToVariable, ScientificNotation)
{
	const float expectedValue = -1.E3f;
	ot::StringToVariableConverter converter;
	const ot::Variable actualValue = converter("-1.00E+03", '.');
	EXPECT_TRUE(actualValue.isFloat());
	EXPECT_EQ(actualValue.getFloat(), expectedValue);
}

TEST(StringToVariable, ScientificNotation_ErrorCase)
{
	const float expectedValue = 0.f;
	ot::StringToVariableConverter converter;
	const ot::Variable actualValue = converter("0000000E+00", '.');
	EXPECT_TRUE(actualValue.isFloat());
	EXPECT_EQ(actualValue.getFloat(), expectedValue);
}


TEST(StringToVariable, ScientificNotationWithCommaAsDecimalSeparator)
{
	const float expectedValue = -1.E3f;

	ot::StringToVariableConverter converter;
	const ot::Variable actualValue = converter("-1,00E+3", ',');
	EXPECT_TRUE(actualValue.isFloat());
	EXPECT_EQ(actualValue.getFloat(), expectedValue);
}


TEST(VariableTest, StringMove)
{
	std::vector<ot::Variable> var;
	var.reserve(1);
	var.push_back(ot::Variable("initial value"));

	var[0] = ot::Variable("new Value");

	EXPECT_TRUE(true);
}


TEST(StringToVariable, StringToFloatVariableWithThousandSeparators)
{
	constexpr const float expectedValue = 1512345.13f;
	std::string str = "1,512,345.13";
	ot::StringToVariableConverter converter;
	const ot::Variable actualValue = converter(str, '.');

	EXPECT_TRUE(actualValue.isFloat());
	EXPECT_EQ(actualValue.getFloat(), expectedValue);
}

TEST(StringToVariable, StringToFloatVariableWithCommaDecimalSeparator)
{
	constexpr const float expectedValue = 1512345.13f;
	std::string str = "1512345,13";
	ot::StringToVariableConverter converter;
	const ot::Variable actualValue = converter(str, ',');

	EXPECT_TRUE(actualValue.isFloat());
	EXPECT_EQ(actualValue.getFloat(), expectedValue);
}