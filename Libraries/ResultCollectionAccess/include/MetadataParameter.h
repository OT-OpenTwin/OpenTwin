#pragma once
#include "OpenTwinCore/Variable.h"
#include "MetadataEntry.h"

#include <string>
#include <list>
#include <map>
#include <stdint.h>

struct MetadataParameter
{
	std::string parameterName;
	std::string parameterAbbreviation;
	std::list<ot::Variable> values;
	
	std::map < std::string, std::shared_ptr<MetadataEntry>> metaData;
};