// @otlicense

#include "MetadataEntryObject.h"
#include "MetadataEntryComperator.h"

bool MetadataEntryObject::operator==(const MetadataEntryObject& other)
{
	if (getEntryName() != other.getEntryName() || other._values.size() != _values.size())
	{
		return false;
	}
	else
	{
		auto thisValue = _values.begin();
		MetadataEntryComperator comperator;
		for (auto otherValue : other._values)
		{
			const bool equal = comperator(*thisValue, otherValue);
			if (!equal) { return equal; }
		}
		return true;
	}
}
