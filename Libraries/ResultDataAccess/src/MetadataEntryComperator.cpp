#include "MetadataEntryComperator.h"
#include "MetadataEntryArray.h"
#include "MetadataEntrySingle.h"
#include "MetadataEntryObject.h"

bool MetadataEntryComperator::operator()(std::shared_ptr<MetadataEntry> one, std::shared_ptr<MetadataEntry> two)
{
	auto otherMetadataEntryArray = dynamic_cast<MetadataEntryArray*>(one.get());
	if (otherMetadataEntryArray != nullptr)
	{
		auto metadataEntryArray = dynamic_cast<MetadataEntryArray*>(two.get());
		if (metadataEntryArray == nullptr)
		{
			return false;
		}
		else
		{
			return (*otherMetadataEntryArray) == (*metadataEntryArray);
		}
	}

	auto otherMetadataEntrySingle = dynamic_cast<MetadataEntrySingle*>(one.get());
	if (otherMetadataEntrySingle != nullptr)
	{
		auto metadataEntrySingle = dynamic_cast<MetadataEntrySingle*>(two.get());
		if (metadataEntrySingle == nullptr)
		{
			return false;
		}
		else
		{
			return (*otherMetadataEntrySingle) == (*metadataEntrySingle);
		}
	}

	auto otherMetadataEntryObject = dynamic_cast<MetadataEntryObject*>(one.get());
	if (otherMetadataEntryObject != nullptr)
	{
		auto metadataEntryObject = dynamic_cast<MetadataEntryObject*>(two.get());
		if (metadataEntryObject == nullptr)
		{
			return false;
		}
		else
		{
			return (*otherMetadataEntryObject) == (*metadataEntryObject);
		}
	}

	return false;
}
