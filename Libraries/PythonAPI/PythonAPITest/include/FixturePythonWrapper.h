#pragma once
#include <gtest/gtest.h>
#include "PythonWrapper.h"
#include <string>

class FixturePythonWrapper : public testing::Test
{
public:
	FixturePythonWrapper();
	PythonWrapper* getPythonWrapper();
	std::string getAllGlobalParameter();
	void SetFalsePythonSysPath();

private:
	PythonWrapper pythonWrapper;
};


