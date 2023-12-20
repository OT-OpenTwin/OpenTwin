#include "MetadataEntryArray.h"

bool MetadataEntryArray::operator==(const MetadataEntryArray& other)
{
	return getEntryName() == other.getEntryName() && _values == other._values;
}
