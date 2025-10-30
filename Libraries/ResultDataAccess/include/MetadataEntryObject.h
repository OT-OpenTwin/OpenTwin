// @otlicense
// File: MetadataEntryObject.h
// 
// License:
// Copyright 2025 by OpenTwin
//  
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
//  
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// @otlicense-end

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
