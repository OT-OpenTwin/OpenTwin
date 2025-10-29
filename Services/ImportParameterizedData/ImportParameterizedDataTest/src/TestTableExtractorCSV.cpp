// @otlicense

#include <iostream>
#include "FixtureTableExtractorCSV.h"


TEST_F(FixtureTableExtractorCSV, FileCouldNotBeOpened) {
	std::string wrongPath = getFilePath() + "KuchenSuppe.csv";
	EXPECT_ANY_THROW(loadFileContent(wrongPath));
}

TEST_P(FixtureTableExtractorCSV, TestColumnDelimiter)
{

	int delimiter = GetParam();
	const int numberOfRows = 3;
	std::string expectedLines[numberOfRows];

	if (delimiter == 0)
	{
		std::string fullPath = getFilePath() + "WorkingTestTableComma.csv";
		loadFileContent(fullPath);
		//The files were create on a windows machine with \r\n for newline. 
		//Trailing control character are being sorted out on the next level, since the table extractors task is only returning the text chunks regarding the set delimiter.
		expectedLines[0] = "FirstColumn,SecondColumn,ThirdColumn,FourthColumn,FifthColumn,SixthColumn,SeventhColumn,EighthColumn,NinthColumn,TenthColumn\r";
		expectedLines[1] = "C1R2,C2R2,C3R2,C4R2,C5R2,C6R2,C7R2,C8R2,C9R2,C10R2\r";
		expectedLines[2] = "C1R3,C2R3,C3R3,C4R3,C5R3,C6R3,C7R3,C8R3,C9R3,C10R3";
	}
	else
	{
		std::string fullPath = getFilePath() + "WorkingTestTableColumnDelimiterSemicolon.csv";
		loadFileWithSemicolonRowDelimiter(fullPath);
		expectedLines[0] = "FirstColumn,SecondColumn,ThirdColumn,FourthColumn,FifthColumn,SixthColumn,SeventhColumn,EighthColumn,NinthColumn,TenthColumn";
		expectedLines[1] = "C1R2,C2R2,C3R2,C4R2,C5R2,C6R2,C7R2,C8R2,C9R2,C10R2";
		expectedLines[2] = "C1R3,C2R3,C3R3,C4R3,C5R3,C6R3,C7R3,C8R3,C9R3,C10R3";
	}

	auto allRows = getAllRows();
	EXPECT_EQ(allRows.size(), numberOfRows);


	for (int i =0; i< numberOfRows; i++)
	{
 		EXPECT_EQ(allRows[i], expectedLines[i]);
	}
}

TEST_P(FixtureTableExtractorCSV, TestRowDelimiter)
{
	std::string fullPath;
	int delimiter = GetParam();

	if (delimiter == 0)
	{
		fullPath = getFilePath() + "WorkingTestTableComma.csv";
		loadFileWithCommaColumnDelimiter(fullPath);

	}
	else if (delimiter == 1)
	{
		fullPath = getFilePath() + "WorkingTestTableTab.csv";
		loadFileWithTabColumnDelimiter(fullPath);
	}
	else
	{
		fullPath = getFilePath() + "WorkingTestTableSemicolon.csv";
		loadFileContent(fullPath);
	}
	
	auto allColumns = getAllColumns();
	const int numberOfColumns = 10;
	EXPECT_EQ(allColumns.size(), numberOfColumns);

	const std::string expectedLines[numberOfColumns] = {
		"FirstColumn",
		"SecondColumn",
		"ThirdColumn",
		"FourthColumn",
		"FifthColumn",
		"SixthColumn",
		"SeventhColumn",
		"EighthColumn",
		"NinthColumn",
		"TenthColumn\r"
	};

	for (int i = 0; i < numberOfColumns; i++)
	{
		EXPECT_EQ(allColumns[i], expectedLines[i]);
	}
}

TEST_F(FixtureTableExtractorCSV, TestMaskedDelimiter)
{
	std::string fullPath = getFilePath() + "WorkingTestMaskedDelimitter.csv";
	loadFileWithCommaColumnDelimiter(fullPath);
	const int numberOfRows = 12;
	//Currently ALL " chars are being removed.
	std::string expectedLines[numberOfRows] = { 
		"First,Column",
		"Second,Column,",
		"ThirdColumn",
		"FourthColumn",
		"FifthColumn",
		"SixthColumn",
		"SeventhColumn",
		"EighthColumn",
		",NinthColumn",
		"",
		"TenthColumn",
		""};
	auto allRows = getAllColumns();
	EXPECT_EQ(allRows.size(), numberOfRows);

	for (int i = 0; i < numberOfRows; i++)
	{
		EXPECT_EQ(allRows[i], expectedLines[i]);
	}
}
