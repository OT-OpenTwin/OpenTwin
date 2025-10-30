// @otlicense
// File: TableExtractorCSV.h
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

#include <sstream>
#include "TableExtractor.h"
#include "EntityFileCSV.h"

//! @brief Concrete Importer which interpretes a csv file.
class TableExtractorCSV : public TableExtractor
{
public:
	~TableExtractorCSV();
	TableExtractorCSV() {};

	void ExtractFromEntitySource(EntityFile * source) override;
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

