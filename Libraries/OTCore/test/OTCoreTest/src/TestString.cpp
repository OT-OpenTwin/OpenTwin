#include "gtest/gtest.h"
#include "OTCore/String.h"
#include "OTCore/ContainerHelper.h"
#include <string>

TEST(StringTests, StringEscapeTest) {
	std::string src = "Test\\nString\"\\\"with some escape\\bcharacters";

	std::string result = ot::String::evaluateEscapeCharacters(src);
	EXPECT_EQ(result, "Test\nString\"\"with some escape\bcharacters");
}

TEST(StringTest, EntityTopologyName_ShortName)
{
	const std::string fullEntityName = "Root/First_Lvl/Second_Lvl/Third_Lvl/Short_Name";
	std::optional<std::string> subName = ot::String::getEntitySubName(fullEntityName);
	EXPECT_TRUE(subName.has_value());

	const std::string expectedName = "Short_Name";
	EXPECT_EQ(subName.value(), expectedName);
}

TEST(StringTest, EntityTopologyName_Root)
{
	const std::string fullEntityName = "Root/First_Lvl/Second_Lvl/Third_Lvl/Short_Name";
	std::optional<std::string> subName = ot::String::getEntitySubName(fullEntityName,0);
	EXPECT_TRUE(subName.has_value());

	const std::string expectedName = "Root";
	EXPECT_EQ(subName.value(), expectedName);
}

TEST(StringTest, EntityTopologyName_FirstLvl)
{
	const std::string fullEntityName = "Root/First_Lvl/Second_Lvl/Third_Lvl/Short_Name";
	std::optional<std::string> subName = ot::String::getEntitySubName(fullEntityName, 1);
	EXPECT_TRUE(subName.has_value());

	const std::string expectedName = "First_Lvl";
	EXPECT_EQ(subName.value(), expectedName);
}

TEST(StringTest, EntityTopologyName_ThirdLvl)
{
	const std::string fullEntityName = "Root/First_Lvl/Second_Lvl/Third_Lvl/Short_Name";
	std::optional<std::string> subName = ot::String::getEntitySubName(fullEntityName, 3);
	EXPECT_TRUE(subName.has_value());

	const std::string expectedName = "Third_Lvl";
	EXPECT_EQ(subName.value(), expectedName);
}

TEST(StringTest, EntityTopologyName_Empty)
{
	const std::string fullEntityName = "";
	std::optional<std::string> subName = ot::String::getEntitySubName(fullEntityName, 0);
	EXPECT_FALSE(subName.has_value());
}

TEST(StringTest, EntityTopologyName_LvlNotExist)
{
	const std::string fullEntityName = "Root/First_Lvl/Second_Lvl/Third_Lvl/Short_Name";
	std::optional<std::string> subName = ot::String::getEntitySubName(fullEntityName, 5);
	EXPECT_FALSE(subName.has_value());
}

TEST(StringTest, EntityTopologyName_ShortNameByLvl)
{
	const std::string fullEntityName = "Root/First_Lvl/Second_Lvl/Third_Lvl/Short_Name";
	std::optional<std::string> subName = ot::String::getEntitySubName(fullEntityName, 4);
	EXPECT_TRUE(subName.has_value());

	const std::string expectedName = "Short_Name";
	EXPECT_EQ(subName.value(), expectedName);
}

TEST(StringTests, StringSmartSplit) {
	//std::string src = "Test\nString \"containing\nsubstring with\\nsome escape\\\"\"\n";

	//std::vector<std::string> result1 = ot::vectorFromList(ot::String::smartSplit(src, "\n", true, false)); // evaluate escape, keep empty
	//std::vector<std::string> result2 = ot::vectorFromList(ot::String::smartSplit(src, "\n", true, true)); // evaluate escape, skip empty
	//std::vector<std::string> result3 = ot::vectorFromList(ot::String::smartSplit(src, "\n", false, false)); // no escape, keep empty
	//std::vector<std::string> result4 = ot::vectorFromList(ot::String::smartSplit(src, "\n", false, false)); // no escape, skip empty

	//// Check result sizes
	//EXPECT_EQ(result1.size(), 3);
	//EXPECT_EQ(result2.size(), 2);
	//EXPECT_EQ(result3.size(), 2); // 2 since last " results in new quotation
	//EXPECT_EQ(result4.size(), 2);
	//
	//// Check result string 1
	//EXPECT_EQ(result1[0], "Test");
	//EXPECT_EQ(result1[1], "String \"containing\nsubstring with\nsome escape\"\"");
	//EXPECT_EQ(result1[2], "");

	//// Check result string 2
	//EXPECT_EQ(result2[0], "Test");
	//EXPECT_EQ(result2[1], "String \"containing\nsubstring with\nsome escape\"\"");

	//// Check result string 3
	//EXPECT_EQ(result3[0], "Test");
	//EXPECT_EQ(result3[1], "String \"containing\nsubstring with\\nsome escape\\\"\"\n");

	//// Check result string 4
	//EXPECT_EQ(result4[0], "Test");
	//EXPECT_EQ(result4[1], "String \"containing\nsubstring with\\nsome escape\\\"\"\n");
}
