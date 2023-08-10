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

TEST_F(FixtureTextEncoding, ISO88591ToUTF8)
{
	auto fileContent = ReadFile(ot::TextEncoding::ANSI);

	ot::EncodingConverter_ISO88591ToUTF8 converter;

	std::vector<unsigned char> temp;
	for (auto& ch : fileContent)
	{
		temp.push_back((unsigned char)ch);
	}

	std::string isValue = converter(temp);
	ot::EncodingGuesser guesser;
	auto encoding =	guesser(isValue.c_str(),isValue.size());

	EXPECT_EQ(encoding, ot::TextEncoding::EncodingStandard::UTF8);
}

TEST_F(FixtureTextEncoding, UTF16ToUTF8)
{
	auto fileContent = ReadFile(ot::TextEncoding::UTF16_BEBOM);

	ot::EncodingConverter_UTF16ToUTF8 converter;
	std::string isValue = converter(fileContent);

	ot::EncodingGuesser guesser;
	auto encoding = guesser(isValue.c_str(), isValue.size());

	EXPECT_EQ(encoding, ot::TextEncoding::UTF8);
}

TEST_F(FixtureTextEncoding, EncodingStandardToString)
{
	ot::TextEncoding encoding;
	std::string encodingString =  encoding.getString(ot::TextEncoding::UTF8);
	EXPECT_EQ(encodingString, "UTF-8");
}


INSTANTIATE_TEST_CASE_P(TestAllEncodingIdentifications, FixtureTextEncoding, ::testing::Values(ot::TextEncoding::EncodingStandard::ANSI, ot::TextEncoding::EncodingStandard::UTF8, ot::TextEncoding::EncodingStandard::UTF8_BOM,ot::TextEncoding::EncodingStandard::UTF16_BEBOM, ot::TextEncoding::EncodingStandard::UTF16_LEBOM));
