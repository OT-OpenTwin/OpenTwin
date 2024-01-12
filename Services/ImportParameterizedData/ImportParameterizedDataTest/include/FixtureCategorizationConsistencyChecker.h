#pragma once
#include <gtest/gtest.h>
#include "MetadataAssemblyData.h"

class FixtureCategorizationConsistencyChecker : public testing::Test
{

public:
	std::map<std::string, MetadataAssemblyData> GetFailureCollection_MissingParameter();
	std::map<std::string, MetadataAssemblyData> GetFailureCollection_MissingQuantity();
	std::map<std::string, MetadataAssemblyData> GetFailureCollection_QuantityAndParameterDontShareTable();
	std::map<std::string, MetadataAssemblyData> GetCorrectCollection_AllLinksCorrect();
	std::map<std::string, MetadataAssemblyData> GetCorrectCollection_AllTableNamesCorrect();
};
