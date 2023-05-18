#include <gtest/gtest.h>
#include "PythonParameter.h"

TEST(PythonParameter, TestSupportedDataTypes_int32)
{
	const std::string parameterName = "Parameter";
	const int32_t value = 13;
		
	ASSERT_NO_THROW(PythonAPI::PythonParameter<int32_t>(parameterName, value));
}

TEST(PythonParameter, TestSupportedDataTypes_int64)
{
	const std::string parameterName = "Parameter";
	const int64_t value = 13;

	ASSERT_NO_THROW(PythonAPI::PythonParameter<int64_t>(parameterName, value));
}
TEST(PythonParameter, TestSupportedDataTypes_double)
{
	const std::string parameterName = "Parameter";
	const double value = 13.;

	ASSERT_NO_THROW(PythonAPI::PythonParameter<double>(parameterName, value));
}

TEST(PythonParameter, TestSupportedDataTypes_float)
{
	const std::string parameterName = "Parameter";
	const float value = 13.f;

	ASSERT_NO_THROW(PythonAPI::PythonParameter<float>(parameterName, value));
}

TEST(PythonParameter, TestSupportedDataTypes_stringUTF8)
{
	const std::string parameterName = "Parameter";
	const std::string value = "13";

	ASSERT_NO_THROW(PythonAPI::PythonParameter<std::string>(parameterName, value));
}
TEST(PythonParameter, TestSupportedDataTypes_bool)
{
	const std::string parameterName = "Parameter";
	const bool value = true;

	ASSERT_NO_THROW(PythonAPI::PythonParameter<bool>(parameterName, value));
}


TEST(PythonParameter, TestNotSupportedDataType_char)
{
	const std::string parameterName = "Parameter";
	const char value = 'q';
	ASSERT_ANY_THROW(PythonAPI::PythonParameter<char>(parameterName, value));
}

TEST(PythonParameter, TestParameterString)
{
	const std::string parameterName = "Parameter";
	const int32_t value = 13;
	const std::string expectedParameterString = "Parameter=13";

	PythonAPI::PythonParameter<int32_t> parameter(parameterName, value);
	ASSERT_EQ(parameter.getParameterString(), expectedParameterString);
}


