#pragma once
#include <string>
#include "CSVProperties.h"
#include <vector>
#include "OTCore/GenericDataStructMatrix.h"
class CSVToTableTransformer
{
public:
	ot::GenericDataStructMatrix operator() (const std::string& _csvText, const CSVProperties& _properties) const;

private:
	std::vector<std::string> splitByDelimiter(const std::string& _text, const std::string& _delimiter) const;
	uint64_t countStringInString(const std::string& _text, const std::string& _searchCriteria) const;

};
