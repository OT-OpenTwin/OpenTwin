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
