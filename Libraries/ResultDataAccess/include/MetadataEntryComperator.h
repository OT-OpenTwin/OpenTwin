#pragma once
#include "MetadataEntry.h"
#include <memory>

class MetadataEntryComperator
{
public:
	bool operator()(std::shared_ptr<MetadataEntry> one, std::shared_ptr<MetadataEntry> two);
};
