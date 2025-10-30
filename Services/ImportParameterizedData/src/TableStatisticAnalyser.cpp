// @otlicense
// File: TableStatisticAnalyser.cpp
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

#include "TableStatisticAnalyser.h"
#include <list>    

uint64_t TableStatisticAnalyser::CountColumns()
{
	_extractor->ResetIterator();
	uint64_t columns = 0;
	while (_extractor->HasNextLine())
	{
		std::vector<std::string> t;
		_extractor->GetNextLine(t);
		if (t.size() > columns)
		{
			columns = t.size();
		}
	}
	_extractor->ResetIterator();
	return columns;
}

uint64_t TableStatisticAnalyser::CountRows()
{
	std::string t;
	uint64_t counter = 0;
	_extractor->ResetIterator();
	while (_extractor->HasNextLine())
	{
		_extractor->GetNextLine(t);
		counter++;
	}
	_extractor->ResetIterator();
	return counter;
}

int TableStatisticAnalyser::CountUniquesInColumn(int column)
{
	std::list<std::string> uniques;

	_extractor->ResetIterator();
	while (_extractor->HasNextLine())
	{
		std::vector<std::string> lineSegments;
		_extractor->GetNextLine(lineSegments);
		uniques.insert(uniques.begin(),lineSegments[column]);		
	}
	uniques.unique();
	_extractor->ResetIterator();
	return static_cast<int>(uniques.size());
}

std::vector<uint64_t> TableStatisticAnalyser::CountUniquesInColumns()
{
	int64_t columns = CountColumns();
	std::vector <std::list<std::string>> uniques(columns);

	std::vector<uint64_t> listOfUniqueEntries;
	_extractor->ResetIterator();
	while (_extractor->HasNextLine())
	{
		std::vector<std::string> lineSegments;
		_extractor->GetNextLine(lineSegments);

		for (uint64_t i = 0; i < lineSegments.size(); i++)
		{
			uniques[i].insert(uniques[i].end(), lineSegments[i]);
		}
	}
	listOfUniqueEntries.reserve(columns);
	for (auto column : uniques)
	{
		column.unique();
		listOfUniqueEntries.push_back(column.size());
	}
	return listOfUniqueEntries;
}

