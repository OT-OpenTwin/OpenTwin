#include "FixtureGrid.h"
#include "FixtureSimulation3D.h"
#include "FixturePECBoundary.h"
#include "SystemDependencies/Allignments.h"

#include <gtest/gtest.h>


INSTANTIATE_TEST_CASE_P(TestAllVectorPaddings, FixtureGrid, ::testing::Values(
	gridSettings{ AVX512,0 },
	gridSettings{ AVX512,1 },
	gridSettings{ AVX512,2 },
	gridSettings{ AVX512,3 },
	gridSettings{ AVX512,4 },
	gridSettings{ AVX512,5 },
	gridSettings{ AVX512,6 },
	gridSettings{ AVX512,7 },
	gridSettings{ AVX512,8 },
	gridSettings{ AVX512,9 },
	gridSettings{ AVX512,10 },
	gridSettings{ AVX512,11 },
	gridSettings{ AVX512,12 },
	gridSettings{ AVX512,13 },

	gridSettings{ AVX2,0 },
	gridSettings{ AVX2,1 },
	gridSettings{ AVX2,2 },
	gridSettings{ AVX2,3 },
	gridSettings{ AVX2,4 },
	gridSettings{ AVX2,5 },

	gridSettings{ AVX2,0 },
	gridSettings{ AVX2,1 },
	gridSettings{ AVX2,2 },
	gridSettings{ AVX2,3 }
));

INSTANTIATE_TEST_CASE_P(TestAll3DLoopSolver, FixtureSimulation3D, ::testing::Values(CacheLine64, AVX2));

INSTANTIATE_TEST_CASE_P(TestAllPECBoundariesForValues, FixturePECBoundary, ::testing::Values(13));