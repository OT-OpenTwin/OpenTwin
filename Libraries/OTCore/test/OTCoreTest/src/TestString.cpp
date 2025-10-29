// @otlicense

#include "gtest/gtest.h"
#include "OTCore/String.h"
#include "OTCore/ContainerHelper.h"
#include <string>

TEST(StringTests, ToLower) {
	const std::string txt = "AaBb01\"xX\\";
	EXPECT_EQ("aabb01\"xx\\", ot::String::toLower(txt));
}

TEST(StringTests, ToUpper) {
	const std::string txt = "AaBb01\"xX\\";
	EXPECT_EQ("AABB01\"XX\\", ot::String::toUpper(txt));
}

TEST(StringTests, FindOccurance) {
	const std::string txt = "A|BB|CCC";

	EXPECT_EQ(std::string::npos, ot::String::findOccurance(txt, '|', 0));
	EXPECT_EQ(1, ot::String::findOccurance(txt, '|', 1));
	EXPECT_EQ(4, ot::String::findOccurance(txt, '|', 2));
	EXPECT_EQ(std::string::npos, ot::String::findOccurance(txt, '|', 3));
}

TEST(StringTests, SplitByCharacter_SkipEmpty) {
	const std::string txt = "A|BB||CCC";

	const std::vector<std::string> result = ot::ContainerHelper::toVector(ot::String::split(txt, '|', true));

	EXPECT_EQ(3, result.size());

	EXPECT_EQ("A", result[0]);
	EXPECT_EQ("BB", result[1]);
	EXPECT_EQ("CCC", result[2]);
}

TEST(StringTests, SplitByCharacter_KeepEmpty) {
	const std::string txt = "A|BB||CCC";

	const std::vector<std::string> result = ot::ContainerHelper::toVector(ot::String::split(txt, '|', false));

	EXPECT_EQ(4, result.size());

	EXPECT_EQ("A", result[0]);
	EXPECT_EQ("BB", result[1]);
	EXPECT_EQ("", result[2]);
	EXPECT_EQ("CCC", result[3]);
}

TEST(StringTests, SplitByString_SkipEmpty) {
	const std::string txt = "AsplitBBsplitsplitCCC";

	const std::vector<std::string> result = ot::ContainerHelper::toVector(ot::String::split(txt, "split", true));

	EXPECT_EQ(3, result.size());

	EXPECT_EQ("A", result[0]);
	EXPECT_EQ("BB", result[1]);
	EXPECT_EQ("CCC", result[2]);
}

TEST(StringTests, SplitByString_KeepEmpty) {
	const std::string txt = "AsplitBBsplitsplitCCC";

	const std::vector<std::string> result = ot::ContainerHelper::toVector(ot::String::split(txt, "split", false));

	EXPECT_EQ(4, result.size());

	EXPECT_EQ("A", result[0]);
	EXPECT_EQ("BB", result[1]);
	EXPECT_EQ("", result[2]);
	EXPECT_EQ("CCC", result[3]);
}

TEST(StringTests, ReplaceChar) {
	const std::string txt = "AaBbCc";
	EXPECT_EQ("AaXbCc", ot::String::replace(txt, 'B', 'X'));
	EXPECT_EQ("AaYbCc", ot::String::replace(txt, 'B', 'Y'));

	std::string txt2 = txt;
	ot::String::replaced(txt2, 'D', 'Y');
	EXPECT_EQ("AaBbCc", txt2);
}

TEST(StringTests, ReplaceStr) {
	const std::string txt = "AaBbCc";
	EXPECT_EQ("AaBbBbCc", ot::String::replace(txt, "Bb", "BbBb"));
	EXPECT_EQ("AaDdDdCc", ot::String::replace(txt, "Bb", "DdDd"));
	EXPECT_EQ("AaBbCc", ot::String::replace(txt, "Dd", "DdDd"));
}

TEST(StringTests, FillPrefix) {
	const std::string txt = "Test";
	EXPECT_EQ("......Test", ot::String::fillPrefix(txt, 10, '.'));
}

TEST(StringTests, FillSuffix) {
	const std::string txt = "Test";
	EXPECT_EQ("Test......", ot::String::fillSuffix(txt, 10, '.'));
}

TEST(StringTests, RemovePrefix) {
	const std::string txt = " \t \nT e\ts\nt\n \t ";
	EXPECT_EQ("T e\ts\nt\n \t ", ot::String::removePrefix(txt, " \t\n"));
}

TEST(StringTests, RemoveSuffix) {
	const std::string txt = " \t \nT e\ts\nt\n \t ";
	EXPECT_EQ(" \t \nT e\ts\nt", ot::String::removeSuffix(txt, " \t\n"));
}

TEST(StringTests, RemovePrefixSuffix) {
	const std::string txt = " \t \nT e\ts\nt\n \t ";
	EXPECT_EQ("T e\ts\nt", ot::String::removePrefixSuffix(txt, " \t\n"));
}

