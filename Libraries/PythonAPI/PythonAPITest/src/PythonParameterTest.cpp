#include <gtest/gtest.h>
#include "PythonParameter.h"

TEST(PythonParameter, TestSupportedDataTypes)
{
	const std::string parameterName = "Parameter";
	const int32_t value = 13;
		
	ASSERT_NO_THROW(PythonAPI::PythonParameter<int32_t>(parameterName, value));
}

TEST(PythonParameter, TestNotSupportedDataTypes)
{
	const std::string parameterName = "Parameter";
	const char value = 'q';
	ASSERT_ANY_THROW(PythonAPI::PythonParameter<char>(parameterName, value));
}


