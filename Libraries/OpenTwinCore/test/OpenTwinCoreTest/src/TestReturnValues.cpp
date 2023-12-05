#include <gtest/gtest.h>
#include "OpenTwinCore/ReturnValues.h"


TEST(ReturnValues, Serialize)
{
	ot::ReturnValues values;

	values.addData("name", { 1,2,3,4,5,6 });
	OT_rJSON_createDOC(doc);
	OT_rJSON_createValueObject(object);
	values.addToJsonObject(doc, object);
	std::string temp = ot::rJSON::toJSON(object);
	
	EXPECT_TRUE(ot::rJSON::memberExists(object, "NameList"));
	EXPECT_TRUE(ot::rJSON::memberExists(object, "ValueLists"));
	
}

TEST(ReturnValues, Deserialize)
{
	ot::ReturnValues expected;

	expected.addData("name", { 1,2,3,4,5,6 });
	OT_rJSON_createDOC(doc);
	OT_rJSON_createValueObject(object);
	expected.addToJsonObject(doc, object);
	std::string temp = ot::rJSON::toJSON(object);

	ot::ReturnValues result;
	result.setFromJsonObject(object);

	EXPECT_TRUE(result.getNumberOfEntries() == 1);
	auto valuesByName =	result.getValuesByName();
	EXPECT_TRUE(valuesByName.begin()->first == "name");
	std::list<ot::Variable> values = valuesByName.begin()->second;
	EXPECT_EQ(values.size(), 6);
}
