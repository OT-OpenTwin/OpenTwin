#include <gtest/gtest.h>

#include "OTCore/ReturnValues.h"
#include "OTCore/ReturnMessage.h"
#include "OTCore/GenericDataStructVector.h"
#include "OTCore/GenericDataStructSingle.h"
#include "OTCore/GenericDataStructMatrix.h"

ot::ReturnMessage createDefaultMessage()
{
	ot::ReturnValues expectedReturnValues;
	auto expectedValues = new ot::GenericDataStructVector({ 1,2,3,4,5,6 });
	ot::GenericDataStructList expectedValueList({ expectedValues });
	expectedReturnValues.addData("name", { expectedValueList });
	ot::ReturnMessage message(expectedReturnValues);
	return message;
}

TEST(ReturnMessage, Deserialise)
{
	auto message = createDefaultMessage();
	auto actualValues = dynamic_cast<ot::GenericDataStructVector*>(*message.getValues().getValuesByName()["name"].begin());
	const std::string serialised = message.toJson();

	ot::ReturnMessage actualReturnMessage;
	actualReturnMessage = ot::ReturnMessage::fromJson(serialised);

	EXPECT_TRUE(actualReturnMessage.getStatus() == ot::ReturnMessage::ReturnMessageStatus::Ok);
	EXPECT_TRUE(actualReturnMessage.getWhat() == "");
	
	auto& actualReturnValues = actualReturnMessage.getValues();
	auto actualGenValues = actualReturnValues.getValuesByName()["name"];
	auto actualGenValueEntry =	dynamic_cast<ot::GenericDataStructVector*>(*actualGenValues.begin());
	
	EXPECT_EQ(actualGenValueEntry->getValues(), actualValues->getValues());

}

TEST(ReturnMessage, FromJSON) {
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

TEST(ReturnMessage, ToJSON) {
	ot::ReturnMessage returnMessage(ot::ReturnMessage::ReturnMessageStatus::Ok, "This was ok");
	ot::JsonDocument doc;
	ot::JsonValue val;
	val.SetObject();
	returnMessage.addToJsonObject(val, doc.GetAllocator());
	EXPECT_TRUE(val.HasMember("Status"));
	EXPECT_TRUE(val.HasMember("What"));
}