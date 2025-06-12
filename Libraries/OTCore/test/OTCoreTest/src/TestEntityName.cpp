#include "gtest/gtest.h"
#include "OTCore/EntityName.h"

TEST(EntityNameTest, EntityTopologyName_ShortName) {
	const std::string fullEntityName = "Root/First_Lvl/Second_Lvl/Third_Lvl/Short_Name";
	std::optional<std::string> subName = ot::EntityName::getSubName(fullEntityName);
	EXPECT_TRUE(subName.has_value());

	const std::string expectedName = "Short_Name";
	EXPECT_EQ(subName.value(), expectedName);
}

TEST(EntityNameTest, EntityTopologyName_Root) {
	const std::string fullEntityName = "Root/First_Lvl/Second_Lvl/Third_Lvl/Short_Name";
	std::optional<std::string> subName = ot::EntityName::getSubName(fullEntityName, 0);
	EXPECT_TRUE(subName.has_value());

	const std::string expectedName = "Root";
	EXPECT_EQ(subName.value(), expectedName);
}

TEST(EntityNameTest, EntityTopologyName_FirstLvl) {
	const std::string fullEntityName = "Root/First_Lvl/Second_Lvl/Third_Lvl/Short_Name";
	std::optional<std::string> subName = ot::EntityName::getSubName(fullEntityName, 1);
	EXPECT_TRUE(subName.has_value());

	const std::string expectedName = "First_Lvl";
	EXPECT_EQ(subName.value(), expectedName);
}

TEST(EntityNameTest, EntityTopologyName_ThirdLvl) {
	const std::string fullEntityName = "Root/First_Lvl/Second_Lvl/Third_Lvl/Short_Name";
	std::optional<std::string> subName = ot::EntityName::getSubName(fullEntityName, 3);
	EXPECT_TRUE(subName.has_value());

	const std::string expectedName = "Third_Lvl";
	EXPECT_EQ(subName.value(), expectedName);
}

TEST(EntityNameTest, EntityTopologyName_Empty) {
	const std::string fullEntityName = "";
	std::optional<std::string> subName = ot::EntityName::getSubName(fullEntityName, 0);
	EXPECT_FALSE(subName.has_value());
}

TEST(EntityNameTest, EntityTopologyName_LvlNotExist) {
	const std::string fullEntityName = "Root/First_Lvl/Second_Lvl/Third_Lvl/Short_Name";
	std::optional<std::string> subName = ot::EntityName::getSubName(fullEntityName, 5);
	EXPECT_FALSE(subName.has_value());
}

TEST(EntityNameTest, EntityTopologyName_ShortNameByLvl) {
	const std::string fullEntityName = "Root/First_Lvl/Second_Lvl/Third_Lvl/Short_Name";
	std::optional<std::string> subName = ot::EntityName::getSubName(fullEntityName, 4);
	EXPECT_TRUE(subName.has_value());

	const std::string expectedName = "Short_Name";
	EXPECT_EQ(subName.value(), expectedName);
}