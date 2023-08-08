#include "FixtureTextEncoding.h"

TEST_F(FixtureTextEncoding, ReadFileUTF8)
{
	auto fileContent = ReadFile(ot::TextEncoding::UTF8);
	const std::string expectedValue = "Das ist ein UTF-8 Dokument";
	std::string isValue = std::string(fileContent.begin(), fileContent.end());
	EXPECT_EQ(expectedValue, isValue);
}

TEST_P(FixtureTextEncoding, IdentifyEncoding)
{
	auto encoding	= GetParam();
	auto fileContent = ReadFile(encoding);
	ot::EncodingGuesser guesser;
	auto identifiedEncoding  = 	guesser(fileContent);

	EXPECT_EQ(identifiedEncoding, encoding);
}


INSTANTIATE_TEST_CASE_P(TestAllEncodingIdentifications, FixtureTextEncoding, ::testing::Values(ot::TextEncoding::ANSI, ot::TextEncoding::UTF8, ot::TextEncoding::UTF8_BOM,ot::TextEncoding::UTF16_BEBOM, ot::TextEncoding::UTF16_LEBOM));
