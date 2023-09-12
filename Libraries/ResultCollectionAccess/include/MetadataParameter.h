#pragma once
#include "OpenTwinCore/Variable.h"

#include <string>
#include <list>
#include <map>
#include <stdint.h>
#include <vector>

struct MetadataParameter
{
	std::string parameterName;
	std::string parameterAbbreviation;
	std::vector<ot::Variable> values;
	
	std::map < std::string, std::list<ot::Variable>> metaData;
};