// @otlicense

#include "FixtureTableExtractorCSV.h"


INSTANTIATE_TEST_SUITE_P(TestSomeColumnDelimiter, FixtureTableExtractorCSV, ::testing::Values(
	0,1,2
));
