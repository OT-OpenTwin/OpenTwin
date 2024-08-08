#include <gtest/gtest.h>
#include "FixtureBusinessLogicHandler.h"

TEST_F(FixtureBusinessLogicHandler, NumberOfCreatedNames)
{
	std::list<std::string> nothingTaken{};
	auto result =	SetAnother5Names(nothingTaken);
	EXPECT_EQ(nothingTaken.size(), 5);
	EXPECT_EQ(result.size(), 5);
}

TEST_F(FixtureBusinessLogicHandler, NumberOfCreatedNamesWithTakenNames)
{
	std::list<std::string> takenNames{"NotReally", "ButStill", "Works"};
	auto result = SetAnother5Names(takenNames);
	EXPECT_EQ(takenNames.size(), 8);
	EXPECT_EQ(result.size(), 5);
}

TEST_F(FixtureBusinessLogicHandler, GetNextFreeNames)
{
	std::list<std::string> nothingTaken{};
	auto result = SetAnother5Names(nothingTaken);
	std::list<std::string> expected
	{
		getFolderName()+"/"+getFileName(),
		getFolderName() + "/" + getFileName() + "_1",
		getFolderName() + "/" + getFileName() + "_2",
		getFolderName() + "/" + getFileName() + "_3",
		getFolderName() + "/" + getFileName() + "_4",
	};

	
	auto resultName = result.begin();
	for (auto it = expected.begin(); it != expected.end(); it++)
	{
		EXPECT_EQ(*resultName, *it);
		resultName++;
	}
}

TEST_F(FixtureBusinessLogicHandler, GetNextFreeNamesAllNumbered)
{
	std::list<std::string> nothingTaken{};
	auto result = SetAnother5NamesAlwaysNumbered(nothingTaken);
	std::list<std::string> expected
	{
		getFolderName() + "/" + getFileName() + "_1",
		getFolderName() + "/" + getFileName() + "_2",
		getFolderName() + "/" + getFileName() + "_3",
		getFolderName() + "/" + getFileName() + "_4",
		getFolderName() + "/" + getFileName() + "_5"
	};


	auto resultName = result.begin();
	for (auto it = expected.begin(); it != expected.end(); it++)
	{
		EXPECT_EQ(*resultName, *it);
		resultName++;
	}
}


TEST_F(FixtureBusinessLogicHandler, GetNextFreeNamesStartWithNonDefaultNumber)
{
	std::list<std::string> nothingTaken{};
	auto result = SetAnother5NamesStartWith7(nothingTaken);
	std::list<std::string> expected
	{
		getFolderName() + "/" + getFileName(),
		getFolderName() + "/" + getFileName() + "_7",
		getFolderName() + "/" + getFileName() + "_8",
		getFolderName() + "/" + getFileName() + "_9",
		getFolderName() + "/" + getFileName() + "_10",
	};

	auto resultName = result.begin();
	for (auto it = expected.begin(); it != expected.end(); it++)
	{
		EXPECT_EQ(*resultName, *it);
		resultName++;
	}
}


TEST_F(FixtureBusinessLogicHandler, FillTheGabs)
{
	std::list<std::string> takenNames
	{
		getFolderName() + "/" + getFileName(),
		//getFolderName() + "/" + getFileName() + "_1",
		getFolderName() + "/" + getFileName() + "_2",
		//getFolderName() + "/" + getFileName() + "_3",
		getFolderName() + "/" + getFileName() + "_4",
	};
	auto result = SetAnother5Names(takenNames);

	std::list<std::string> expected
	{
		getFolderName() + "/" + getFileName(),
		getFolderName() + "/" + getFileName() + "_1",
		getFolderName() + "/" + getFileName() + "_2",
		getFolderName() + "/" + getFileName() + "_3",
		getFolderName() + "/" + getFileName() + "_4",
		getFolderName() + "/" + getFileName() + "_5",
		getFolderName() + "/" + getFileName() + "_6",
		getFolderName() + "/" + getFileName() + "_7",
	};
	takenNames.sort();
	auto takenName = takenNames.begin();
	for (auto it = expected.begin(); it != expected.end(); it++)
	{
		EXPECT_EQ(*takenName, *it);
		takenName++;
	}
}
