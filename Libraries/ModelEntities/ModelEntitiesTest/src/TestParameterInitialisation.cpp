#include "FixtureCSVToTableTransformer.h"

INSTANTIATE_TEST_SUITE_P(TestSomeColumnDelimiter, FixtureCSVToTableTransformer, ::testing::Values(
	0, 1, 2
));
