// @otlicense

#include "FixtureGrid.h"
#include "SystemDependencies/Allignments.h"

#include "MeasuredTargetValues.h"
#include <gtest/gtest.h>



TEST_F(FixtureGrid, TestDefaultAlignment)
{
	std::vector<Alignment> defaultAlignments{ CacheLine64,AVX};
	Alignment activeAlignment;
	for each (auto align in defaultAlignments)
	{
		CreateAlignedGridSP(align,13);
		activeAlignment = GetGrid3DSP()->GetAlignment();
		EXPECT_EQ(activeAlignment, CacheLine64);
	}
}

TEST_F(FixtureGrid, TestAlignmentAVX512Support)
{
	Alignment testedAlignment = AVX512;
	CreateAlignedGridSP(testedAlignment,13);
	Alignment activeAlignment = GetGrid3DSP()->GetAlignment();
	if (AllignmentIsSupported(testedAlignment))
	{
		EXPECT_EQ(activeAlignment, testedAlignment);
	}
	else
	{
		EXPECT_NE(activeAlignment, testedAlignment);
		//GTEST_SKIP();// wird aktuell noch als fail im Tetst explorer gezeigt.
	}
}

TEST_F(FixtureGrid, TestAlignmentAVX2Support)
{
	Alignment testedAlignment = AVX2;
	CreateAlignedGridSP(testedAlignment, 13);
	Alignment activeAlignment = GetGrid3DSP()->GetAlignment();
	if (AllignmentIsSupported(testedAlignment))
	{
		EXPECT_EQ(activeAlignment, testedAlignment);
	}
	else
	{
		EXPECT_NE(activeAlignment, testedAlignment);
		//GTEST_SKIP();// wird aktuell noch als fail im Tetst explorer gezeigt.
	}
}

TEST_P(FixtureGrid, TestPeddingProperties)
{
	auto param = GetParam();
	Alignment alignment = std::get<0>(param);
	int addToDofX = std::get<1>(param);
	if (AllignmentIsSupported(alignment))
	{
		int expectedVectorLength = GetIntrinsicVectorLength(alignment);
		int dofX = expectedVectorLength + addToDofX;
		CreateAlignedGridSP(alignment, dofX);
		EXPECT_EQ(expectedVectorLength, GetGrid3DSP()->GetLengthOfIntrinsicVector());

		int expectedNumberOfVectors = static_cast<int>(std::ceilf(static_cast<float>(dofX) / expectedVectorLength));
		EXPECT_EQ(expectedNumberOfVectors, GetGrid3DSP()->GetNumberOfIntrinsicVectors());
	}
	else
	{

	}
}

TEST_P(FixtureGrid, TestIntrinsicMaskBeginOneVector)
{
	auto param = GetParam();
	Alignment alignment = std::get<0>(param);
	int addToDofX = std::get<1>(param);

	if (AllignmentIsSupported(alignment))
	{
		int dofX = 3 + addToDofX;
		int intrinsicVectorLength = GetIntrinsicVectorLength(alignment);
		CreateAlignedGridSP(alignment, dofX);

		auto masks = GetGrid3DSP()->GetIntrinsicMasks();

		const int  inclVal = _measuredTargetValuesSP.GetIntrinsicIncludeValue();
		const int exclVal = _measuredTargetValuesSP.GetIntrinsicExcludeValue();

		int bitValue = masks->GetMaskBeginNoBeginningExcludeBit(0);
		EXPECT_EQ(bitValue, inclVal);
		bitValue = masks->GetMaskBeginBit(0);
		EXPECT_EQ(bitValue, exclVal);

		for (int i = 1; i < dofX - 1; i++)
		{
			bitValue = masks->GetMaskBeginNoBeginningExcludeBit(i);
			EXPECT_EQ(bitValue, inclVal);
			bitValue = masks->GetMaskBeginBit(i);
			EXPECT_EQ(bitValue, inclVal);
		}
		for (int i = dofX - 1; i < intrinsicVectorLength; i++)
		{
			bitValue = masks->GetMaskBeginNoBeginningExcludeBit(i);
			EXPECT_EQ(bitValue, exclVal);
			bitValue = masks->GetMaskBeginBit(i);
			EXPECT_EQ(bitValue, exclVal);
		}
	}
	else
	{
		EXPECT_NE(1, 2);
		//GTEST_SKIP();// wird aktuell noch als fail im Tetst explorer gezeigt.
	}
}

