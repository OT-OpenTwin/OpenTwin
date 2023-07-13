#include <gtest/gtest.h>
#include "PythonObjectBuilder.h"
#include "FixturePythonObjectBuilder.h"

TEST_F(FixturePythonObjectBuilder, FloatToVariable)
{
	double expectedValue = 13.;
	PythonObjectBuilder builder;
	CPythonObjectNew doubleValue = builder.setDouble(expectedValue);
	variable_t value = 	builder.getVariable(doubleValue);
	double clearValue = std::get<double>(value);
	EXPECT_EQ(expectedValue, clearValue);
}

TEST_F(FixturePythonObjectBuilder, Int32ToVariable)
{
	int32_t expectedValue = 13;
	PythonObjectBuilder builder;
	CPythonObjectNew pValue = builder.setInt32(expectedValue);
	variable_t vValue = builder.getVariable(pValue);
	double value = std::get<int32_t>(vValue);
	EXPECT_EQ(expectedValue, value);
}


TEST_F(FixturePythonObjectBuilder, StringToVariable)
{
	std::string expectedValue = "Blub";
	PythonObjectBuilder builder;
	CPythonObjectNew pValue =  builder.setString(expectedValue);
	variable_t vValue = builder.getVariable(pValue);
	std::string value =std::string(std::get<const char*>(vValue));
	EXPECT_EQ(expectedValue, value);
}