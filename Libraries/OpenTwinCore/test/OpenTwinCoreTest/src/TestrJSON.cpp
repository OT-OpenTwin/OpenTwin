#include "gtest/gtest.h"
#include "OpenTwinCore/rJSON.h"
#include "OpenTwinCore/ReturnMessage.h"
#include <string>


TEST(rJSONTests, ReturnMessageToJSON) 
{
	ot::ReturnMessage returnMessage(ot::ReturnStatus::Ok(), "This was ok");
	auto doc =	ot::rJSON::fromJSON(returnMessage);
	EXPECT_TRUE(ot::rJSON::memberExists(doc, "Status"));
	EXPECT_TRUE(ot::rJSON::memberExists(doc, "What"));
}

TEST(rJSONTests, JSONToReturnMessage)
{
	ot::ReturnMessage expected("OK", "This was ok");
	auto doc = ot::rJSON::fromJSON(expected);
	ot::ReturnMessage result(doc);
	EXPECT_EQ(expected.getStatus(), result.getStatus());
	EXPECT_EQ(expected.getWhat(), result.getWhat());
	EXPECT_EQ(static_cast<std::string>(expected), static_cast<std::string>(result));
}