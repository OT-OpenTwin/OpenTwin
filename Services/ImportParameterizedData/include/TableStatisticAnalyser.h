// @otlicense

#pragma once
#include "TableExtractor.h"

//! @brief Takes care of calculating the caracteristics of a table behind a TableExtractor object.
class TableStatisticAnalyser
{
private:
	TableExtractor * const _extractor = nullptr;

public:
	TableStatisticAnalyser(TableExtractor * extractor)
		: _extractor(extractor)
	{
		if (_extractor == nullptr)
		{
			throw std::logic_error("Nullptr not supported");
		}
	};
	
	uint64_t CountColumns();
	uint64_t CountRows();
	int CountUniquesInColumn(int column);
	std::vector<uint64_t> CountUniquesInColumns();
};