TEST(StringTests, CStringCopy) {
	const std::string txt = "Test";
	EXPECT_EQ(std::string("Test"), ot::String::getCStringCopy(txt));
}

TEST(StringTests, ToNumber_Int) {
	bool failed = false;

	// Valid cases
	int num = ot::String::toNumber<int>("123", failed);
	EXPECT_FALSE(failed);
	EXPECT_EQ(123, num);

	num = ot::String::toNumber<int>(" 123 ", failed);
	EXPECT_FALSE(failed);
	EXPECT_EQ(123, num);

	num = ot::String::toNumber<int>("+123", failed);
	EXPECT_FALSE(failed);
	EXPECT_EQ(123, num);

	num = ot::String::toNumber<int>("-123", failed);
	EXPECT_FALSE(failed);
	EXPECT_EQ(-123, num);

	num = ot::String::toNumber<int>("0000123", failed);
	EXPECT_FALSE(failed);
	EXPECT_EQ(123, num);

	num = ot::String::toNumber<int>(std::to_string(std::numeric_limits<int>::max()), failed);
	EXPECT_FALSE(failed);
	EXPECT_EQ(std::numeric_limits<int>::max(), num);

	num = ot::String::toNumber<int>(std::to_string(std::numeric_limits<int>::min()), failed);
	EXPECT_FALSE(failed);
	EXPECT_EQ(std::numeric_limits<int>::min(), num);

	// Invalid cases
	const std::vector<std::string> invalidInputs = {
		"",           // Empty
		"   ",        // Spaces only
		"abc",        // Letters
		"123abc",     // Alphanumeric
		"12.3",       // Float
		"1e3",        // Scientific notation
		"1.0",        // Still float
		"--123",      // Double minus
		"++123",      // Double plus
		"99999999999999999999999999999", // Overflow
		"-99999999999999999999999999999", // Underflow
		"NaN",        // Not a number
		"INF",        // Infinity
		"0xFF",       // Hex
		"123.0",      // Still float
		".123",       // Malformed float
		"123.",       // Malformed float
		"e10",        // Starts with exponent
	};

	for (const auto& str : invalidInputs) {
		num = ot::String::toNumber<int>(str, failed);
		EXPECT_TRUE(failed) << "Input should have failed: '" << str << "'";
	}
}

TEST(StringTests, ToNumber_Double) {
	bool failed = false;
	
	// Valid cases
	double num = ot::String::toNumber<double>("123", failed);
	EXPECT_FALSE(failed);
	EXPECT_EQ(123., num);

	num = ot::String::toNumber<double>("123.4", failed);
	EXPECT_FALSE(failed);
	EXPECT_EQ(123.4, num);

	num = ot::String::toNumber<double>(" 123.4 ", failed);
	EXPECT_FALSE(failed);
	EXPECT_EQ(123.4, num);

	num = ot::String::toNumber<double>("+123.4", failed);
	EXPECT_FALSE(failed);
	EXPECT_EQ(123.4, num);

	num = ot::String::toNumber<double>("-123.4", failed);
	EXPECT_FALSE(failed);
	EXPECT_EQ(-123.4, num);

	num = ot::String::toNumber<double>("0.0", failed);
	EXPECT_FALSE(failed);
	EXPECT_EQ(0.0, num);

	num = ot::String::toNumber<double>(".5", failed);
	EXPECT_FALSE(failed);
	EXPECT_EQ(0.5, num);

	num = ot::String::toNumber<double>("5.", failed);
	EXPECT_FALSE(failed);
	EXPECT_EQ(5.0, num);

	num = ot::String::toNumber<double>("1e3", failed);
	EXPECT_FALSE(failed);
	EXPECT_EQ(1000.0, num);

	num = ot::String::toNumber<double>("1E3", failed);
	EXPECT_FALSE(failed);
	EXPECT_EQ(1000.0, num);

	num = ot::String::toNumber<double>("1.23e2", failed);
	EXPECT_FALSE(failed);
	EXPECT_EQ(123.0, num);

	num = ot::String::toNumber<double>("-1.23e-2", failed);
	EXPECT_FALSE(failed);
	EXPECT_EQ(-0.0123, num);

	num = ot::String::toNumber<double>("000123.4500", failed);
	EXPECT_FALSE(failed);
	EXPECT_EQ(123.45, num);

	num = ot::String::toNumber<double>("+0.000", failed);
	EXPECT_FALSE(failed);
	EXPECT_EQ(0.0, num);

	num = ot::String::toNumber<double>("0xFF", failed);
	EXPECT_FALSE(failed);
	EXPECT_EQ(255.0, num);

	// Invalid cases
	const std::vector<std::string> invalidInputs = {
		"",           // Empty string
		"   ",        // Only spaces
		"abc",        // Letters
		"123abc",     // Mixed digits and letters
		"12.3.4",     // Multiple dots
		"--123",      // Double minus
		"++123",      // Double plus
		"1e",         // Incomplete exponent
		"1e+",        // Incomplete exponent with sign
		"1e- ",       // Exponent with space
		"1.2e3.4",    // Malformed exponent
		"1,234.5",    // Comma as thousand separator (not valid in C++ parsing)
		"NaN",        // Not a number (depends on if you want to allow it)
		"INF",        // Infinity
		"-.e5",       // Malformed decimal + exponent
		"e123",       // Starts with exponent
	};

	for (const std::string& str : invalidInputs) {
		num = ot::String::toNumber<double>(str, failed);
		EXPECT_TRUE(failed) << "Input should have failed: '" << str << "'";
	}
}

