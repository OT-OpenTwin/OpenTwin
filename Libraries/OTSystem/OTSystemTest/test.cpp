#include <iostream>
#include "gtest/gtest.h"
#include "openTwinSystem/OperatingSystem.h"

TEST(OperatingSystem, GetCurrentPath) {
  
	auto path = ot::OperatingSystem::getExecutablePath();
	EXPECT_NE(path, "");
	
}