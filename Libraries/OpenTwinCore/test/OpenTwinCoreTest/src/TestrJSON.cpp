#include "gtest/gtest.h"
#include "OpenTwinCore/rJSON.h"
#include "OpenTwinCore/ReturnMessage.h"
#include <string>

TEST(rJSONTests, JSONToReturnMessage)
{
	ot::ReturnMessage expected(ot::ReturnMessage::ReturnMessageStatus::Ok, "This was ok");
	OT_rJSON_createDOC(doc);
	OT_rJSON_createValueObject(val);
	expected.addToJsonObject(doc, val);
	const std::string expectedString = ot::rJSON::toJSON(val);
	
	ot::ReturnMessage result;
	result.setFromJsonObject(val);
	EXPECT_EQ(expected.getStatus(), result.getStatus());
	EXPECT_EQ(expected.getWhat(), result.getWhat());
}


TEST(rJSONTests, ReturnMessageToJSON)
{
	ot::ReturnMessage returnMessage(ot::ReturnMessage::ReturnMessageStatus::Ok, "This was ok");
	OT_rJSON_createDOC(doc);
	OT_rJSON_createValueObject(val);
	returnMessage.addToJsonObject(doc, val);
	EXPECT_TRUE(ot::rJSON::memberExists(val, "Status"));
	EXPECT_TRUE(ot::rJSON::memberExists(val, "What"));
}