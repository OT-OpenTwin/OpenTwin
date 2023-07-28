#include <iostream>
#include "gtest/gtest.h"
#include "OpenTwinCore/Variable.h"
#include <string>

TEST(VariableTest, VariableToJSON)
{
	ot::variable_t var = 5;
	ot::VariableToJSONConverter converter;
	rapidjson::Value result = converter(std::move(var));
	EXPECT_TRUE(result.IsInt());
	EXPECT_EQ(result.GetInt(), std::get<int>(var));
}


TEST(VariableTest, JSONToVariable)
{
	int expected = 5;
	rapidjson::Value var(expected);

	ot::JSONToVariableConverter converter;
	ot::variable_t result = converter(var);
	
	EXPECT_TRUE(std::holds_alternative <int>(result));
	EXPECT_EQ(var.GetInt(), std::get<int>(result));
}



