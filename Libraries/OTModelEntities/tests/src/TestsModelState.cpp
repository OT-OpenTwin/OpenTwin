#include "FixtureModelState.h"

TEST_F(FixtureModelState, PreSwitchStateOnMainBranch)
{
	std::string currentVersion = "4";
	const std::string precedingVersion = getPrecedingVersion(currentVersion);
	EXPECT_EQ(precedingVersion, "3");
}


TEST_F(FixtureModelState, PreSwitchStateWithinOtherBranch)
{
	std::string currentVersion = "3.2.2";
	std::string precedingVersion = getPrecedingVersion(currentVersion);
	EXPECT_EQ(precedingVersion, "3.2.1");

	currentVersion = "3.2.1.1.2";
	precedingVersion = getPrecedingVersion(currentVersion);
	EXPECT_EQ(precedingVersion, "3.2.1.1.1");
}

TEST_F(FixtureModelState, PreSwitchStateAcrossBranches)
{
	std::string currentVersion = "3.2.1";
	std::string precedingVersion = getPrecedingVersion(currentVersion);
	EXPECT_EQ(precedingVersion, "3");

	currentVersion = "3.2.1.1.1";
	precedingVersion = getPrecedingVersion(currentVersion);
	EXPECT_EQ(precedingVersion, "3.2.1");
}

TEST_F(FixtureModelState, PreInvalidSwitchToVersion0)
{
	std::string currentVersion = "1";
	EXPECT_ANY_THROW(getPrecedingVersion(currentVersion));
}

TEST_F(FixtureModelState, PreInvalidVersionFormat)
{
	std::string currentVersion = "1.1";
	EXPECT_ANY_THROW(getPrecedingVersion(currentVersion));
}

TEST_F(FixtureModelState, ProSwitchStateOnMainBranch)
{
	std::string currentVersion = "4";
	std::string targetVersion = "6";

	std::string proceedingVersion = getProceedingVersion(currentVersion, targetVersion);
	EXPECT_EQ(proceedingVersion, "5");

	proceedingVersion = getProceedingVersion(proceedingVersion, targetVersion);
	EXPECT_EQ(proceedingVersion, "6");
	
}


TEST_F(FixtureModelState, ProSwitchStateWithinOtherBranch)
{
	std::string currentVersion = "4.3.2";
	std::string targetVersion = "4.3.4";

	std::string proceedingVersion = getProceedingVersion(currentVersion, targetVersion);
	EXPECT_EQ(proceedingVersion, "4.3.3");
	
	proceedingVersion = getProceedingVersion(proceedingVersion, targetVersion);
	EXPECT_EQ(proceedingVersion, "4.3.4");

}

TEST_F(FixtureModelState, ProSwitchStateAcrossBranches)
{
	std::string currentVersion = "3";
	std::string targetVersion = "3.2.1";
	std::string proceedingVersion = getProceedingVersion(currentVersion, targetVersion);
	EXPECT_EQ(proceedingVersion, "3.2.1");

	currentVersion = "3.2.1";
	targetVersion = "3.2.1.1.1";
	proceedingVersion = getProceedingVersion(currentVersion, targetVersion);
	EXPECT_EQ(proceedingVersion, "3.2.1.1.1");
}

TEST_F(FixtureModelState, ProEqualVersions)
{
	std::string currentVersion = "3.2.1";
	std::string targetVersion = "3.2.1";
	std::string proceedingVersion = getProceedingVersion(currentVersion, targetVersion);
	EXPECT_EQ(proceedingVersion, "3.2.1");
}

