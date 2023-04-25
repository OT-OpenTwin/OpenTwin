/*****************************************************************//**
 * \file   TableExtractorCSV.h
 * \brief  Concrete Importer which interpretes a csv file.
 * 
 * \author Wagner
 * \date   February 2023
 *********************************************************************/
#include <sstream>
#include "TableExtractor.h"
#include "EntityParameterizedDataSourceCSV.h"

class TableExtractorCSV : public TableExtractor
{
public:
	~TableExtractorCSV();
	TableExtractorCSV() {};

	void ExtractFromEntitySource(EntityParameterizedDataSource * source) override;
	/**
	 * Directly sets the content of a file to fileContentStream. Only for testing purposes.
	 * 
	 * \param fileName
	 */
	void SetFileContent(std::vector<char> fileContent);
	void GetNextLine(std::vector<std::string> & line) override;
	void GetNextLine(std::string & entireLine) override;
	bool HasNextLine() override;
	void ResetIterator() override;

	void SetRowDelimiter(const char delimiter) ;
	void SetColumnDelimiter(const char delimiter);
private:
	std::istringstream _fileContentStream;
	char _columnDelimiter = ';';
	char _rowDelimiter = '\n';
	int _numberOfColumns = 0;
	int _numberOfRows = 0;

	void Split(std::string & line, std::vector<std::string>& lineSegments);
};

