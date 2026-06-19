// @otlicense
// File: TestCurlCoefficients.cpp
// 
// License:
// Copyright 2025 by OpenTwin
//  
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
//  
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// @otlicense-end

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