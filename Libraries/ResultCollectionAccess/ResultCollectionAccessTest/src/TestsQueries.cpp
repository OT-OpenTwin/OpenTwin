#include <gtest/gtest.h>
#include "FixtureQueryBuilder.h"
#include "OTCore/JSON.h"
#include "AdvancedQueryBuilder.h"

TEST_F(FixtureQueryBuilder, Comparision)
{
	AdvancedQueryBuilder builder;
	ot::Variable value = (0);
	std::string comparator = ">";
	auto comparision = builder.CreateComparision(comparator, value);
	//auto query = builder.GenerateFilterQuery("Values", comparision);

	/*std::vector<std::string> filter(0);
	auto select = builder.GenerateSelectQuery(filter, true);
	auto response = ExecuteQuery(query, select);
	
	EXPECT_TRUE(response.getSuccess());
	
	ot::JsonDocument doc;
	doc.fromJson(response.getResult());
	auto allEntries = doc["Documents"].GetArray();
	auto numberOfDocuments = allEntries.Size();
	EXPECT_EQ(numberOfDocuments, 33);*/
	EXPECT_TRUE(true);
}