// @otlicense

#include <gtest/gtest.h>
#include "EntityFactory.h"

TEST(CADModelEntities, CADEntityFound) 
{
	EntityBase* entity = EntityFactory::instance().create("EntityGeometry");
	EXPECT_NE(entity, nullptr);
}
