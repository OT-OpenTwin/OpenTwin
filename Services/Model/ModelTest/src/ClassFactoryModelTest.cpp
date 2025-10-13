#include "gtest/gtest.h"
#include "ClassFactoryModel.h"

TEST(CADModelEntities, BlockEntityFound)
{
	ClassFactoryModel handler;
	EntityBase* entity = handler->createEntity("EntityBlockPython");
	EXPECT_NE(entity, nullptr);
}

TEST(CADModelEntities, ModelEntityNotFound)
{
	ClassFactoryHandler* handler = new ClassFactoryBlock();
	EntityBase* entity = handler->createEntity("EntityAnnotation");
	EXPECT_EQ(entity, nullptr);
}

TEST(CADModelEntities, ModelEntityFound)
{
	ClassFactoryHandler* cadHandler = new ClassFactoryBlock();
	ClassFactoryHandler* modelHandler = new ClassFactory();
	cadHandler->SetNextHandler(modelHandler);
	EntityBase* entity = cadHandler->createEntity("EntityAnnotation");
	EXPECT_NE(entity, nullptr);
}