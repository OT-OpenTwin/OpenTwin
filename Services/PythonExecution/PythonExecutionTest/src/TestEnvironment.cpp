#include <gtest/gtest.h>
#include <OTSystem/OperatingSystem.h>

TEST(Environment, EnvironmentIsSet)
{
	std::string envName = "OPENTWIN_DEV_ROOT";
	const char* deploymentFolder = ot::OperatingSystem::getEnvironmentVariable(envName.c_str());
	EXPECT_TRUE(deploymentFolder != nullptr);
}

TEST(Environment, PATH)
{
	std::string envName = "PATH";
	const char* pathVar=ot::OperatingSystem::getEnvironmentVariable(envName.c_str());

	EXPECT_TRUE(pathVar!= nullptr);
}

TEST(Environment, OT_ALL_DLLD)
{
	std::string envName = "OT_ALL_DLLD";
	const char* deploymentFolder = ot::OperatingSystem::getEnvironmentVariable(envName.c_str());
	EXPECT_TRUE(deploymentFolder != nullptr);
}