#include <gtest/gtest.h>
#include "OTSystem/OperatingSystem.h"

TEST(OperatingSystem, GetCurrentPath) {
  
	auto path = ot::OperatingSystem::getCurrentExecutableDirectory();
	EXPECT_NE(path, "");
}