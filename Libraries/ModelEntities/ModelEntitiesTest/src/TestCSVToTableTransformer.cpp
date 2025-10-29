// @otlicense

#include <iostream>
#include "FixtureCSVToTableTransformer.h"

TEST_F(FixtureCSVToTableTransformer, FileCouldNotBeOpened) {
	std::string wrongPath = getFilePath() + "KuchenSuppe.csv";
	EXPECT_ANY_THROW(loadFileContent(wrongPath));
}

TEST_P(FixtureCSVToTableTransformer, TestColumnDelimiter)
{

	int delimiter = GetParam();
	const int numberOfRows = 3;
	std::string expectedLines[numberOfRows];
	std::string text("");
	CSVProperties csvProperties;
	if (delimiter == 0)
	{
		std::string fullPath = getFilePath() + "WorkingTestTableComma.csv";
		text = loadFileContent(fullPath);
		csvProperties.m_rowDelimiter = "\n";
		csvProperties.m_columnDelimiter = ",";
		//The files were create on a windows machine with \r\n for newline. 
		//Trailing control character are being sorted out on the next level, since the table extractors task is only returning the text chunks regarding the set delimiter.
		expectedLines[0] = "FirstColumn,SecondColumn,ThirdColumn,FourthColumn,FifthColumn,SixthColumn,SeventhColumn,EighthColumn,NinthColumn,TenthColumn\r";
		expectedLines[1] = "C1R2,C2R2,C3R2,C4R2,C5R2,C6R2,C7R2,C8R2,C9R2,C10R2\r";
		expectedLines[2] = "C1R3,C2R3,C3R3,C4R3,C5R3,C6R3,C7R3,C8R3,C9R3,C10R3";
	}
	else
	{
		std::string fullPath = getFilePath() + "WorkingTestTableColumnDelimiterSemicolon.csv";
		text = loadFileContent(fullPath);
		csvProperties.m_columnDelimiter = ",";
		csvProperties.m_rowDelimiter = ";";
		
		expectedLines[0] = "FirstColumn,SecondColumn,ThirdColumn,FourthColumn,FifthColumn,SixthColumn,SeventhColumn,EighthColumn,NinthColumn,TenthColumn";
		expectedLines[1] = "C1R2,C2R2,C3R2,C4R2,C5R2,C6R2,C7R2,C8R2,C9R2,C10R2";
		expectedLines[2] = "C1R3,C2R3,C3R3,C4R3,C5R3,C6R3,C7R3,C8R3,C9R3,C10R3";
	}

	ot::GenericDataStructMatrix table =	getTable(text, csvProperties);
	EXPECT_EQ(table.getNumberOfRows(), numberOfRows);

	ot::MatrixEntryPointer matrixPointer;
	
	for (matrixPointer.m_row = 0; matrixPointer.m_row < table.getNumberOfRows(); matrixPointer.m_row++)
	{
		std::string row("");
		for (matrixPointer.m_column = 0; matrixPointer.m_column < table.getNumberOfColumns(); matrixPointer.m_column++)
		{
			ot::Variable value = table.getValue(matrixPointer);
			const std::string stringValue = value.getConstCharPtr();
			if (matrixPointer.m_column == table.getNumberOfColumns() - 1)
			{
				row += stringValue;
			}
			else
			{
				row += stringValue + csvProperties.m_columnDelimiter;
			}
		}
		EXPECT_EQ(expectedLines[matrixPointer.m_row], row);
	}
}

TEST_P(FixtureCSVToTableTransformer, TestRowDelimiter)
{
	std::string fullPath;
	int delimiter = GetParam();
	std::string text("");
	CSVProperties csvProperties;
	if (delimiter == 0)
	{
		fullPath = getFilePath() + "WorkingTestTableComma.csv";
		text = loadFileContent(fullPath);
		csvProperties.m_rowDelimiter = "\n";
		csvProperties.m_columnDelimiter = ",";
	}
	else if (delimiter == 1)
	{
		fullPath = getFilePath() + "WorkingTestTableTab.csv";
		text = loadFileContent(fullPath);
		csvProperties.m_rowDelimiter = "\n";
		csvProperties.m_columnDelimiter = "\t";
	}
	else
	{
		fullPath = getFilePath() + "WorkingTestTableSemicolon.csv";
		text = loadFileContent(fullPath);
		csvProperties.m_rowDelimiter = "\n";
		csvProperties.m_columnDelimiter = ";";
	}

	ot::GenericDataStructMatrix table =	getTable(text, csvProperties);

	const int numberOfColumns = 10;
	EXPECT_EQ(table.getNumberOfColumns(), numberOfColumns);

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

	ot::MatrixEntryPointer matrixEntry;
	matrixEntry.m_row = 0;
	for (matrixEntry.m_column= 0; matrixEntry.m_column < numberOfColumns; matrixEntry.m_column++)
	{
		ot::Variable entry = table.getValue(matrixEntry);
		std::string cellValue( entry.getConstCharPtr());
		EXPECT_EQ(cellValue, expectedLines[matrixEntry.m_column]);
	}
}

TEST_F(FixtureCSVToTableTransformer, TestMaskedDelimiter)
{
	std::string fullPath = getFilePath() + "WorkingTestMaskedDelimitter.csv";
	std::string text = loadFileContent(fullPath);
	CSVProperties csvProperties;
	csvProperties.m_columnDelimiter = ",";
	csvProperties.m_rowDelimiter = "\n";
	const int numberOfColumns= 12;
	//Currently ALL " chars are being removed.
	std::string expectedLines[numberOfColumns] = {
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
		"\"" };
	ot::GenericDataStructMatrix table =	getTable(text, csvProperties);
	EXPECT_EQ(table.getNumberOfColumns(), numberOfColumns);

	ot::MatrixEntryPointer matrixEntry;
	matrixEntry.m_row = 0;
	
	for (matrixEntry.m_column= 0; matrixEntry.m_column< numberOfColumns; matrixEntry.m_column++)
	{
		ot::Variable value = table.getValue(matrixEntry);
		std::string cellEntry(value.getConstCharPtr());
		EXPECT_EQ(cellEntry, expectedLines[matrixEntry.m_column]);
	}
}
