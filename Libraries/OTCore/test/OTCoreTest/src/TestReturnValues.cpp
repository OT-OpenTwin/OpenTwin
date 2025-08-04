#include <gtest/gtest.h>
#include "OTCore/ReturnValues.h"
#include "OTCore/ReturnMessage.h"
#include "OTCore/GenericDataStructVector.h"
#include "OTCore/GenericDataStructSingle.h"
#include "OTCore/GenericDataStructMatrix.h"
/*
TEST(ReturnValues, Serialize)
{
	ot::ReturnValues returnValues;
	ot::GenericDataStructList values({ new ot::GenericDataStructVector({ 1,2,3,4,5,6 }) });
	returnValues.addData("name", { values });
	ot::JsonDocument doc;
	ot::JsonValue object;
	object.SetObject();
	returnValues.addToJsonObject(object,doc.GetAllocator());
	std::string temp = doc.toJson();
	
	EXPECT_TRUE(object.HasMember("NameList"));
	EXPECT_TRUE(object.HasMember("ValueLists"));
	
}

TEST(ReturnValues, DeepCopy)
{
	ot::ReturnValues expected;
	auto expectedGDS = new ot::GenericDataStructVector({ 1,2,3,4,5,6 });
	ot::GenericDataStructList expectedValues({ expectedGDS});
	expected.addData("name", { expectedValues});

	ot::ReturnValues actual(expected);
	EXPECT_EQ(expected.getValuesByName().size(), actual.getValuesByName().size());
	const ot::GenericDataStructList& actualValues = actual.getValuesByName().begin()->second;
	ot::GenericDataStruct* firstEntry = *actualValues.begin();
	ot::GenericDataStructVector* actualGDS = dynamic_cast<ot::GenericDataStructVector*>(firstEntry);
	EXPECT_EQ(actualGDS->getNumberOfEntries(), expectedGDS->getNumberOfEntries());
}

TEST(ReturnValues, Move)
{
	ot::ReturnValues expected;
	auto expectedGDS = new ot::GenericDataStructVector({ 1,2,3,4,5,6 });
	ot::GenericDataStructList expectedValues({ expectedGDS });
	expected.addData("name", { expectedValues });

	ot::ReturnValues actual(std::move(expected));
	EXPECT_NE(expected.getValuesByName().size(), actual.getValuesByName().size());
	const ot::GenericDataStructList& actualValues = actual.getValuesByName().begin()->second;
	ot::GenericDataStructVector* actualGDS = dynamic_cast<ot::GenericDataStructVector*>(*actualValues.begin());
	EXPECT_EQ(actualGDS->getNumberOfEntries(), 6);
}


TEST(ReturnValues, Deserialize)
{
	ot::ReturnValues expected;
	ot::GenericDataStructList expectedValues({ new ot::GenericDataStructVector({ 1,2,3,4,5,6 }) });
	expected.addData("name", { expectedValues });
	ot::JsonDocument(doc);
	ot::JsonObject (object);
	
	expected.addToJsonObject(object, doc.GetAllocator());
	doc.AddMember("Value", object, doc.GetAllocator());
	std::string temp = doc.toJson();

	ot::ReturnValues result;
	result.setFromJsonObject(ot::json::getObject(doc,"Value"));

	EXPECT_TRUE(result.getNumberOfEntries() == 1);
	auto valuesByName =	result.getValuesByName();
	EXPECT_TRUE(valuesByName.begin()->first == "name");
	auto& genericDataStruct= valuesByName.begin()->second;
	auto values = dynamic_cast<ot::GenericDataStructVector*>(*genericDataStruct.begin());
	EXPECT_EQ(values->getNumberOfEntries(), 6);
}

TEST(ReturnValues, DeserializeFromMessage)
{
	ot::ReturnValues expectedValues;
	ot::GenericDataStructVector* expected(new ot::GenericDataStructVector({ 1,2,3,4,5,6 }));
	expectedValues.addData("name", { expected });
	ot::ReturnMessage message(expectedValues);
	
	ot::JsonDocument(doc);
	ot::JsonObject(object);

	message.addToJsonObject(object, doc.GetAllocator());
	doc.AddMember("Message", object, doc.GetAllocator());
	std::string temp = doc.toJson();

	ot::ReturnMessage isMessage;
	isMessage.setFromJsonObject(ot::json::getObject(doc, "Message"));

	auto& isValues= isMessage.getValues();
	EXPECT_TRUE(isValues.getNumberOfEntries() == 1);
	auto valuesByName = isValues.getValuesByName();
	EXPECT_TRUE(valuesByName.begin()->first == "name");
	auto& genericDataStruct = valuesByName.begin()->second;
	auto values = dynamic_cast<ot::GenericDataStructVector*>(*genericDataStruct.begin());
	EXPECT_EQ(values->getNumberOfEntries(), 6);
}

TEST(ReturnValues, Null)
{
	ot::ReturnValues expectedValues;
	ot::GenericDataStructVector* expected(nullptr);
	expectedValues.addData("name", { expected });
	ot::ReturnMessage message(expectedValues);

	ot::JsonDocument(doc);
	ot::JsonObject(object);

	message.addToJsonObject(object, doc.GetAllocator());
	doc.AddMember("Message", object, doc.GetAllocator());
	std::string temp = doc.toJson();

	ot::ReturnMessage isMessage;
	isMessage.setFromJsonObject(ot::json::getObject(doc, "Message"));

	auto& isValues = isMessage.getValues();
	EXPECT_TRUE(isValues.getNumberOfEntries() == 1);
	auto valuesByName = isValues.getValuesByName();
	EXPECT_TRUE(valuesByName.begin()->first == "name");
	auto& genericDataStruct = valuesByName.begin()->second;
	auto value = genericDataStruct.begin();
	EXPECT_TRUE(*value == nullptr);
}
*/