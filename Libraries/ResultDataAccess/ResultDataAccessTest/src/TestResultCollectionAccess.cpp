#include <gtest/gtest.h>
#include "FixtureResultCollectionExtender.h"
#include "EntityContainer.h"
#include "DataBase.h"
TEST_F(FixtureResultCollectionExtender, init)
{
	EntityContainer c(0, nullptr, nullptr, nullptr, nullptr, "");
	ASSERT_EQ(nullptr, c.getParent());
	DataBase b();

}
