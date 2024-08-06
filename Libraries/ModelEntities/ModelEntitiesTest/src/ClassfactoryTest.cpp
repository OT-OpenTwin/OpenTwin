#include <gtest/gtest.h>
#include "ClassFactoryHandler.h"
#include "ClassFactory.h"

TEST(ClassfactoryTest, EntityFound)
{
	ClassFactoryHandler* classFactory = new ClassFactory();
	EntityBase* entity = classFactory->CreateEntity("EntityAnnotation");
	EXPECT_NE(entity, nullptr);
}

TEST(ClassfactoryTest, EntityNotFound)
{
	ClassFactoryHandler* classFactory = new ClassFactory();
	EntityBase* entity = classFactory->CreateEntity("NotExisting");
	EXPECT_EQ(entity, nullptr);
}
