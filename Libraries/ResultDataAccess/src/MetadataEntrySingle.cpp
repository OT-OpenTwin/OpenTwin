// @otlicense

#include "MetadataEntrySingle.h"

bool MetadataEntrySingle::operator==(const MetadataEntrySingle& other)
{
	return getEntryName() == other.getEntryName() && _value == other.getValue();
}
