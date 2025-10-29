// @otlicense

#pragma once
#include <gtest/gtest.h>

#include "PythonWrapper.h"

class FixturePythonObjectBuilder : public testing::Test
{
public:
	FixturePythonObjectBuilder();
private:
	PythonWrapper _wrapper;
};
