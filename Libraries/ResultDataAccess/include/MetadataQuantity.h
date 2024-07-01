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
	std::string quantityLabel;
	std::string quantityAbbreviation;
	uint64_t quantityIndex;

	std::string typeName;
	std::string unit;

	uint32_t dataColumns = 1;
	uint32_t dataRows = 1;
	
	std::map < std::string, std::shared_ptr<MetadataEntry>> metaData;
	static const std::string getFieldName() { return "Quantity"; }
};
