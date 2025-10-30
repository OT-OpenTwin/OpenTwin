// @otlicense
// File: TestEnvironment.cpp
// 
// License:
// Copyright 2025 by OpenTwin
//  
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
//  
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// @otlicense-end

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