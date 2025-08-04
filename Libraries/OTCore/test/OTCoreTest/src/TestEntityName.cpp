#include "gtest/gtest.h"
#include "OTSystem/Exception.h"
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

TEST(EntityNameTest, EntityChildOf_True) {
	const std::string parentName = "Root/FirstLvl/SecondLvl/ThirdLvl";
	const std::string entityName = "Root/FirstLvl/SecondLvl/ThirdLvl/Entity";
	EXPECT_TRUE(ot::EntityName::isChildOf(entityName, parentName));
}

TEST(EntityNameTest, EntityChildOf_False) {
	const std::string parentName = "Root/FirstLvl/OtherLvl/ThirdLvl";
	const std::string entityName = "Root/FirstLvl/SecondLvl/ThirdLvl/Entity";
	EXPECT_FALSE(ot::EntityName::isChildOf(entityName, parentName));
}

TEST(EntityNameTest, EntityTopoLevel_Zero) {
	const std::string entityPath = "Root";
	EXPECT_EQ(0, ot::EntityName::getTopologyLevel(entityPath));
}

TEST(EntityNameTest, EntityTopoLevel_One) {
	const std::string entityPath = "Root/FirstLevel";
	EXPECT_EQ(1, ot::EntityName::getTopologyLevel(entityPath));
}

TEST(EntityNameTest, EntityTopoLevel_Two) {
	const std::string entityPath = "Root/FirstLvl/SecondLvl";
	EXPECT_EQ(2, ot::EntityName::getTopologyLevel(entityPath));
}

TEST(EntityNameTest, EntityReparentName_EqLength) {
	const std::string entityPath = "Root/OldParent/Entity";
	const std::string parentPath = "Root/NewParent";
	const std::string expected = "Root/NewParent/Entity";
	EXPECT_EQ(expected, ot::EntityName::changeParentPath(entityPath, parentPath));
}

TEST(EntityNameTest, EntityReparentName_DifLength) {
	const std::string entityPath = "Root/OldParent/Entity";
	const std::string parentPath = "Root/NewParentPath";
	const std::string expected = "Root/NewParentPath/Entity";
	EXPECT_EQ(expected, ot::EntityName::changeParentPath(entityPath, parentPath));
}

TEST(EntityNameTest, EntityReparentName_SmallerTopo) {
	const std::string entityPath = "Root/OldParent/Entity";
	const std::string parentPath = "NewParent";
	const std::string expected = "NewParent/OldParent/Entity";
	EXPECT_EQ(expected, ot::EntityName::changeParentPath(entityPath, parentPath));
}

/*TEST(EntityNameTest, EntityReparentName_GreaterTopo) {
	const std::string entityPath = "Root/OldParent/Entity";
	const std::string parentPath = "Root/NewParent/OtherParent";
	EXPECT_THROW(ot::EntityName::changeParentPath(entityPath, parentPath), ot::OutOfBoundsException);
}*/