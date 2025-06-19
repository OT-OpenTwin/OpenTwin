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

TEST(StringTests, Replace) {
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
