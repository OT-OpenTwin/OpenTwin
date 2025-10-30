// @otlicense

#pragma once
#include <string>
#include <vector>
#include "EntityFile.h"

//! @brief API for creating a Table entity from any source. 
//! The task of this class is only the extraction of single rows, depending in the set delimiter. 
//! The contained strings are not altered in any way.
class TableExtractor
{
public:
	TableExtractor() {};
	virtual ~TableExtractor() {};
	
	virtual void ExtractFromEntitySource(EntityFile* source) = 0;

	virtual void GetNextLine(std::vector<std::string> & line)=0;
	virtual void GetNextLine(std::string & entireLine)=0;
	virtual bool HasNextLine()=0;
	virtual void ResetIterator()=0;
};