#include <gtest/gtest.h>
#include "GenericDataBlock.h"

TEST(GenericDataBlock, FillingWithOneValue)
{
	ot::Variable expectedValue(5);
	GenericDataBlock block(1, 1);
	{
		ot::Variable value(5);
		block.setValue(0,0,std::move(value));
	}
	const ot::Variable& actualValue = block.getValue(0, 0);

	EXPECT_EQ(actualValue, expectedValue);
}

TEST(GenericDataBlock, VectorOfVariables)
{
	std::vector<ot::Variable> variables;
	variables.reserve(2);
	{
		EXPECT_NO_THROW(variables.push_back(ot::Variable(5)));
		
	}
}
