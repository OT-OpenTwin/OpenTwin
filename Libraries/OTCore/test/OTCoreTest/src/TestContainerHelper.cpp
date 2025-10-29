// @otlicense

#include "gtest/gtest.h"
#include "OTCore/ContainerHelper.h"
#include <string>

TEST(ContainerTests, MapGetValuesAndKeys) {
	std::map<int, char> initialData;
	initialData.insert_or_assign(1, 'A');
	initialData.insert_or_assign(2, 'B');
	initialData.insert_or_assign(3, 'C');

	std::list<int> keyList = ot::ContainerHelper::getKeys(initialData);
	std::list<char> valueList = ot::ContainerHelper::getValues(initialData);

	EXPECT_EQ(keyList.size(), initialData.size());
	EXPECT_EQ(keyList.size(), valueList.size());

	auto it = valueList.begin();
	for (int k : keyList) {
		EXPECT_NE(initialData.find(k), initialData.end());
		EXPECT_EQ(initialData[k], *it);
		it++;
	}
}

TEST(ContainerTests, MapRemoveByValue) {
	std::map<int, char> initialData;
	initialData.insert_or_assign(1, 'A');
	initialData.insert_or_assign(2, 'B');
	initialData.insert_or_assign(3, 'C');

	ot::ContainerHelper::removeByValue(initialData, 'B');
	EXPECT_EQ(initialData.size(), 2);
	EXPECT_EQ(initialData.find(2), initialData.end());

	ot::ContainerHelper::removeByValue(initialData, 'D');
	EXPECT_EQ(initialData.size(), 2);
}

TEST(ContainerTests, MapHasValue) {
	std::map<int, char> initialData;
	initialData.insert_or_assign(1, 'A');
	initialData.insert_or_assign(2, 'B');
	initialData.insert_or_assign(3, 'C');

	EXPECT_TRUE(ot::ContainerHelper::hasValue(initialData, 'A'));
	EXPECT_FALSE(ot::ContainerHelper::hasValue(initialData, 'D'));
}

TEST(ContainerTests, ContainsMap) {
	std::map<int, char> initialData;
	initialData.insert_or_assign(1, 'A');
	initialData.insert_or_assign(2, 'B');
	initialData.insert_or_assign(3, 'C');

	EXPECT_TRUE(ot::ContainerHelper::contains(initialData, 1));
	EXPECT_FALSE(ot::ContainerHelper::contains(initialData, 4));
}

TEST(ContainerTests, ContainsList) {
	std::list<std::string> initialData = { "One", "Two", "Three" };
	EXPECT_TRUE(ot::ContainerHelper::contains(initialData, std::string("One")));
	EXPECT_FALSE(ot::ContainerHelper::contains(initialData, std::string("Four")));
}

TEST(ContainerTests, ContainsVector) {
	std::vector<std::string> initialData = { "One", "Two", "Three" };
	EXPECT_TRUE(ot::ContainerHelper::contains(initialData, std::string("One")));
	EXPECT_FALSE(ot::ContainerHelper::contains(initialData, std::string("Four")));
}

TEST(ContainerTests, ListVectorConvertTest) {
	std::list<int> initialData;
	initialData.push_back(1);
	initialData.push_back(2);
	initialData.push_back(3);

	std::vector<int> data1 = ot::ContainerHelper::toVector(initialData);
	EXPECT_EQ(initialData.size(), data1.size());

	std::list<int> data2 = ot::ContainerHelper::toList(data1);
	EXPECT_EQ(data1.size(), data2.size());

	int i = 0;
	for (int entry : data2) {
		EXPECT_EQ(data1[i++], entry);
	}
}

TEST(ContainerTests, DiffMissingLeft) {
	std::list<int> left = { 1, 2, 3 };
	std::list<int> right = { 3, 4, 5 };
	
	std::list<int> missing = ot::ContainerHelper::createDiff(left, right, ot::ContainerHelper::MissingLeft);
	std::list<int> expected = { 4, 5 };

	EXPECT_EQ(missing.size(), expected.size());
	auto it = missing.begin();
	for (int val : expected) {
		EXPECT_EQ(val, *it);
		it++;
	}
}

