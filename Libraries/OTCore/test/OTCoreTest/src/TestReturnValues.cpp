#include <gtest/gtest.h>
#include "OTCore/ReturnValues.h"
#include "OTCore/ReturnMessage.h"

TEST(ReturnValues, Serialize)
{
	ot::ReturnValues values;

	values.addData("name", { 1,2,3,4,5,6 });
	ot::JsonDocument (doc);
	ot::JsonValue (object);
	object.SetObject();
	values.addToJsonObject(object,doc.GetAllocator());
	std::string temp = doc.toJson();
	
	EXPECT_TRUE(object.HasMember("NameList"));
	EXPECT_TRUE(object.HasMember("ValueLists"));
	
}

TEST(ReturnValues, Deserialize)
{
	ot::ReturnValues expected;

	expected.addData("name", { 1,2,3,4,5,6 });
	ot::JsonDocument(doc);
	ot::JsonValue (object);
	object.SetObject();
	expected.addToJsonObject(object, doc.GetAllocator());
	doc.AddMember("Value", object, doc.GetAllocator());
	std::string temp = doc.toJson();

	ot::ReturnValues result;
	result.setFromJsonObject(ot::json::getObject(doc,"Value"));

	EXPECT_TRUE(result.getNumberOfEntries() == 1);
	auto valuesByName =	result.getValuesByName();
	EXPECT_TRUE(valuesByName.begin()->first == "name");
	std::list<ot::Variable> values = valuesByName.begin()->second;
	EXPECT_EQ(values.size(), 6);
}

TEST(ReturnValues, DeserializeFromMessage)
{
	ot::ReturnValues expectedValues;

	expectedValues.addData("name", { 1,2,3,4,5,6 });
	ot::ReturnMessage message(expectedValues);
	
	ot::JsonDocument(doc);
	ot::JsonValue(object);
	object.SetObject();
	message.addToJsonObject(object, doc.GetAllocator());
	doc.AddMember("Message", object, doc.GetAllocator());
	std::string temp = doc.toJson();

	ot::ReturnMessage isMessage;
	isMessage.setFromJsonObject(ot::json::getObject(doc, "Message"));

	auto& isValues= isMessage.getValues();
	EXPECT_TRUE(isValues.getNumberOfEntries() == 1);
	auto valuesByName = isValues.getValuesByName();
	EXPECT_TRUE(valuesByName.begin()->first == "name");
	std::list<ot::Variable> values = valuesByName.begin()->second;
	EXPECT_EQ(values.size(), 6);
}