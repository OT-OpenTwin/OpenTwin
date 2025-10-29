// @otlicense

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