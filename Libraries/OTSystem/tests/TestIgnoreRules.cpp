// @otlicense
// File: TestIgnoreRules.cpp
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
#include "OTSystem/IgnoreRules.h"

namespace ot {
	namespace testHelper {
		IgnoreRules createTestRules() {
			std::string fileContent =
				"# Ignore rules example\n"
				"*.log\n"
				"!important.log\n"
				"build/\n"
				"**/temp/\n"
				"/rootfile.txt\n"
				;

			IgnoreRules rules = IgnoreRules::parseFromText(fileContent);
			return rules;
		}
	}
}

using namespace ot;

TEST(IgnoreRulesTest, Parse) {
	IgnoreRules rules = testHelper::createTestRules();
	EXPECT_EQ(rules.getRules().size(), 5);
}

TEST(IgnoreRulesTest, MatchLiteral) {
	IgnoreRules rules = testHelper::createTestRules();
	EXPECT_TRUE(rules.matches("build/", true));
	EXPECT_FALSE(rules.matches("build/file.txt", false));
	EXPECT_TRUE(rules.matches("rootfile.txt", false));
	EXPECT_FALSE(rules.matches("notrootfile.txt", false));
}

TEST(IgnoreRulesTest, MatchGlob) {
	IgnoreRules rules = testHelper::createTestRules();
	EXPECT_TRUE(rules.matches("error.log", false));
	EXPECT_FALSE(rules.matches("important.log", false));
	EXPECT_FALSE(rules.matches("notes.txt", false));
}

TEST(IgnoreRulesTest, MatchRecursiveGlob) {
	IgnoreRules rules = testHelper::createTestRules();
	EXPECT_TRUE(rules.matches("temp/", true));
	EXPECT_TRUE(rules.matches("subdir/temp/", true));
	EXPECT_TRUE(rules.matches("a/b/c/temp/", true));
	EXPECT_FALSE(rules.matches("temp/file.txt", false));
	EXPECT_FALSE(rules.matches("otherdir/file.txt", false));
}

TEST(IgnoreRulesTest, MatchDirectoryOnly) {
	IgnoreRules rules = testHelper::createTestRules();
	EXPECT_TRUE(rules.matches("build/", true));
	EXPECT_FALSE(rules.matches("build/file.txt", false));
}

TEST(IgnoreRulesTest, Anchored) {
	IgnoreRules rules = testHelper::createTestRules();
	EXPECT_TRUE(rules.matches("rootfile.txt", false));
	EXPECT_FALSE(rules.matches("subdir/rootfile.txt", false));
}

TEST(IgnoreRulesTest, NoMatch) {
	IgnoreRules rules = testHelper::createTestRules();
	EXPECT_FALSE(rules.matches("randomfile.txt", false));
	EXPECT_FALSE(rules.matches("logs/error.txt", false));
	EXPECT_FALSE(rules.matches("buildtemp/", true));
}
