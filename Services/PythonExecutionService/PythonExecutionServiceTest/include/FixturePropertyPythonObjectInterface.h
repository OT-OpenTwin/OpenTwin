#pragma once
#include <gtest/gtest.h>
#include "PythonWrapper.h"

class FixturePropertyPythonObjectInterface : public testing::Test
{
public:
	FixturePropertyPythonObjectInterface();

private:
	PythonWrapper _wrapper;
};