TEST(StringTests, NumberToHex) {
	EXPECT_EQ("01", ot::String::numberToHexString(1, '0', 2));
	EXPECT_EQ("ff", ot::String::numberToHexString(255, '0', 2));
	EXPECT_EQ("00ff", ot::String::numberToHexString(255, '0', 4));
}

TEST(StringTests, EvaluateEscapeCharacters) {
	const std::string src = "Test\\nString\"\\\"with some escape\\bcharacters";

	std::string result = ot::String::evaluateEscapeCharacters(src);
	EXPECT_EQ(result, "Test\nString\"\"with some escape\bcharacters");
}

TEST(StringTests, StringSmartSplit_EvaluateEscapeKeepEmpty) {
	const std::string src = "Test\nString \"containing\nsubstring with\\nsome escape\\\"\"\n";

	std::vector<std::string> result = ot::ContainerHelper::toVector(ot::String::smartSplit(src, "\n", true, false)); // evaluate escape, keep empty
	
	// Check result size
	EXPECT_EQ(result.size(), 3);
	
	// Check result content
	EXPECT_EQ(result[0], "Test");
	EXPECT_EQ(result[1], "String \"containing\nsubstring with\nsome escape\"\"");
	EXPECT_EQ(result[2], "");
}

TEST(StringTests, StringSmartSplit_EvaluateEscapeSkipEmpty) {
	const std::string src = "Test\nString \"containing\nsubstring with\\nsome escape\\\"\"\n";

	std::vector<std::string> result = ot::ContainerHelper::toVector(ot::String::smartSplit(src, "\n", true, true)); // evaluate escape, skip empty

	// Check result size
	EXPECT_EQ(result.size(), 2);
	
	// Check result content
	EXPECT_EQ(result[0], "Test");
	EXPECT_EQ(result[1], "String \"containing\nsubstring with\nsome escape\"\"");
}

TEST(StringTests, StringSmartSplit_NoEscapeKeepEmpty) {
	const std::string src = "Test\nString \"containing\nsubstring\\\" with\\nsome escape\\\"\"\n";

	std::vector<std::string> result = ot::ContainerHelper::toVector(ot::String::smartSplit(src, "\n", false, false)); // no escape, keep empty
	
	// Check result size
	EXPECT_EQ(result.size(), 3);

	// Check result content
	EXPECT_EQ(result[0], "Test");
	EXPECT_EQ(result[1], "String \"containing\nsubstring\\\" with\\nsome escape\\\"\"");
	EXPECT_EQ(result[2], "");
}

TEST(StringTests, StringSmartSplit_NoEscapeSkipEmpty) {
	const std::string src = "Test\nString \"containing\nsubstring\\\" with\\nsome escape\\\"\"\n";

	std::vector<std::string> result = ot::ContainerHelper::toVector(ot::String::smartSplit(src, "\n", false, true)); // no escape, skip empty

	// Check result size
	EXPECT_EQ(result.size(), 2);

	// Check result content
	EXPECT_EQ(result[0], "Test");
	EXPECT_EQ(result[1], "String \"containing\nsubstring\\\" with\\nsome escape\\\"\"");
}

TEST(StringTests, Hex_Basic) {
	const std::string input = "ABC";
	std::string result = ot::String::toHex(input);

	// Each byte 2 hex chars: 'A' = 0x41, 'B' = 0x42, 'C' = 0x43
	EXPECT_EQ(result, "414243");
}

TEST(StringTests, Hex_WithUmlauts1) {
	// "Ä" in UTF-8 = 0xC3 0x84
	const std::string input = u8"Ä";
	std::string result = ot::String::toHex(input);

	EXPECT_EQ(result, "c384");
}

TEST(StringTests, Hex_WithUmlauts2) {
	// "Überprüfung" in UTF-8 = 0xC3 0x9C 0x62 0x65 0x72 0x70 0x72 0xFC 0x66 0x75 0x6E 0x67
	const std::string input = u8"Überprüfung";
	std::string result = ot::String::toHex(input);

	EXPECT_EQ(result, "c39c6265727072c3bc66756e67");
}

