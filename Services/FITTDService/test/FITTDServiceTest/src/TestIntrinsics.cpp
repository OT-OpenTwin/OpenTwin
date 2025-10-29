// @otlicense

#include "gtest/gtest.h"
#include "FixtureIntrinsics.h"
#include <immintrin.h>
#include "SystemDependencies/SystemProperties.h"
#include <signal.h>


TEST_F(FixtureIntrinsics, FeatureDetectionAVX512Test)
{
	auto systemProperties = SystemProperties::GetInstance();
	auto vector = GetVectorA();
	if (systemProperties.CPUSupportsAVX512())
	{
		EXPECT_NO_THROW([vector]() {__m512 registerLoadedA = _mm512_load_ps(vector); });
	}
	else
	{
		//The oposite cannot be tested (at least on Windows). The operation would cause an Invalid Operation Signal. On other OS than windows the functionality: EXPECT_EXIT(myObject.doWork(), ::testing::KilledBySignal(SIGBART)) is supported.
		//Implementing a signal handler doesn't work either. The handler is never being reached.
		EXPECT_EQ(0, 0);
	}

}

TEST_F(FixtureIntrinsics, AddOperationAVX512Test)
{
	auto systemProperties = SystemProperties::GetInstance();
	if (systemProperties.CPUSupportsAVX512())
	{
		__m512 registerLoadedA = _mm512_load_ps(GetVectorA());
		__m512 registerLoadedB = _mm512_load_ps(GetVectorB());
		__m512 registerLoadedResult = _mm512_add_ps(registerLoadedA, registerLoadedB);
		float * result = static_cast<float*>(_aligned_malloc(GetVectorSize() * sizeof(float), GetAlignment()));
		_mm512_store_ps(result, registerLoadedResult);
		float *expectedResult = new float[GetVectorSize()]{ 1,3,5,7,9,11,13,15,17,19,21,23,25,27,29,31 };
		for (int i = 0; i < 16; i++)
		{
			EXPECT_DOUBLE_EQ(expectedResult[i], result[i]) << "Index: " << i;
		}
	}
}

TEST_F(FixtureIntrinsics, StoringWithinLargerVectorTest)
{
	auto systemProperties = SystemProperties::GetInstance();
	if (systemProperties.CPUSupportsAVX512())
	{
		__m512 registerLoadedA = _mm512_load_ps(GetVectorA());
		int additionalSize = 10;
		float * result = static_cast<float*>(_aligned_malloc(GetVectorSize() + additionalSize * sizeof(float), GetAlignment()));
		for (int i = 0; i < GetVectorSize() + additionalSize; i++)
		{
			result[i] = -1.f;
		}
		int insertStartIndex = 4;
		_mm512_store_ps(&result[insertStartIndex], registerLoadedA);
		float *expectedResult = new float[GetVectorSize()]{ 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15 };

		for (int i = 0; i < insertStartIndex; i++)
		{
			EXPECT_DOUBLE_EQ(-1.f, result[i]) << "Index: " << i;
		}
		for (int i = 0; i < GetVectorSize(); i++)
		{
			EXPECT_DOUBLE_EQ(expectedResult[i], result[i + insertStartIndex]) << "Index: " << i;
		}
		for (int i = GetVectorSize() + insertStartIndex; i < GetVectorSize() + additionalSize; i++)
		{
			EXPECT_DOUBLE_EQ(-1.f, result[i]) << "Index: " << i;
		}
	}
}

TEST_F(FixtureIntrinsics, MaskedAddTest)
{
	auto systemProperties = SystemProperties::GetInstance();
	if (systemProperties.CPUSupportsAVX512())
	{
		__m512 registerLoadedA = _mm512_load_ps(GetVectorA());
		__m512 registerLoadedB = _mm512_load_ps(GetVectorB());
		__mmask16 mask = 0i16;
		mask |= 1i16 << 15;
		mask |= 1i16 << 10;
		mask |= 1i16 << 3;
		mask |= 1i16 << 4;
		mask |= 1i16 << 5;
		mask |= 1i16 << 6;
		__m512 registerLoadedResult = _mm512_add_ps(registerLoadedA, registerLoadedB);
		float * result = static_cast<float*>(_aligned_malloc(GetVectorSize() * sizeof(float), GetAlignment()));

		for (int i = 0; i < GetVectorSize(); i++)
		{
			result[i] = -1;
		}

		_mm512_mask_store_ps(result, mask, registerLoadedResult);

		float *expectedResult = new float[GetVectorSize()]{ 1,3,5,7,9,11,13,15,17,19,21,23,25,27,29,31 };

		for (int i = 0; i < 16; i++)
		{
			if (i == 15 || i == 10 || i == 3 || i == 4 || i == 5 || i == 6)
			{
				EXPECT_DOUBLE_EQ(expectedResult[i], result[i]) << "Index: " << i;
			}
			else
			{
				EXPECT_DOUBLE_EQ(-1, result[i]) << "Index: " << i;
			}
		}
	}
}

TEST_F(FixtureIntrinsics, AddWithOffsetTest)
{
	auto systemProperties = SystemProperties::GetInstance();
	if (systemProperties.CPUSupportsAVX512())
	{
		__m512 registerLoadedA = _mm512_load_ps(GetVectorA());

		float * vectorC = static_cast<float*>(_aligned_malloc(32 * sizeof(float), GetAlignment()));
		for (int i = 0; i < 32; i++)
		{
			vectorC[i] = -i;
		}
		__m512 registerLoadedC = _mm512_load_ps(&vectorC[3]);

		__m512 registerLoadedResult = _mm512_add_ps(registerLoadedA, registerLoadedC);

		float * result = static_cast<float*>(_aligned_malloc(GetVectorSize() * sizeof(float), GetAlignment()));

		for (int i = 0; i < GetVectorSize(); i++)
		{
			result[i] = -1;
		}

		_mm512_store_ps(result, registerLoadedResult);

		float *expectedResult = new float[GetVectorSize()]{ -3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3 };

		for (int i = 0; i < 16; i++)
		{
			EXPECT_DOUBLE_EQ(expectedResult[i], result[i]) << "Index: " << i;
		}
	}
}

TEST_F(FixtureIntrinsics, MulSubTest)
{
	auto systemProperties = SystemProperties::GetInstance();
	if (systemProperties.CPUSupportsAVX512())
	{
		__m512 registerLoadedA = _mm512_load_ps(GetVectorA());
		__m512 registerLoadedB = _mm512_load_ps(GetVectorB());
		__m512 registerLoadedMulResult = _mm512_mul_ps(registerLoadedA, registerLoadedB);
		__m512 registerLoadedSubResult = _mm512_sub_ps(registerLoadedMulResult, registerLoadedB);
		__m512 registerLoadedMulSubResult = _mm512_fmsub_ps(registerLoadedA, registerLoadedB, registerLoadedB);
		float * resultMulSub = static_cast<float*>(_aligned_malloc(GetVectorSize() * sizeof(float), GetAlignment()));
		float * resultSub = static_cast<float*>(_aligned_malloc(GetVectorSize() * sizeof(float), GetAlignment()));
		_mm512_store_ps(resultMulSub, registerLoadedMulSubResult);
		_mm512_store_ps(resultSub, registerLoadedSubResult);
		for (int i = 0; i < 16; i++)
		{
			EXPECT_DOUBLE_EQ(resultMulSub[i], resultSub[i]) << "Index: " << i;
		}
	}
}