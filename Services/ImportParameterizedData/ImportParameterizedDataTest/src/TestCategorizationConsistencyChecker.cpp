// @otlicense
// File: TestCategorizationConsistencyChecker.cpp
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

#include <gtest/gtest.h>
#include "FixtureCategorizationConsistencyChecker.h"
#include "DataCategorizationConsistencyChecker.h"

TEST_F(FixtureCategorizationConsistencyChecker, MSMDMissesParameter) 
{
	DataCategorizationConsistencyChecker checker;
	bool isValid = checker.isValidAllMSMDHaveParameterAndQuantities(GetFailureCollection_MissingParameter());
	EXPECT_FALSE(isValid);
}

TEST_F(FixtureCategorizationConsistencyChecker, MSMDMissesQuantity)
{
	DataCategorizationConsistencyChecker checker;
	bool isValid = checker.isValidAllMSMDHaveParameterAndQuantities(GetFailureCollection_MissingQuantity());
	EXPECT_FALSE(isValid);
}

TEST_F(FixtureCategorizationConsistencyChecker, MSMDsHasQuantityAndParameter)
{
	DataCategorizationConsistencyChecker checker;
	bool isValid = checker.isValidAllMSMDHaveParameterAndQuantities(GetCorrectCollection_AllLinksCorrect());
	EXPECT_TRUE(isValid);
}

TEST_F(FixtureCategorizationConsistencyChecker, ParameterAndQuantityUseDifferentTables)
{
	DataCategorizationConsistencyChecker checker;
	bool isValid = checker.isValidAllParameterAndQuantitiesReferenceSameTable(GetFailureCollection_QuantityAndParameterDontShareTable());
	EXPECT_FALSE(isValid);
}

TEST_F(FixtureCategorizationConsistencyChecker, ParameterAndQuantityUseSameTables)
{
	DataCategorizationConsistencyChecker checker;
	bool isValid = checker.isValidAllParameterAndQuantitiesReferenceSameTable(GetCorrectCollection_AllTableNamesCorrect());
	EXPECT_TRUE(isValid);
}