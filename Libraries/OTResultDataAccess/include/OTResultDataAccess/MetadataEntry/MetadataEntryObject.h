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

// OpenTwin header
#include "OTResultDataAccess/MetadataEntry/MetadataEntry.h"

// std header
#include <list>
#include <memory>

class MetadataEntryObject : public MetadataEntry
{
public:
	MetadataEntryObject(const std::string& _name) : MetadataEntry(_name) {}
	const std::list<std::shared_ptr<MetadataEntry>>& getEntries() const { return m_values; };
	void AddMetadataEntry(std::shared_ptr<MetadataEntry> _entry) { m_values.push_back(_entry); }
	void AddMetadataEntry(std::list<std::shared_ptr<MetadataEntry>> _entries) { m_values.splice(m_values.end(), _entries); }
	bool operator==(const MetadataEntryObject& _other);
private:
	std::list<std::shared_ptr<MetadataEntry>> m_values;
};
