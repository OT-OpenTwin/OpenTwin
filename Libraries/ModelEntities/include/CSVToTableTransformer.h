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
	ot::GenericDataStructMatrix transformRawMatrixToGenericDatastruct(const std::list<std::vector<std::string>>& _rawMatrix);
};
