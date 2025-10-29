// @otlicense

#pragma once
#include <gtest/gtest.h>
#include "EntityWithDynamicFields.h"

class FixtureEntityWithDynamicFields : public testing::Test
{
public:
	void OrderDocumentsHierarchical(EntityWithDynamicFields& entity);
};
