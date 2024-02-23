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

	EXPECT_TRUE(r1.childItems().size() == 2);
	EXPECT_TRUE(r1.childItems().front().itemPath() == "Root/B");
	EXPECT_TRUE(r1.childItems().back().itemPath() == "Root/C");
}