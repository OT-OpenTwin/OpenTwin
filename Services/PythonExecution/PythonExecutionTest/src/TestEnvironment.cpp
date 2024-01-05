#include <gtest/gtest.h>
#include <OTSystem/OperatingSystem.h>

TEST(Environment, EnvironmentIsSet)
{
	std::string envName = "OPENTWIN_DEV_ROOT";
	const char* deploymentFolder = ot::os::getEnvironmentVariable(envName.c_str());
	EXPECT_TRUE(std::string(deploymentFolder).size() != 0);
}

TEST(Environment, PATH)
{
	std::string envName = "DebuggerArgs";
	const char* deploymentFolder = ot::os::getEnvironmentVariable(envName.c_str());
	EXPECT_TRUE(std::string(deploymentFolder).size() != 0);
}

TEST(Environment, OT_ALL_DLLD)
{
	std::string envName = "OT_ALL_DLLD";
	const char* deploymentFolder = ot::os::getEnvironmentVariable(envName.c_str());
	EXPECT_TRUE(std::string(deploymentFolder).size() != 0);
}