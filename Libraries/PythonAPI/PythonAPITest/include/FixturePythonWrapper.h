#pragma once
#include <gtest/gtest.h>
#include "PythonWrapper.h"
#include <string>

class FixturePythonWrapper : public testing::Test
{
public:
	FixturePythonWrapper();
	PythonWrapper* getPythonWrapper();
	void SetFalsePythonSysPath();
	void CleanDictionaryCopy();
private:

	PythonWrapper pythonWrapper;
};


