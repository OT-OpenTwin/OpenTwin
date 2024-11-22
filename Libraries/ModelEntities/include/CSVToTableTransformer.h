#pragma once
#include <string>
#include "CSVProperties.h"
#include <vector>
#include "OTCore/GenericDataStructMatrix.h"
class CSVToTableTransformer
{
public:
	ot::GenericDataStructMatrix operator() (const std::string& _csvText, const CSVProperties& _properties);
	std::string operator() (const ot::GenericDataStructMatrix& _matrix, const CSVProperties& _properties);

private:
	char m_maskingChar = '"';
	std::vector<std::string> m_segments;
	std::string m_composed = "";
	
	void clearBuffer();
	void buildSegment(const std::string& _subString, const std::string& _delimiter, bool _lastSegment = true);
	uint64_t countStringInString(const std::string& _text, const std::string& _searchCriteria) ;
	std::vector<std::string> splitByDelimiter(const std::string& _text, const std::string& _delimiter) ;

	ot::GenericDataStructMatrix transformRawMatrixToGenericDatastruct(const std::list<std::vector<std::string>>& _rawMatrix);
};
