// @otlicense

#pragma once
#include "MetadataEntry.h"
#include <list>
#include <memory>

class MetadataEntryObject : public MetadataEntry
{
public:
	MetadataEntryObject(const std::string& name) :MetadataEntry(name) {}
	const std::list<std::shared_ptr<MetadataEntry>>& getEntries() const { return _values; };
	void AddMetadataEntry(std::shared_ptr<MetadataEntry> entry) { _values.push_back(entry); }
	void AddMetadataEntry(std::list<std::shared_ptr<MetadataEntry>> entries) { _values.splice(_values.end(), entries); }
	bool operator==(const MetadataEntryObject& other);
private:
	std::list<std::shared_ptr<MetadataEntry>> _values;
};
