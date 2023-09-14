#pragma once
#include <string>

class MetadataEntry
{
public:
	MetadataEntry(const std::string& entryName):_entryName(entryName){}
	virtual const std::string& getEntryName() const { return _entryName; }
private:
	std::string _entryName;
};
