#include "ClassFactoryBlock.h"
#include "gtest/gtest.h"
#include "ClassFactory.h"

TEST(BlockEntities, BlockEntityFound)
{
	ClassFactoryHandler* handler = new ClassFactoryBlock();
	EntityBase* entity = handler->CreateEntity("EntityBlockPython");
	EXPECT_NE(entity, nullptr);
}

TEST(BlockEntities, ModelEntityNotFound)
{
	ClassFactoryHandler* handler = new ClassFactoryBlock();
	EntityBase* entity = handler->CreateEntity("EntityAnnotation");
	EXPECT_EQ(entity, nullptr);
}

TEST(BlockEntities, ModelEntityFound)
{
	ClassFactoryHandler* cadHandler = new ClassFactoryBlock();
	ClassFactoryHandler* modelHandler = new ClassFactory();
	cadHandler->SetNextHandler(modelHandler);
	EntityBase* entity = cadHandler->CreateEntity("EntityAnnotation");
	EXPECT_NE(entity, nullptr);
}