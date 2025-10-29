// @otlicense

#include <gtest/gtest.h>
#include "EntityFactory.h"

TEST(ClassfactoryTest, EntityFound)
{
	EntityBase* entity = EntityFactory::instance().create("EntityAnnotation");
	EXPECT_NE(entity, nullptr);
}

TEST(ClassfactoryTest, EntityNotFound)
{
	EntityBase* entity = EntityFactory::instance().create("NotExisting");
	EXPECT_EQ(entity, nullptr);
}
