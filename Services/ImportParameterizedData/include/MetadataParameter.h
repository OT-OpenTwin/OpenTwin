#pragma once
#include "OTCore/Variable.h"

#include <string>
#include <list>
#include <map>
#include <stdint.h>
#include <vector>

struct MetadataParameter
{
	std::string parameterName;
	std::string parameterAbbreviation;
	std::list<ot::Variable> uniqueValues;
	std::vector<ot::Variable> selectedValues;
	std::map<ot::Variable, uint32_t> valueIndices;
};