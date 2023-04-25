#include <iostream>
#include "gtest/gtest.h"
#include "FixtureMatrix.h"


TEST_F(FixtureMatrix, TestMatrixCreationInt)
{
	Matrix<int>* result = CreateSquareMatrixRowWiseOptimizedInt();
	for (int j = 0; j < result->GetNumberOfColumns(); j++)
	{
		for (int i = 0; i < result->GetNumberOfRows(); i++)
		{
			int testValue = result->GetValue(i, j);
			int expectedValue = GetExpectedData()[j][i];
			EXPECT_EQ(testValue, expectedValue);
		}
	}
}