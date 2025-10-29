// @otlicense

#include "FixtureCurlCoefficients3D.h"
#include "MeasuredTargetValues.h"
#include "gtest/gtest.h"


TEST_F(FixtureCurlCoefficients3D, TestGridConsistency)
{
	ASSERT_NO_THROW(SetCorrectUniformCubeGrid());
}

TEST_F(FixtureCurlCoefficients3D, TestTimestepwidth)
{
	SetCorrectUniformCubeGrid();
	
	float expectedTimeDiscretizationValue = _measuredTargetValuesSP.GetUniformGridTimeDiscretization();
	float calculatedTimeDiscretizationValue = GetCurlCoefficients3DSp()->GetTimeDiscretization();
	ASSERT_FLOAT_EQ(expectedTimeDiscretizationValue, calculatedTimeDiscretizationValue);
}

TEST_F(FixtureCurlCoefficients3D, TestGridCoefficients)
{
	SetCorrectUniformCubeGrid();
	float expectedCoefficientValue = _measuredTargetValuesSP.GetUniformGridCoefficientValue();
	Grid<float> * gridPtr = GetGridSp();
	CurlCoefficients3D<float> * coeffPtr = GetCurlCoefficients3DSp();

	for (index_t i = 0; i < gridPtr->GetDegreesOfFreedomNumberTotal(); i++)
	{
		ASSERT_FLOAT_EQ(*coeffPtr->GetCoefficientComponentXCurlYAtIndex(i), expectedCoefficientValue);
		ASSERT_FLOAT_EQ(*coeffPtr->GetCoefficientComponentXCurlZAtIndex(i), expectedCoefficientValue);
		ASSERT_FLOAT_EQ(*coeffPtr->GetCoefficientComponentYCurlXAtIndex(i), expectedCoefficientValue);
		ASSERT_FLOAT_EQ(*coeffPtr->GetCoefficientComponentYCurlZAtIndex(i), expectedCoefficientValue);
		ASSERT_FLOAT_EQ(*coeffPtr->GetCoefficientComponentZCurlXAtIndex(i), expectedCoefficientValue);
		ASSERT_FLOAT_EQ(*coeffPtr->GetCoefficientComponentZCurlYAtIndex(i), expectedCoefficientValue);
	}
}

TEST_F(FixtureCurlCoefficients3D, TestGridDualCoefficients)
{
	SetCorrectUniformCubeDualGrid();
	float expectedCoefficientValue = _measuredTargetValuesSP.GetUniformGridDualCoefficientValue();
	Grid<float> * gridPtr = GetGridSp();
	CurlCoefficients3D<float> * coeffPtr = GetCurlCoefficients3DSp();

	for (index_t i = 0; i < gridPtr->GetDegreesOfFreedomNumberTotal(); i++)
	{
		ASSERT_FLOAT_EQ(*coeffPtr->GetCoefficientComponentXCurlYAtIndex(i), expectedCoefficientValue);
		ASSERT_FLOAT_EQ(*coeffPtr->GetCoefficientComponentXCurlZAtIndex(i), expectedCoefficientValue);
		ASSERT_FLOAT_EQ(*coeffPtr->GetCoefficientComponentYCurlXAtIndex(i), expectedCoefficientValue);
		ASSERT_FLOAT_EQ(*coeffPtr->GetCoefficientComponentYCurlZAtIndex(i), expectedCoefficientValue);
		ASSERT_FLOAT_EQ(*coeffPtr->GetCoefficientComponentZCurlXAtIndex(i), expectedCoefficientValue);
		ASSERT_FLOAT_EQ(*coeffPtr->GetCoefficientComponentZCurlYAtIndex(i), expectedCoefficientValue);
	}
}

TEST_F(FixtureCurlCoefficients3D, TestGridOnlyPECElements)
{
	ASSERT_ANY_THROW(SetCorrectUniformPECCubeGrid());
}