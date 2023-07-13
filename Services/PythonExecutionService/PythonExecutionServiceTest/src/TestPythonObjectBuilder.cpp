#include <gtest/gtest.h>
#include "PythonObjectBuilder.h"
#include "FixturePythonObjectBuilder.h"
#include <limits>
TEST_F(FixturePythonObjectBuilder, FloatToVariable)
{
	float expectedValue = std::numeric_limits<float>::max();
	PythonObjectBuilder builder;
	CPythonObjectNew doubleValue = builder.setDouble(expectedValue); //PythonObjects do not differ between float and double. Its all double
	std::optional<variable_t> value = 	builder.getVariable(doubleValue);
	double clearValue = std::get<double>(value.value());
	EXPECT_EQ(expectedValue, clearValue);
}

TEST_F(FixturePythonObjectBuilder, DoubleToVariable)
{
	constexpr double expectedValue = std::numeric_limits<double>::max();
	PythonObjectBuilder builder;
	CPythonObjectNew doubleValue = builder.setDouble(expectedValue);
	std::optional<variable_t> value = builder.getVariable(doubleValue);
	double clearValue = std::get<double>(value.value());
	EXPECT_EQ(expectedValue, clearValue);
}

TEST_F(FixturePythonObjectBuilder, Int32ToVariable)
{
	int32_t expectedValue = std::numeric_limits<int32_t>::max();
	PythonObjectBuilder builder;
	CPythonObjectNew pValue = builder.setInt32(expectedValue);
	std::optional<variable_t> vValue = builder.getVariable(pValue);
	double value = std::get<int64_t>(vValue.value()); //difference between int32 and int64 cannot be detected in a pythonobject. Thus int32 is stored as a int64.
	EXPECT_EQ(expectedValue, value);
}

TEST_F(FixturePythonObjectBuilder, Int64ToVariable)
{
	int64_t expectedValue = std::numeric_limits<int64_t>::max();
	PythonObjectBuilder builder;
	CPythonObjectNew pValue = builder.setInt64(expectedValue);
	PyObject* pyValue = PyLong_FromLongLong(expectedValue);
	std::optional<variable_t> vValue = builder.getVariable(pValue);
	double value = std::get<int64_t>(vValue.value());
	EXPECT_EQ(expectedValue, value);
}

TEST_F(FixturePythonObjectBuilder, StringToVariable)
{
	std::string expectedValue = "Blub";
	PythonObjectBuilder builder;
	CPythonObjectNew pValue =  builder.setString(expectedValue);
	std::optional<variable_t> vValue = builder.getVariable(pValue);
	std::string value =std::string(std::get<const char*>(vValue.value()));
	EXPECT_EQ(expectedValue, value);
}