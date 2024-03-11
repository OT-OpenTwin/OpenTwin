#pragma once
#include <gtest/gtest.h>

#define NPY_NO_DEPRECATED_API NPY_1_7_API_VERSION
#include "numpy/ndarrayobject.h"

class FixtureNumpyAPI : public testing::Test
{
public:
	FixtureNumpyAPI();
	~FixtureNumpyAPI();

};
