#include "gtest/gtest.h"
#include "ClassFactoryModel.h"

TEST(CADModelEntities, BlockEntityFound)
{
	ClassFactoryModel handler;
	EntityBase* entity = handler->CreateEntity("EntityBlockPython");
	EXPECT_NE(entity, nullptr);
}

TEST(CADModelEntities, ModelEntityNotFound)
{
	ClassFactoryHandler* handler = new ClassFactoryBlock();
	EntityBase* entity = handler->CreateEntity("EntityAnnotation");
	EXPECT_EQ(entity, nullptr);
}

TEST(CADModelEntities, ModelEntityFound)
{
	ClassFactoryHandler* cadHandler = new ClassFactoryBlock();
	ClassFactoryHandler* modelHandler = new ClassFactory();
	cadHandler->SetNextHandler(modelHandler);
	EntityBase* entity = cadHandler->CreateEntity("EntityAnnotation");
	EXPECT_NE(entity, nullptr);
}