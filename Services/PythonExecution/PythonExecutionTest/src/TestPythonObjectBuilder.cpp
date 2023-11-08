#include <gtest/gtest.h>
#include "PythonObjectBuilder.h"
#include "FixturePythonObjectBuilder.h"
#include <limits>
#include <optional>
TEST_F(FixturePythonObjectBuilder, FloatToVariable)
{
	constexpr float expectedValue = std::numeric_limits<float>::max();
	PythonObjectBuilder builder;
	CPythonObjectNew doubleValue = builder.setDouble(expectedValue); //PythonObjects do not differ between float and double. Its all double
	std::optional<ot::Variable> value = 	builder.getVariable(doubleValue);
	double clearValue = value.value().getDouble();
	EXPECT_EQ(expectedValue, clearValue);
}

TEST_F(FixturePythonObjectBuilder, DoubleToVariable)
{
	constexpr double expectedValue = std::numeric_limits<double>::max();
	PythonObjectBuilder builder;
	CPythonObjectNew doubleValue = builder.setDouble(expectedValue);
	std::optional<ot::Variable> value = builder.getVariable(doubleValue);
	double clearValue = value.value().getDouble();
	EXPECT_EQ(expectedValue, clearValue);
}

TEST_F(FixturePythonObjectBuilder, Int32ToVariable)
{
	constexpr int32_t expectedValue = std::numeric_limits<int32_t>::max();
	PythonObjectBuilder builder;
	CPythonObjectNew pValue = builder.setInt32(expectedValue);
	std::optional<ot::Variable> vValue = builder.getVariable(pValue);
	int32_t value = vValue.value().getInt32(); //difference between int32 and int64 cannot be detected in a pythonobject. Thus int32 is stored as a int64.
	EXPECT_EQ(expectedValue, value);
}

TEST_F(FixturePythonObjectBuilder, Int64ToVariable)
{
	constexpr int64_t expectedValue = std::numeric_limits<int64_t>::max();
	PythonObjectBuilder builder;
	CPythonObjectNew pValue = builder.setInt64(expectedValue);
	PyObject* pyValue = PyLong_FromLongLong(expectedValue);
	std::optional<ot::Variable> vValue = builder.getVariable(pValue);
	int64_t value = vValue.value().getInt64();
	EXPECT_EQ(expectedValue, value);
}

TEST_F(FixturePythonObjectBuilder, StringToVariable)
{
	std::string expectedValue = "Blub";
	PythonObjectBuilder builder;
	CPythonObjectNew pValue =  builder.setString(expectedValue);
	std::optional<ot::Variable> vValue = builder.getVariable(pValue);
	std::string value =std::string(vValue.value().getConstCharPtr());
	EXPECT_EQ(expectedValue, value);
}

TEST_F(FixturePythonObjectBuilder, TupleCreation)
{
	PythonObjectBuilder builder;
	builder.StartTupleAssemply(4);
	auto val1 = builder.setInt32(1);
	builder << &val1;
	auto val2 = builder.setString("Temp");
	builder << &val2;
	auto val3 = builder.setBool(true);
	builder << &val3;
	auto val4 = builder.setDouble(3.);
	builder << &val4;

	CPythonObjectNew result(builder.getAssembledTuple());
	EXPECT_TRUE(PyTuple_Check(result));
	EXPECT_TRUE(result != nullptr);
	EXPECT_EQ(PyTuple_Size(result), 4);
}