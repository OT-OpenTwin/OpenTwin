// @otlicense
// File: TestNavigationTreeItem.cpp
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

#include "gtest/gtest.h"

#include "OTGui/NavigationTreeItem.h"

TEST(NavigationTreeItem, Merge) {
	ot::NavigationTreeItem r1("Root");
	ot::NavigationTreeItem B("B");
	r1.addChildItem(B);

	ot::NavigationTreeItem r2("Root");
	ot::NavigationTreeItem C("C");
	r2.addChildItem(C);

	r1.merge(r2);

	//EXPECT_TRUE(r1.childItems().size() == 2);
	//EXPECT_TRUE(r1.childItems().front().itemPath() == "Root/B");
	//EXPECT_TRUE(r1.childItems().back().itemPath() == "Root/C");
}