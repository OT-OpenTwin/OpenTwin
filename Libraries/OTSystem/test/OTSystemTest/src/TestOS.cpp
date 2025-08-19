#include <gtest/gtest.h>
#include "OTSystem/OperatingSystem.h"

TEST(OperatingSystem, GetCurrentPath) {
  
	auto path = ot::OperatingSystem::getExecutablePath();
	EXPECT_NE(path, "");
}