TEST(ContainerTests, DiffMissingRight) {
	std::list<int> left = { 1, 2, 3 };
	std::list<int> right = { 3, 4, 5 };

	std::list<int> missing = ot::ContainerHelper::createDiff(left, right, ot::ContainerHelper::MissingRight);
	std::list<int> expected = { 1, 2 };

	EXPECT_EQ(missing.size(), expected.size());
	auto it = missing.begin();
	for (int val : expected) {
		EXPECT_EQ(val, *it);
		it++;
	}
}

TEST(ContainerTests, DiffMissingBoth) {
	std::list<int> left = { 1, 2, 3 };
	std::list<int> right = { 3, 4, 5 };

	std::list<int> missing = ot::ContainerHelper::createDiff(left, right, ot::ContainerHelper::MissingEither);
	std::list<int> expected = { 1, 2, 4, 5 };

	EXPECT_EQ(missing.size(), expected.size());
	auto it = missing.begin();
	for (int val : expected) {
		EXPECT_EQ(val, *it);
		it++;
	}
}

TEST(ContainerTests, IsSubsetList) {
	std::list<int> list = { 1, 2, 3 };
	std::list<int> sub = { 1, 2 };
	std::list<int> invalid = { 3, 4 };

	EXPECT_TRUE(ot::ContainerHelper::isSubset(sub, list));
	EXPECT_FALSE(ot::ContainerHelper::isSubset(invalid, list));
}

TEST(ContainerTests, IsSubsetVector) {
	std::vector<int> list = { 1, 2, 3 };
	std::vector<int> sub = { 1, 2 };
	std::vector<int> invalid = { 3, 4 };

	EXPECT_TRUE(ot::ContainerHelper::isSubset(sub, list));
	EXPECT_FALSE(ot::ContainerHelper::isSubset(invalid, list));
}

TEST(ContainerTests, IntersectList) {
	std::list<int> l1 = { 1, 2, 3 };
	std::list<int> l2 = { 2, 3, 4 };
	std::list<int> l3 = { 3, 4, 5 };
	std::list<int> l4 = { 4, 5, 6 };

	std::list<int> intersect1 = ot::ContainerHelper::intersect(l1, l2);
	EXPECT_EQ(intersect1.size(), 2);
	EXPECT_TRUE(ot::ContainerHelper::isEqual(intersect1, std::list<int>({ 2, 3 })));

	std::list<int> intersect2 = ot::ContainerHelper::intersect(l1, l3);
	EXPECT_EQ(intersect2.size(), 1);
	EXPECT_TRUE(ot::ContainerHelper::isEqual(intersect2, std::list<int>({ 3 })));

	std::list<int> intersect3 = ot::ContainerHelper::intersect(l1, l4);
	EXPECT_EQ(intersect3.size(), 0);
	EXPECT_TRUE(intersect3.empty());
}

TEST(ContainerTests, IntersectVector) {
	std::vector<int> l1 = { 1, 2, 3 };
	std::vector<int> l2 = { 2, 3, 4 };
	std::vector<int> l3 = { 3, 4, 5 };
	std::vector<int> l4 = { 4, 5, 6 };

	std::vector<int> intersect1 = ot::ContainerHelper::intersect(l1, l2);
	EXPECT_EQ(intersect1.size(), 2);
	EXPECT_TRUE(ot::ContainerHelper::isEqual(intersect1, std::vector<int>({ 2, 3 })));
	
	std::vector<int> intersect2 = ot::ContainerHelper::intersect(l1, l3);
	EXPECT_EQ(intersect2.size(), 1);
	EXPECT_TRUE(ot::ContainerHelper::isEqual(intersect2, std::vector<int>({ 3 })));
	
	std::vector<int> intersect3 = ot::ContainerHelper::intersect(l1, l4);
	EXPECT_EQ(intersect3.size(), 0);
	EXPECT_TRUE(intersect3.empty());
}

TEST(ContainerTests, IsIntersectList) {
	std::list<int> l1 = { 1, 2 };
	std::list<int> l2 = { 2, 3 };
	std::list<int> l3 = { 3, 4 };

	EXPECT_TRUE(ot::ContainerHelper::hasIntersection(l1, l2));
	EXPECT_TRUE(ot::ContainerHelper::hasIntersection(l2, l3));
	EXPECT_FALSE(ot::ContainerHelper::hasIntersection(l1, l3));
}

