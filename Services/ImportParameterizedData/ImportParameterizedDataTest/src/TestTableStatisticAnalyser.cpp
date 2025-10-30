// @otlicense
// File: TestTableStatisticAnalyser.cpp
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