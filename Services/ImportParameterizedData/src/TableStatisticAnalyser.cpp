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