TEST(ContainerTests, IsIntersectVector) {
	std::vector<int> l1 = { 1, 2 };
	std::vector<int> l2 = { 2, 3 };
	std::vector<int> l3 = { 3, 4 };

	EXPECT_TRUE(ot::ContainerHelper::hasIntersection(l1, l2));
	EXPECT_TRUE(ot::ContainerHelper::hasIntersection(l2, l3));
	EXPECT_FALSE(ot::ContainerHelper::hasIntersection(l1, l3));
}

TEST(ContainerTests, EqualList) {
	// Hashing
	std::list<int> l1 = { 1, 2 };
	std::list<int> l2 = { 1, 2, 3 };
	std::list<int> l3 = { 2, 1, 3 };
	std::list<int> l4 = { 1, 3, 2 };
	std::list<int> l5 = { 1, 3, 2, 2 };

	// Sorting
	std::list<std::vector<int>> v1 = { {1,2}, {3,4} };
	std::list<std::vector<int>> v2 = { {1,2}, {3,4}, {5,6} };
	std::list<std::vector<int>> v3 = { {3,4}, {5,6}, {1,2} };
	std::list<std::vector<int>> v4 = { {5,6}, {1,2}, {3,4} };
	std::list<std::vector<int>> v5 = { {5,6}, {1,2}, {3,4}, {1,2} };

	EXPECT_FALSE(ot::ContainerHelper::isEqual(l1, l2));
	EXPECT_TRUE(ot::ContainerHelper::isEqual(l2, l3));
	EXPECT_TRUE(ot::ContainerHelper::isEqual(l2, l4));
	EXPECT_TRUE(ot::ContainerHelper::isEqual(l3, l4));
	EXPECT_FALSE(ot::ContainerHelper::isEqual(l2, l5));

	EXPECT_FALSE(ot::ContainerHelper::isEqual(v1, v2));
	EXPECT_TRUE(ot::ContainerHelper::isEqual(v2, v3));
	EXPECT_TRUE(ot::ContainerHelper::isEqual(v2, v4));
	EXPECT_TRUE(ot::ContainerHelper::isEqual(v3, v4));
	EXPECT_FALSE(ot::ContainerHelper::isEqual(v2, v5));
}

TEST(ContainerTests, EqualVector) {
	// Hashing
	std::vector<int> l1 = { 1, 2 };
	std::vector<int> l2 = { 1, 2, 3 };
	std::vector<int> l3 = { 2, 1, 3 };
	std::vector<int> l4 = { 1, 3, 2 };
	std::vector<int> l5 = { 1, 3, 2, 2 };

	// Sorting
	std::vector<std::vector<int>> v1 = { {1,2}, {3,4} };
	std::vector<std::vector<int>> v2 = { {1,2}, {3,4}, {5,6} };
	std::vector<std::vector<int>> v3 = { {3,4}, {5,6}, {1,2} };
	std::vector<std::vector<int>> v4 = { {5,6}, {1,2}, {3,4} };
	std::vector<std::vector<int>> v5 = { {5,6}, {1,2}, {3,4}, {1,2} };

	EXPECT_FALSE(ot::ContainerHelper::isEqual(l1, l2));
	EXPECT_TRUE(ot::ContainerHelper::isEqual(l2, l3));
	EXPECT_TRUE(ot::ContainerHelper::isEqual(l2, l4));
	EXPECT_TRUE(ot::ContainerHelper::isEqual(l3, l4));
	EXPECT_FALSE(ot::ContainerHelper::isEqual(l2, l5));

	EXPECT_FALSE(ot::ContainerHelper::isEqual(v1, v2));
	EXPECT_TRUE(ot::ContainerHelper::isEqual(v2, v3));
	EXPECT_TRUE(ot::ContainerHelper::isEqual(v2, v4));
	EXPECT_TRUE(ot::ContainerHelper::isEqual(v3, v4));
	EXPECT_FALSE(ot::ContainerHelper::isEqual(v2, v5));
}