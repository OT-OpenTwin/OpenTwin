// @otlicense

#pragma once
#include "gtest/gtest.h"

class FixtureIntrinsics : public testing::Test
{
	float *_a = nullptr;
	float *_b = nullptr;
	const int _vectorSize = 16;
	const int _alignment = 512;

public:
	FixtureIntrinsics();
	~FixtureIntrinsics();
	float * GetVectorB() const { return _b; };
	float * GetVectorA() const { return _a; };
	const int GetAlignment()const { return _alignment; }
	const int GetVectorSize()const { return _vectorSize; };
};
