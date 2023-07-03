#pragma once
#include "PythonWrapper.h"
#include <gtest/gtest.h>

#ifdef _RELEASEDEBUG
#define n 5
#else
#define n 5
#endif // RELEASEDEBUG

class FixturePythonWrapper : testing::Test
{
public:
	FixturePythonWrapper();

private:
	PythonWrapper wrapper;

	void ExecuteString(const std::string& command);
};
