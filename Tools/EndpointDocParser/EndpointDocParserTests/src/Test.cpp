#include <gtest/gtest.h>

#include "Endpoint.h"

TEST(FirstTest, SampleTest)
{
	Endpoint endpoint;
	ASSERT_TRUE(true);
}

//TEST(FirstTest, defectParameterEntry)
//{
//	std::string capitalPrefix =
//		"//API parmeter: uid";
//	Endpoint parser;
//	parser.parse(capitalPrefix);
//	size_t numberOfParameter =	parser.getParameters().size();
//
//	EXPECT_TRUE(parser.errorLog().size() != 0);
//	EXPECT_EQ(numberOfParameter,0);
//}
//
//TEST(FirstTest, differentAPISpelling)
//{
//	std::string capitalPrefix =
//		"//api parameter: uid";
//		"//api name: CallMe";
//	
//
//	EXPECT_TRUE(parser.errorLog().size() != 0);
//	EXPECT_EQ(numberOfParameter, 0);
//}