TEST(StringTests, Hex_Empty) {
	const std::string input = u8"Überprüfung";
	const std::string result = ot::String::toHex(input);
	const std::string inverse = ot::String::fromHex(result);
	EXPECT_EQ(input, inverse);
}

TEST(StringTests, Hex_AllBytes) {
	std::string input;
	input.resize(256);
	for (size_t i = 0; i < 256; ++i) {
		input[i] = static_cast<char>(i);
	}
	const std::string result = ot::String::toHex(input);
	const std::string expected = "000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f202122232425262728292a2b2c2d2e2f303132333435363738393a3b3c3d3e3f404142434445464748494a4b4c4d4e4f505152535455565758595a5b5c5d5e5f606162636465666768696a6b6c6d6e6f707172737475767778797a7b7c7d7e7f808182838485868788898a8b8c8d8e8f909192939495969798999a9b9c9d9e9fa0a1a2a3a4a5a6a7a8a9aaabacadaeafb0b1b2b3b4b5b6b7b8b9babbbcbdbebfc0c1c2c3c4c5c6c7c8c9cacbcccdcecfd0d1d2d3d4d5d6d7d8d9dadbdcdddedfe0e1e2e3e4e5e6e7e8e9eaebecedeeeff0f1f2f3f4f5f6f7f8f9fafbfcfdfeff";
	EXPECT_EQ(expected, result);
}

TEST(StringTests, Hex_Inverse1) {
	const std::string input;
	std::string result = ot::String::toHex(input);

	EXPECT_TRUE(result.empty());
}

TEST(StringTests, Hex_Inverse2) {
	std::string input;
	input.resize(256);
	for (size_t i = 0; i < 256; ++i) {
		input[i] = static_cast<char>(i);
	}
	const std::string encoded = ot::String::toHex(input);
	const std::string decoded = ot::String::fromHex(encoded);

	EXPECT_EQ(input, decoded);
}

TEST(StringTests, Base64Url_Basic) {
	const std::string input = "ABC";
	std::string result = ot::String::toBase64Url(input);

	EXPECT_EQ(result, "QUJD");
}

TEST(StringTests, Base64Url_WithUmlauts1) {
	// "Ä" in UTF-8 = 0xC3 0x84
	const std::string input = u8"Ä";
	std::string result = ot::String::toBase64Url(input);

	EXPECT_EQ(result, "w4Q");
}

TEST(StringTests, Base64Url_WithUmlauts2) {
	// "Ä" in UTF-8 = 0xC3 0x84
	const std::string input = u8"Überprüfung";
	std::string result = ot::String::toBase64Url(input);
	const std::string expected = "w5xiZXJwcsO8ZnVuZw";
	EXPECT_EQ(result, expected);
}

TEST(StringTests, Base64Url_Empty) {
	const std::string input;
	std::string result = ot::String::toBase64Url(input);

	EXPECT_TRUE(result.empty());
}

TEST(StringTests, Base64Url_AllBytes) {
	std::string input;
	input.resize(256);
	for (size_t i = 0; i < 256; ++i) {
		input[i] = static_cast<char>(i);
	}
	const std::string result = ot::String::toBase64Url(input);
	const std::string expected = "AAECAwQFBgcICQoLDA0ODxAREhMUFRYXGBkaGxwdHh8gISIjJCUmJygpKissLS4vMDEyMzQ1Njc4OTo7PD0-P0BBQkNERUZHSElKS0xNTk9QUVJTVFVWV1hZWltcXV5fYGFiY2RlZmdoaWprbG1ub3BxcnN0dXZ3eHl6e3x9fn-AgYKDhIWGh4iJiouMjY6PkJGSk5SVlpeYmZqbnJ2en6ChoqOkpaanqKmqq6ytrq-wsbKztLW2t7i5uru8vb6_wMHCw8TFxsfIycrLzM3Oz9DR0tPU1dbX2Nna29zd3t_g4eLj5OXm5-jp6uvs7e7v8PHy8_T19vf4-fr7_P3-_w";

	EXPECT_EQ(expected, result);
}

TEST(StringTests, Base64Url_Inverse1) {
	std::string input = u8"Überprüfung";
	const std::string encoded = ot::String::toBase64Url(input);
	const std::string decoded = ot::String::fromBase64Url(encoded);
	EXPECT_EQ(input, decoded);
}

TEST(StringTests, Base64Url_Inverse2) {
	std::string input;
	input.resize(256);
	for (size_t i = 0; i < 256; ++i) {
		input[i] = static_cast<char>(i);
	}
	const std::string encoded = ot::String::toBase64Url(input);
	const std::string decoded = ot::String::fromBase64Url(encoded);
	EXPECT_EQ(input, decoded);
}