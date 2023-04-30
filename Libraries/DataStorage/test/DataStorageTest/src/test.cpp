#include <iostream>
#include "gtest/gtest.h"

//#include "Connection/ConnectionAPI.h"

using namespace std;

TEST(DataStorage, Test_One) {
	//DataStorageAPI::ConnectionAPI &test = DataStorageAPI::ConnectionAPI::getInstance();
	//DataStorageAPI::callMe();

  EXPECT_EQ(1, 1);
  EXPECT_TRUE(true);
}

TEST(DataStorage, Test_Two) {
	EXPECT_EQ(1, 1);
	EXPECT_TRUE(true);
}

TEST(DataStorage, Test_Three) {
	EXPECT_EQ(1, 0);
	EXPECT_TRUE(true);
}

TEST(DataStorage, Test_Four) {
	EXPECT_EQ(1, 0);
	EXPECT_TRUE(true);
}