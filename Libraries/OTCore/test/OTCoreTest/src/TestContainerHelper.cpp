#include "gtest/gtest.h"
#include "OTCore/ContainerHelper.h"
#include <string>

TEST(ContainerTests, MapTest) {
	//std::map<int, char> initialData;
	//initialData.insert_or_assign(1, 'A');
	//initialData.insert_or_assign(2, 'B');
	//initialData.insert_or_assign(3, 'C');

	//std::list<int> keyList = ot::listFromMapKeys(initialData);
	//std::list<char> valueList = ot::listFromMapValues(initialData);

	//EXPECT_EQ(keyList.size(), initialData.size());
	//EXPECT_EQ(keyList.size(), valueList.size());

	//auto it = valueList.begin();
	//for (int k : keyList) {
	//	EXPECT_NE(initialData.find(k), initialData.end());
	//	EXPECT_EQ(initialData[k], *it);
	//	it++;
	//}
}

TEST(ContainerTests, ListVectorTest) {
	//std::list<int> initialData;
	//initialData.push_back(1);
	//initialData.push_back(2);
	//initialData.push_back(3);

	//std::vector<int> data1 = ot::vectorFromList(initialData);
	//EXPECT_EQ(initialData.size(), data1.size());

	//std::list<int> data2 = ot::listFromVector(data1);
	//EXPECT_EQ(data1.size(), data2.size());

	//int i = 0;
	//for (int entry : data2) {
	//	EXPECT_EQ(data1[i++], entry);
	//}
}