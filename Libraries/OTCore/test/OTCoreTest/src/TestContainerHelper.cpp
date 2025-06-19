#include "gtest/gtest.h"
#include "OTCore/ContainerHelper.h"
#include <string>

TEST(ContainerTests, MapTest) {
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

TEST(ContainerTests, ListVectorTest) {
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

TEST(ContainerTests, SubsetList) {
	std::list<int> list = { 1, 2, 3 };
	std::list<int> sub = { 1, 2 };
	std::list<int> invalid = { 3, 4 };

	EXPECT_TRUE(ot::ContainerHelper::isSubset(sub, list));
	EXPECT_FALSE(ot::ContainerHelper::isSubset(invalid, list));
}

TEST(ContainerTests, SubsetVector) {
	std::vector<int> list = { 1, 2, 3 };
	std::vector<int> sub = { 1, 2 };
	std::vector<int> invalid = { 3, 4 };

	EXPECT_TRUE(ot::ContainerHelper::isSubset(sub, list));
	EXPECT_FALSE(ot::ContainerHelper::isSubset(invalid, list));
}