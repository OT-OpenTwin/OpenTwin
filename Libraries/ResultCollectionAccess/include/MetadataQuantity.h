#pragma once
#include <stdint.h>
#include <string>
#include <map>
#include <list>
#include <memory>

#include "OTCore/Variable.h"
#include "MetadataEntry.h"

struct __declspec(dllexport) MetadataQuantity
{
	std::string quantityName;
	std::string quantityAbbreviation;
	std::string typeName;
	uint64_t quantityIndex;
	std::map < std::string, std::shared_ptr<MetadataEntry>> metaData;
	static const std::string getFieldName() { return "Quantity"; }
};