TEST_P(FixtureGrid, TestIntrinsicMaskBeginTwoVectors)
{
	auto param = GetParam();
	Alignment alignment = std::get<0>(param);
	int addToDofX = std::get<1>(param);

	if (AllignmentIsSupported(alignment))
	{
		int intrinsicVectorLength = GetIntrinsicVectorLength(alignment);
		int dofX = 3 + intrinsicVectorLength + addToDofX;
		CreateAlignedGridSP(alignment, dofX);

		auto masks = GetGrid3DSP()->GetIntrinsicMasks();


		const int  inclVal = _measuredTargetValuesSP.GetIntrinsicIncludeValue();
		const int exclVal = _measuredTargetValuesSP.GetIntrinsicExcludeValue();

		int bitValue = masks->GetMaskBeginNoBeginningExcludeBit(0);
		EXPECT_EQ(bitValue, inclVal);
		bitValue = masks->GetMaskBeginBit(0);
		EXPECT_EQ(bitValue, exclVal);

		for (int i = 1; i < intrinsicVectorLength; i++)
		{
			bitValue = masks->GetMaskBeginNoBeginningExcludeBit(i);
			EXPECT_EQ(bitValue, inclVal);
			bitValue = masks->GetMaskBeginBit(i);
			EXPECT_EQ(bitValue, inclVal);
		}
	}
	else
	{
		EXPECT_NE(1, 2);
		//GTEST_SKIP();// wird aktuell noch als fail im Tetst explorer gezeigt.
	}
}

TEST_P(FixtureGrid, TestIntrinsicMaskEndOneVectors)
{
	auto param = GetParam();
	Alignment alignment = std::get<0>(param);
	int addToDofX = std::get<1>(param);


	if (AllignmentIsSupported(alignment))
	{
		int partialFilling = 3 + addToDofX;
		int intrinsicVectorLength = GetIntrinsicVectorLength(alignment);
		int dofX = partialFilling;
		CreateAlignedGridSP(alignment, dofX);

		auto masks = GetGrid3DSP()->GetIntrinsicMasks();

		const int  inclVal = _measuredTargetValuesSP.GetIntrinsicIncludeValue();
		const int exclVal = _measuredTargetValuesSP.GetIntrinsicExcludeValue();

		int bitVal;
		for (int i = 0; i < intrinsicVectorLength; i++)
		{
			bitVal = masks->GetMaskEndBit(i);
			EXPECT_EQ(bitVal, exclVal);
		}
	}
	else
	{
		EXPECT_NE(1, 2);
		//GTEST_SKIP();// wird aktuell noch als fail im Tetst explorer gezeigt.
	}
}

TEST_P(FixtureGrid, TestIntrinsicMaskEndTwoVectors)
{
	auto param = GetParam();
	Alignment alignment = std::get<0>(param);
	int addToDofX = std::get<1>(param);

	if (AllignmentIsSupported(alignment))
	{
		int partialFilling = 3 + addToDofX;
		int intrinsicVectorLength = GetIntrinsicVectorLength(alignment);
		int dofX = intrinsicVectorLength + partialFilling;
		CreateAlignedGridSP(alignment, dofX);

		auto masks = GetGrid3DSP()->GetIntrinsicMasks();
		const int  inclVal = _measuredTargetValuesSP.GetIntrinsicIncludeValue();
		const int exclVal = _measuredTargetValuesSP.GetIntrinsicExcludeValue();

		int bitVal;
		if (addToDofX == 13)
		{
			for (int i = 0; i < intrinsicVectorLength - 1; i++)
			{
				bitVal = masks->GetMaskEndBit(i);
				EXPECT_EQ(bitVal, inclVal);
			}
		}
		else
		{
			for (int i = 0; i < partialFilling - 1; i++)
			{
				bitVal = masks->GetMaskEndBit(i);
				EXPECT_EQ(bitVal, inclVal);
			}
			for (int i = partialFilling - 1; i < intrinsicVectorLength; i++)
			{
				bitVal = masks->GetMaskEndBit(i);
				EXPECT_EQ(bitVal, exclVal);
			}
		}
	}
	else
	{
		EXPECT_NE(1, 2);
		//GTEST_SKIP();// wird aktuell noch als fail im Tetst explorer gezeigt.
	}
}