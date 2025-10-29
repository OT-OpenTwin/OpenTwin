// @otlicense

#pragma once
#include "MetadataEntry.h"
#include "OTCore/Variable.h"

class MetadataEntrySingle : public MetadataEntry
{
public:
	MetadataEntrySingle(const std::string& name, ot::Variable&& value) : MetadataEntry(name), _value(value){}
	MetadataEntrySingle(const std::string& name, const ot::Variable& value) : MetadataEntry(name), _value(value){}
	const ot::Variable& getValue() const { return _value; }
	bool operator==(const MetadataEntrySingle& other);

private:
	const ot::Variable _value;
};
