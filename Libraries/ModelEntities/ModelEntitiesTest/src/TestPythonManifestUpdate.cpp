
#include "FixturePythonManifest.h"


TEST_F(FixturePythonManifest, ParserSuccess_CorrectFile)
{
	std::string content = getManifestA();
	auto packageList =	getPackageList(content);
	EXPECT_EQ(packageList.size(), 42);
}

TEST_F(FixturePythonManifest, ParserSuccess_CorrectFileShorter)
{
	std::string content = getManifestB();
	auto packageList = getPackageList(content);
	EXPECT_EQ(packageList.size(), 41);
}

TEST_F(FixturePythonManifest, ParserSuccess_CorrectFile_NoCleanFormat)
{
	std::string content = getManifestD();
	auto packageList = getPackageList(content);
	EXPECT_EQ(packageList.size(), 42);
}

TEST_F(FixturePythonManifest, ParserFailure_IncorrectFile)
{
	std::string content = getManifestE();
	EXPECT_ANY_THROW(getPackageList(content));
}

TEST_F(FixturePythonManifest, Compare_NoChanges)
{
	std::string content1 = getManifestA();
	std::string content2 = getManifestD();
	EXPECT_FALSE(environmentHasChanged(content1, content2));
}

TEST_F(FixturePythonManifest, Compare_DifferentPackage)
{
	std::string content1 = getManifestA();
	std::string content2 = getManifestB();

	EXPECT_TRUE(environmentHasChanged(content1, content2));
}

TEST_F(FixturePythonManifest, Compare_DifferentVersion)
{
	std::string content1 = getManifestA();
	std::string content2 = getManifestC();
	EXPECT_TRUE(environmentHasChanged(content1, content2));
}