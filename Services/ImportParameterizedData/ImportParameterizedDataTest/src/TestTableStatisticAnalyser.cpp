// @otlicense

#include "FixtureTableStatisticAnalyser.h"

TEST_F(FixtureTableStatisticAnalyser, CountOfRows) {
	uint64_t numberOfRows = countRows();
	uint64_t expectedValue = 5;
	EXPECT_EQ(numberOfRows, expectedValue);
}

TEST_F(FixtureTableStatisticAnalyser, CountOfColumns) {
	uint64_t numberOfColumns = countColumns();
	uint64_t expectedValue = 10;
	EXPECT_EQ(numberOfColumns, expectedValue);
}

TEST_F(FixtureTableStatisticAnalyser, CountUniques) {
	int numberOfUniques = countUniquesInColumn(0);
	int expectedValue = 4;
	EXPECT_EQ(numberOfUniques, expectedValue);
}

TEST_F(FixtureTableStatisticAnalyser, CountAllUniques) {
	std::vector<size_t> numberOfUniques = countUniquesInAllColumns();
	std::vector<size_t> expectedValues{4,5,5,5,5,5,3,5,5,3}; 
	for (int i = 0; i < expectedValues.size(); i++)
	{
		EXPECT_EQ(numberOfUniques[i], expectedValues[i]);
	}
}