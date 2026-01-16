// @otlicense
// File: TestPythonManifestUpdate.cpp
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