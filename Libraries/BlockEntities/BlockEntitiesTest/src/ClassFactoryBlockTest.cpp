#include "gtest/gtest.h"
#include "EntityFactory.h"

TEST(BlockEntities, BlockEntityFound)
{
	EntityBase* entity = EntityFactory::instance().create("EntityBlockPython");
	EXPECT_NE(entity, nullptr);
}