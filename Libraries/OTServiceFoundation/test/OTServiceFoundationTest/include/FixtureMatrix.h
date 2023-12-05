#pragma once
#include "gtest/gtest.h"
#include "OpenTwinFoundation/Matrix.h"
#include <string>

class FixtureMatrix : public testing::Test
{
public:
	FixtureMatrix();
	Matrix<int>* CreateSquareMatrixRowWiseOptimizedInt();

	std::vector<std::vector<int>> GetExpectedData() const { return _expectedDataInt; }
private:
	std::vector<std::vector<int>> _expectedDataInt;
	int _rows;
	int _columns;
};
