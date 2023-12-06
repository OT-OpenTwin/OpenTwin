#include "gtest/gtest.h"
#include "OTCore/JSON.h"
#include "OTCore/ReturnMessage.h"
#include <string>

TEST(rJSONTests, JSONToReturnMessage)
{
	ot::ReturnMessage expected(ot::ReturnMessage::ReturnMessageStatus::Ok, "This was ok");
	ot::JsonDocument doc;
	ot::JsonValue val;
	val.SetObject();
	expected.addToJsonObject(val, doc.GetAllocator());
	doc.AddMember("RetMessage", val, doc.GetAllocator());
	const std::string expectedString = doc.toJson();
	
	ot::ReturnMessage result;
	result.setFromJsonObject(ot::json::getObject(doc, "RetMessage"));
	EXPECT_EQ(expected.getStatus(), result.getStatus());
	EXPECT_EQ(expected.getWhat(), result.getWhat());
}


TEST(rJSONTests, ReturnMessageToJSON)
{
	ot::ReturnMessage returnMessage(ot::ReturnMessage::ReturnMessageStatus::Ok, "This was ok");
	ot::JsonDocument doc;
	ot::JsonValue val;
	val.SetObject();
	returnMessage.addToJsonObject(val, doc.GetAllocator());
	EXPECT_TRUE(val.HasMember("Status"));
	EXPECT_TRUE(val.HasMember("What"));
}