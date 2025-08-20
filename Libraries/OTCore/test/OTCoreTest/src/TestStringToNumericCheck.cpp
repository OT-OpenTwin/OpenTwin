#include <gtest/gtest.h>
#include "OTCore/StringToNumericCheck.h"

TEST(StringToNumericCheck,ScientificNotationAsINT)
{
	
	const std::string value = "0000000E+00";
	
	bool possibleInt64 = ot::StringToNumericCheck::fitsInInt64(value);
	
	EXPECT_FALSE(possibleInt64);

}
